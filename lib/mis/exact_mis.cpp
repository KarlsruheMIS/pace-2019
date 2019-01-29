/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/
#include <limits.h>

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
    omp_set_num_threads(1);
    auto fastKer = full_reductions(_adj, _adj.size());
    fastKer.reduce_graph();
    graph_access FastKerKernel;
    std::vector<NodeID> fastKer_reverse_mapping(fastKer.number_of_nodes_remaining());
    std::cout << _adj.size() << std::endl;
    fastKer.convert_adj_lists(FastKerKernel, fastKer_reverse_mapping);


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

    ils localSearch = ils();

    localSearch.perform_ils(config, VCSolverKernel, config.ils_iterations);

    std::vector<int> VCSolverKernelLocalSearchSolution(FastKerKerneladj.size());


    unsigned int solution_size = 0;
    forall_nodes(VCSolverKernel, node) {
        if (VCSolverKernel.getPartitionIndex(node) == 1) {
            VCSolverKernelLocalSearchSolution[vcsolver_reverse_mapping[node]] = 0;
            solution_size++;
        }
        else VCSolverKernelLocalSearchSolution[vcsolver_reverse_mapping[node]] = 1;
    } endfor

          vcSolver.addStartingSolution(VCSolverKernelLocalSearchSolution, solution_size);


    timer t;
	vcSolver.solve(t, config.time_limit);

    std::vector<bool> solution(FastKerKernel.number_of_nodes());
    vcSolver.get_solved_is(solution);


    fastKer.extend_finer_is(solution);

    return solution;
}
