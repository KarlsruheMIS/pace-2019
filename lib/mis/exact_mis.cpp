/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/
#include <limits.h>

#include "exact_mis.h"

// #include "full_reductions.h"
#include "mis_log.h"
#include "graph_io.h"
#include "timer.h"
#include "mis_config.h"
#include "data_structure/graph_access.h"
#include "mis/kernel/branch_and_reduce_algorithm.h"
// #include "mis/darren/Experiments.h"
#include "ils.h"
#include "omp.h"

std::vector<bool> getExactMISCombined(std::vector<std::vector<int>> &_adj, MISConfig &config) {
    unsigned int n = _adj.size();
    omp_set_num_threads(1);

    // Copy adj vector
    std::vector<std::vector<int>> adj(_adj.size());
    for(unsigned int i = 0; i < adj.size(); ++i) {
        for(unsigned int j = 0; j < _adj[i].size(); ++j) {
            adj[i].push_back(_adj[i][j]);
        }
    }

    // Run VCSolver reductions on preliminary kernel
    auto vcSolverAlgorithm = branch_and_reduce_algorithm(_adj, _adj.size());
    vcSolverAlgorithm.reduce();

    // Extract kernel graph
    graph_access vcKernel;
    std::vector<NodeID> vcKernelReverseMapping(vcSolverAlgorithm.number_of_nodes_remaining());
    vcSolverAlgorithm.convert_adj_lists(vcKernel, vcKernelReverseMapping);

    // Run iterated local search on kernel graph
    ils localSearch = ils();
    localSearch.perform_ils(config, vcKernel, config.ils_iterations);

    // Build adjacency lists for kernel graph
    std::vector<std::vector<int>> vcKernelAdj(vcSolverAlgorithm.number_of_nodes_remaining());
    // Build vcKernelAdj vectors
    forall_nodes(vcKernel, node) {
        vcKernelAdj[node].reserve(vcKernel.getNodeDegree(node));
        forall_out_edges(vcKernel, edge, node) {
            NodeID neighbor = vcKernel.getEdgeTarget(edge);
            vcKernelAdj[node].push_back(neighbor);
        } endfor
    } endfor

    // Extract solution from iterated local search 
    std::vector<int> ilsSolution(vcSolverAlgorithm.number_of_nodes_remaining());
    unsigned int solution_size = 0;
    forall_nodes(vcKernel, node) {
        if (vcKernel.getPartitionIndex(node) == 1) 
            ilsSolution[node] = 0;
        else {
            ilsSolution[node] = 1;
            solution_size++;
        }
    } endfor

    // Apply solution to exact algorithm
    auto BnRAlgorithm = branch_and_reduce_algorithm(vcKernelAdj, vcKernelAdj.size());
    BnRAlgorithm.addStartingSolution(ilsSolution, solution_size);

    // Run exact algorithm
    timer t;
    BnRAlgorithm.solve(t, config.time_limit);

    // Extract solution from exact algorithm
    std::vector<bool> exactSolution(vcSolverAlgorithm.number_of_nodes_remaining());
    BnRAlgorithm.get_solved_is(exactSolution);

    // Propagate solution to preliminary kernel
    std::vector<bool> finalSolution(n, false);
    for(unsigned int i = 0; i < exactSolution.size(); ++i)
        finalSolution[vcKernelReverseMapping[i]] = exactSolution[i];
    vcSolverAlgorithm.extend_finer_is(finalSolution);

    return finalSolution;
}

