/** 
 * exactVC.cpp
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

        // Print setup information
        // mis_log::instance()->print_graph();
        // mis_log::instance()->print_config();
        //
        std::streambuf* backup = std::cout.rdbuf();
        std::ofstream ofs;
        ofs.open("/dev/null");
        std::cout.rdbuf(ofs.rdbuf()); 
        std::cout <<  "test"  << std::endl;

        timer t;
        t.restart();
        std::vector<bool> MIS;
        MIS = getExactMISCombined(graph, mis_config);

        // Output size and solution
        // std::cout << "\t\tResult"        << std::endl;
        // std::cout << "=========================================="                           << std::endl;
        // std::cout << "VC size:\t\t\t" << std::count(MIS.begin(), MIS.end(), false) << std::endl;
        ofs.close();
        std::cout.rdbuf(backup);

        if(graph_filepath.empty()) {
                writePaceSolutionFromMISToCout(MIS);
        } else {
                // Output size and solution
                std::cout << "\t\tResult"        << std::endl;
                std::cout << "=========================================="                           << std::endl;
                std::cout << "VC size:\t\t\t" << std::count(MIS.begin(), MIS.end(), false) << std::endl;
                writePaceSolutionFromMISToFile(MIS, mis_config.graph_filename + ".vc");
                // writePaceSolutionFromMISToCout(MIS);
        }
        return 0;
}
