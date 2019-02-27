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
#include "ils/local_search.h"
#include "mis_log.h"
#include "graph_io.h"
#include "reduction_evolution.h"
#include "mis_config.h"
#include "greedy_mis.h"
#include "parse_parameters.h"
#include "data_structure/graph_access.h"
#include "data_structure/mis_permutation.h"
#include "full_reductions.h"
#include "exact_mis.h"
#include "PACE_graph_reader.h"
#include "PACE_solution_writer.h"

int main(int argn, char **argv) {
    mis_log::instance()->restart_total_timer();
    mis_log::instance()->print_title();
    
    MISConfig mis_config;
    std::string graph_filepath;

    // Parse the command line parameters;
    int ret_code = parse_parameters(argn, argv, mis_config, graph_filepath);
    if (ret_code) return 0;
    mis_config.graph_filename = graph_filepath.substr(graph_filepath.find_last_of( '/' ) +1);
    mis_log::instance()->set_config(mis_config);


    vector<vector<int>> graph = readPaceGraph(graph_filepath);

    mis_log::instance()->number_of_nodes = graph.size();

    // Print setup information
    mis_log::instance()->print_graph();
    mis_log::instance()->print_config();

    std::vector<bool> MIS;
    if (evaluateCriterion(graph, mis_config))
      MIS = getExactMISCombined(graph, mis_config);
    else
      MIS = getExactMISDarren(graph, mis_config);

    writePaceSolutionFromMIS(MIS, mis_config.graph_filename + ".vc");
    return 0;
}
