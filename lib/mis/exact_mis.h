/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "mis_config.h"
#include "graph_io.h"

bool evaluateCriterion(std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISCombined(std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISDarren(std::vector<std::vector<int>> &_adj, MISConfig &config);
bool getMISBnR(graph_access &graph, std::vector<bool> &solution, double time_limit, MISConfig &config);
bool getMISClique(graph_access &graph, std::vector<bool> &solution, bool docheck = true);
bool canSolveClique(graph_access &graph);
