/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "mis_config.h"

bool evaluateCriterion(std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISCombined(std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISDarren(std::vector<std::vector<int>> &_adj, MISConfig &config);
