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
#include "CliqueSolverAdapter.h"
// #include "mis/darren/Experiments.h"
#include "ils.h"
#include "omp.h"

bool getMISBnR(graph_access &graph, std::vector<bool> &solution, double time_limit, MISConfig &config) {
        // Run iterated local search on kernel graph
        timer lt;
        ils localSearch = ils();
        localSearch.perform_ils(config, graph, config.ils_iterations);

        // Build adjacency lists for kernel graph
        std::vector<std::vector<int>> vcKernelAdj(graph.number_of_nodes());
        // Build vcKernelAdj vectors
        forall_nodes(graph, node) {
            vcKernelAdj[node].reserve(graph.getNodeDegree(node));
            forall_out_edges(graph, edge, node) {
                NodeID neighbor = graph.getEdgeTarget(edge);
                vcKernelAdj[node].push_back(neighbor);
            } endfor
        } endfor

        // Extract solution from iterated local search
        std::vector<int> ilsSolution(graph.number_of_nodes());
        unsigned int solution_size = 0;
        forall_nodes(graph, node) {
            if (graph.getPartitionIndex(node) == 1)
                ilsSolution[node] = 0;
            else {
                ilsSolution[node] = 1;
                solution_size++;
            }
        } endfor

              // std::cout << "Starting solution:\t\t" << n - (vcSolverAlgorithm.get_current_is_size_with_folds() + solution_size) << std::endl;
              // std::cout << "Time taken:\t\t\t" << lt.elapsed() << std::endl;
              // std::cout << std::endl;

              // Apply solution to exact algorithm
        auto BnRAlgorithm = branch_and_reduce_algorithm(vcKernelAdj, vcKernelAdj.size());
        BnRAlgorithm.addStartingSolution(ilsSolution, solution_size);

        // Run exact algorithm
        timer t;
        int retValue = BnRAlgorithm.solve(t, time_limit);

        if(retValue == -1) {
            return false;
        } else {
            // Extract solution from exact algorithm
            BnRAlgorithm.get_solved_is(solution);
            return true;
        }
}

void getMISCliqueInitial(std::vector<std::vector<int>> & input, std::vector<bool> &solution) {
    std::vector<std::vector<int>> vcKernelAdj = input;
    //// Build vcKernelAdj vectors
    //int numEdgesKernel = 0;
    //forall_nodes(graph, node) {
        //vcKernelAdj[node].reserve(graph.getNodeDegree(node));
        //forall_out_edges(graph, edge, node) {
            //NodeID neighbor = graph.getEdgeTarget(edge);
            //vcKernelAdj[node].push_back(neighbor);
            //++numEdgesKernel;
        //} endfor
    //} endfor

    auto solutionvertices = solveMISInstanceWithCliqueSolver(vcKernelAdj,2000000000, false);

    for(const auto & solutionVertex: solutionvertices) {
        if(solution[solutionVertex - 1]) {
            std::cout << "ERROR! solution vector was not false!" <<std::endl;
            exit(1);
        }
        solution[solutionVertex - 1] = true;
    }
}

bool getMISClique(graph_access &graph, std::vector<bool> &solution, bool check) {
    std::vector<std::vector<int>> vcKernelAdj(graph.number_of_nodes());
    // Build vcKernelAdj vectors
    int numEdgesKernel = 0;
    forall_nodes(graph, node) {
        vcKernelAdj[node].reserve(graph.getNodeDegree(node));
        forall_out_edges(graph, edge, node) {
            NodeID neighbor = graph.getEdgeTarget(edge);
            vcKernelAdj[node].push_back(neighbor);
            ++numEdgesKernel;
        } endfor
    } endfor

    auto solutionvertices = solveMISInstanceWithCliqueSolver(vcKernelAdj, 20000, check);
    if( solutionvertices.size() == 0) return false;

    for(const auto & solutionVertex: solutionvertices) {
        if(solution[solutionVertex - 1]) {
            std::cout << "ERROR! solution vector was not false!" <<std::endl;
            exit(1);
        }
        solution[solutionVertex - 1] = true;
    }
    return true;
}

bool canSolveClique(graph_access &graph) {
    if(graph.number_of_nodes() > tab_node_size) {
        return false;
    }
    int numEdgesComplement = graph.number_of_nodes() * (graph.number_of_nodes() - 1) - graph.number_of_edges();

    if(graph.number_of_nodes() < 3000) {
        return true;
    } else {
        return false;
    }
}

std::vector<bool> getExactMISCombined(std::vector<std::vector<int>> &_adj, MISConfig &config) {
    unsigned int n = _adj.size();
    std::vector<bool> finalSolution(n, false);
    // omp_set_num_threads(1);

    // Copy adj vector
    std::vector<std::vector<int>> adj(_adj.size());
    for(unsigned int i = 0; i < _adj.size(); ++i) {
        for(unsigned int j = 0; j < _adj[i].size(); ++j) {
            adj[i].push_back(_adj[i][j]);
        }
    }

    // Run VCSolver reductions on preliminary kernel
    timer rt;
    rt.restart();
    auto vcSolverAlgorithm = branch_and_reduce_algorithm(_adj, _adj.size());
    vcSolverAlgorithm.reduce();

    // Extract kernel graph
    graph_access vcKernel;
    std::vector<NodeID> vcKernelReverseMapping(vcSolverAlgorithm.number_of_nodes_remaining());
    vcSolverAlgorithm.convert_adj_lists(vcKernel, vcKernelReverseMapping);

    // std::cout << "Reduced size:\t\t\t" << vcSolverAlgorithm.number_of_nodes_remaining() << std::endl;
    // std::cout << "Time taken:\t\t\t" << rt.elapsed() << std::endl;
    // std::cout << std::endl;

    std::vector<bool> exactSolution(vcSolverAlgorithm.number_of_nodes_remaining(), false);
    if(vcKernel.number_of_nodes() > 0){
        if(canSolveClique(vcKernel)) {
//bool foundSolution = getMISBnR(vcKernel, exactSolution, config.time_limit, config);
                //if(!foundSolution) {
                        //if( adj.size() < 3000 ) {
                                //bool MISfoundsolution = getMISClique(vcKernel, exactSolution, true);
                                //if(!MISfoundsolution) {
                                        //// call getMISClique on origianl instance with large ...
                                        ////
                                        //std::cout <<  "c finished is false, so trying on input"  << std::endl;
                                        //getMISCliqueInitial(adj, finalSolution);
                                        //return finalSolution;

                                //}
                        //}
                        //else {
                                //getMISClique(vcKernel, exactSolution, false);
                                ////getMISCliqueInitial(adj, finalSolution );
                        //}
                //}
                bool MISfoundsolution = getMISClique(vcKernel, exactSolution, true);
                if(!MISfoundsolution) {
                        if( adj.size() < 3000 ) {
                                for( unsigned i = 0; i < exactSolution.size(); i++) {
                                        exactSolution[i] = false;
                                }
                                bool foundSolution = getMISBnR(vcKernel, exactSolution, config.time_limit, config);
                                if(!foundSolution) {
                                        // call getMISClique on origianl instance with large ...
                                        //
                                        std::cout <<  "c finished is false, so trying on input"  << std::endl;
                                        getMISCliqueInitial(adj, finalSolution);
                                        return finalSolution;

                                }
                        }
                        else {
                                //getMISClique(vcKernel, exactSolution, false);
                                bool foundSolution = getMISBnR(vcKernel, exactSolution, 9999999.0, config);
                                //getMISCliqueInitial(adj, finalSolution );
                        }
                }
        } else {
            bool foundSolution = getMISBnR(vcKernel, exactSolution, 9999999.0, config);
        }
    }

    // Propagate solution to preliminary kernel
    for(unsigned int i = 0; i < exactSolution.size(); ++i)
        finalSolution[vcKernelReverseMapping[i]] = exactSolution[i];
    vcSolverAlgorithm.extend_finer_is(finalSolution);

    return finalSolution;
}

