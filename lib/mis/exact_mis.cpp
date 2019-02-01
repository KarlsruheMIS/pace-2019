/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/
#include <limits.h>
// #include <algorithm>

#include "exact_mis.h"

#include "full_reductions.h"
#include "mis_log.h"
#include "diversifier.h"
#include "cover_combine.h"
#include "multiway_combine.h"
#include "separator_combine.h"
#include "graph_io.h"
#include "timer.h"
#include "mis_config.h"
#include "population_mis.h"
#include "separator_pool.h"
#include "data_structure/graph_access.h"
#include "mis/kernel/branch_and_reduce_algorithm.h"
#include "ils.h"
#include "omp.h"

std::vector<bool> getExactMIS(const std::vector<std::vector<int>> &_adj, MISConfig &config) {
    std::vector<std::vector<int>> adj(_adj.size());

    for(int i = 0; i < adj.size(); ++i) {
        for(int j = 0; j < _adj[i].size(); ++j) {
            adj[i].push_back(_adj[i][j]);
        }
    }

    int n = _adj.size();
    omp_set_num_threads(1);
    auto fastKer = full_reductions(_adj, _adj.size());
    fastKer.reduce_graph();
    graph_access FastKerKernel;
    std::vector<NodeID> fastKer_reverse_mapping(fastKer.number_of_nodes_remaining());
    fastKer.convert_adj_lists(FastKerKernel, fastKer_reverse_mapping);

    std::cout << "FastKer kernel size: " << fastKer.number_of_nodes_remaining() << std::endl;

    std::vector<std::vector<int>> FastKerKerneladj(FastKerKernel.number_of_nodes());

    // Build FastKerKerneladjacency vectors
    forall_nodes(FastKerKernel, node) {
        FastKerKerneladj[node].reserve(FastKerKernel.getNodeDegree(node));
        forall_out_edges(FastKerKernel, edge, node) {
            NodeID neighbor = FastKerKernel.getEdgeTarget(edge);
            FastKerKerneladj[node].push_back(neighbor);
        } endfor
    } endfor

    auto vcSolver = branch_and_reduce_algorithm(FastKerKerneladj, FastKerKerneladj.size());

    vcSolver.reduce();

    graph_access VCSolverKernel;
    std::vector<NodeID> vcsolver_reverse_mapping(vcSolver.number_of_nodes_remaining());
    vcSolver.convert_adj_lists(VCSolverKernel, vcsolver_reverse_mapping);

    std::cout << "VCSolver kernel size: " << vcSolver.number_of_nodes_remaining() << std::endl;

    ils localSearch = ils();

    localSearch.perform_ils(config, VCSolverKernel, config.ils_iterations);

    // std::vector<int> VCSolverKernelLocalSearchSolution(fastKer.number_of_nodes_remaining());
    // unsigned int solution_size = 0;
    // forall_nodes(VCSolverKernel, node) {
    //     if (VCSolverKernel.getPartitionIndex(node) == 1) {
    //         VCSolverKernelLocalSearchSolution[vcsolver_reverse_mapping[node]] = 0;
    //     }
    //     else {
    //         VCSolverKernelLocalSearchSolution[vcsolver_reverse_mapping[node]] = 1;
    //         solution_size++;
    //     }
    // } endfor

    // vcSolver.addStartingSolution(VCSolverKernelLocalSearchSolution, solution_size);
    //

    // timer t;
    // vcSolver.solve(t, config.time_limit);
    
    // std::cout << "Final solution size: " << (fastKer.number_of_nodes_remaining() - vcSolver.opt) + fastKer.get_current_is_size_with_folds() << std::endl;
    //

    // std::vector<bool> vcSolverSolution(fastKer.number_of_nodes_remaining());
    // vcSolver.get_solved_is(vcSolverSolution);

    // std::vector<bool> finalSolution(n, false);

    std::vector<std::vector<int>> VCSolverKerneladj(vcSolver.number_of_nodes_remaining());

    // Build VCSolverKerneladj vectors
    forall_nodes(VCSolverKernel, node) {
        VCSolverKerneladj[node].reserve(VCSolverKernel.getNodeDegree(node));
        forall_out_edges(VCSolverKernel, edge, node) {
            NodeID neighbor = VCSolverKernel.getEdgeTarget(edge);
            VCSolverKerneladj[node].push_back(neighbor);
        } endfor
    } endfor

    auto vcSolver2 = branch_and_reduce_algorithm(VCSolverKerneladj, VCSolverKerneladj.size());

    // Compute and apply solution
    std::vector<int> VCSolverKernelLocalSearchSolution(vcSolver.number_of_nodes_remaining());
    unsigned int solution_size = 0;
    forall_nodes(VCSolverKernel, node) {
        if (VCSolverKernel.getPartitionIndex(node) == 1) {
            VCSolverKernelLocalSearchSolution[node] = 0;
        }
        else {
            VCSolverKernelLocalSearchSolution[node] = 1;
            solution_size++;
        }
    } endfor
    vcSolver2.addStartingSolution(VCSolverKernelLocalSearchSolution, solution_size);

    std::cout << "Local search found solution of size " << (vcSolver.number_of_nodes_remaining() - solution_size) + fastKer.get_current_is_size_with_folds() + vcSolver.get_current_is_size_with_folds() << std::endl;


    timer t;
    vcSolver2.solve(t, config.time_limit);

    std::cout << "Number of branches pruned by starting solution: " << vcSolver2.numBranchesPrunedByStartingSolution << std::endl;

    std::cout << "Final solution size: " << (vcSolver.number_of_nodes_remaining() - vcSolver2.opt) + fastKer.get_current_is_size_with_folds() + vcSolver.get_current_is_size_with_folds() << std::endl;

    std::vector<bool> vcSolver2Solution(vcSolver.number_of_nodes_remaining());
    vcSolver2.get_solved_is(vcSolver2Solution);

    std::vector<bool> vcSolverSolution(fastKer.number_of_nodes_remaining(), false);
    for(int i = 0; i < vcSolver2Solution.size(); ++i) {
        vcSolverSolution[vcsolver_reverse_mapping[i]] = vcSolver2Solution[i];
    }

    vcSolver.extend_finer_is(vcSolverSolution);

    std::vector<bool> finalSolution(n, false);
    for(int i = 0; i < vcSolverSolution.size(); ++i) {
        finalSolution[fastKer_reverse_mapping[i]] = vcSolverSolution[i];
    }

    fastKer.extend_finer_is(finalSolution);

    int solutionSize = 0;
    // Check if valid MIS
    bool valid = true;
    for (int i = 0; i < adj.size(); ++i) {
      if (finalSolution[i] == true) {
          ++solutionSize;
        for (int j = 0; j < adj[i].size(); j++) {
            if (finalSolution[adj[i][j]] == true) valid = false;
        }
        if (!valid) {
          std::cout << "ERROR! Invalid solution" << std::endl;
          break;
        }
      }
    }


    if (!valid) {
        std::cout << "ERROR! Invalid solution" << std::endl;
    }
    else {
        std::cout << "Valid solution" << std::endl;
    }

    std::cout << "Actual solution size: " << solutionSize << std::endl;

    return finalSolution;
}
