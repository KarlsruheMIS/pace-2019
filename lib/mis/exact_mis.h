/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "mis_config.h"

bool evaluateCriterion(const std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISCombined(const std::vector<std::vector<int>> &_adj, MISConfig &config);
std::vector<bool> getExactMISDarren(const std::vector<std::vector<int>> &_adj, MISConfig &config);
