/** 
 * kernelizeAndMakeCliqueInstance.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <argtable2.h>

#include "timer.h"
#include "ils/ils.h"
#include "mis_log.h"
#include "graph_io.h"
#include "mis_config.h"
#include "parse_parameters.h"
#include "data_structure/graph_access.h"
#include "data_structure/mis_permutation.h"
#include "exact_mis.h"
#include "PACE_graph_reader.h"
#include "PACE_solution_writer.h"
#include "CliqueInstanceWriter.h"
#include "mis/kernel/branch_and_reduce_algorithm.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream> 


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(int argn, char **argv) {
    mis_log::instance()->restart_total_timer();
    // mis_log::instance()->print_title();
    
    MISConfig mis_config;
    std::string graph_filepath;

    // Parse the command line parameters;
    int ret_code = parse_parameters(argn, argv, mis_config, graph_filepath);
    if (ret_code) return 0;
    mis_config.graph_filename = graph_filepath.substr(graph_filepath.find_last_of( '/' ) +1);
    mis_log::instance()->set_config(mis_config);

    // Read input file
    std::vector<std::vector<int>> graph;

    if(graph_filepath.empty()) {
        graph = readPaceGraphFromCin();
    } else {
        graph = readPaceGraphFromFile(graph_filepath);
    }
    mis_log::instance()->number_of_nodes = graph.size();
    unsigned int num_edges = 0;
    for (auto &v : graph) num_edges += v.size();
    mis_log::instance()->number_of_edges = num_edges;

    // Copy adj vector
    std::vector<std::vector<int>> adj(graph.size());
    for(unsigned int i = 0; i < adj.size(); ++i) {
        for(unsigned int j = 0; j < graph[i].size(); ++j) {
            adj[i].push_back(graph[i][j]);
        }
    }
    // Print setup information
    // mis_log::instance()->print_graph();
    // mis_log::instance()->print_config();

    timer rt;
    rt.restart();
    auto vcSolverAlgorithm = branch_and_reduce_algorithm(adj, adj.size());
    vcSolverAlgorithm.reduce();

    // Extract kernel graph
    graph_access vcKernel;
    std::vector<NodeID> vcKernelReverseMapping(vcSolverAlgorithm.number_of_nodes_remaining());
    vcSolverAlgorithm.convert_adj_lists(vcKernel, vcKernelReverseMapping);

    // std::cout << "Reduced size:\t\t\t" << vcSolverAlgorithm.number_of_nodes_remaining() << std::endl;
    // std::cout << "Time taken:\t\t\t" << rt.elapsed() << std::endl;
    // std::cout << std::endl;

    // Build adjacency lists for kernel graph
    std::vector<std::vector<int>> vcKernelAdj(vcSolverAlgorithm.number_of_nodes_remaining());
    // Build vcKernelAdj vectors
    int numEdgesKernel = 0;
    forall_nodes(vcKernel, node) {
        vcKernelAdj[node].reserve(vcKernel.getNodeDegree(node));
        forall_out_edges(vcKernel, edge, node) {
            NodeID neighbor = vcKernel.getEdgeTarget(edge);
            vcKernelAdj[node].push_back(neighbor);
            ++numEdgesKernel;
        } endfor
    } endfor
          // std::cout << "Kernel has " << numEdgesKernel / 2 << " edges" <<std::endl;

    char cliqueFileNamecstr[L_tmpnam];
    std::tmpnam(cliqueFileNamecstr);

    std::string cliqueFileName(cliqueFileNamecstr);
    writeCliqueInstanceToFile(vcKernelAdj, cliqueFileName);

    // std::cout << "Wrote file" <<std::endl;
    std::string executablePath = std::string(argv[0]);
    size_t dirIndex = executablePath.find_last_of("/\\");
    // std::cout << " folder: " << executablePath.substr(0, dirIndex) << std::endl;
    std::string cmdString = executablePath.substr(0, dirIndex) + "/extern/cliqueSolver/a.out " + cliqueFileName;
    std::string cliqueSolverResult = exec(cmdString.c_str());


    cmdString = "rm " + cliqueFileName;
    exec(cmdString.c_str());


    std::vector<bool> exactSolution(vcSolverAlgorithm.number_of_nodes_remaining());

    // std::cout << cliqueSolverResult <<std::endl;
    std::istringstream iss(cliqueSolverResult);

    bool foundSolution = false;
    for (std::string line; std::getline(iss, line); ) {
        if(line[0] == 'M') {
            foundSolution = true;
            // std::cout << line <<std::endl;
            std::istringstream s(line);

            std::string word;
            int solutionVertex;
            while (s >> word) {
                if (std::stringstream(word) >> solutionVertex) {
                    exactSolution[solutionVertex - 1] = true;
                }
            }
        }
    }

    if(!foundSolution) {
        fprintf( stderr, "Clique solver did not find a solution" );
        exit(1);
    }


    std::vector<bool> finalSolution(graph.size(), false);
    for(unsigned int i = 0; i < exactSolution.size(); ++i)
        finalSolution[vcKernelReverseMapping[i]] = exactSolution[i];
    vcSolverAlgorithm.extend_finer_is(finalSolution);

    int numVerticesChecked = 0;
    int numEdgesInCheckedGraph = 0;
    int MISSize = 0;
    for(unsigned i = 0; i < graph.size(); ++i) {
        numVerticesChecked++;
        numEdgesInCheckedGraph += graph[i].size();
        if(finalSolution[i]) {
            ++MISSize;
            for(int j = 0; j < graph[i].size(); ++j) {
                if(finalSolution[graph[i][j]]) {
                    std::cout << "NOT AN INDEPENDENT SET!" << std::endl;
                }
            }
        }
    }

    // std::cout << "Done checking graph (" << numVerticesChecked << ", " << numEdgesInCheckedGraph << ")" << std::endl;
    // std::cout << "MIS size: " << MISSize << std::endl;

    writePaceSolutionFromMISToCout(finalSolution);
    return 0;
}
