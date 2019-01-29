/**
 * exact_mis.cpp
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *
 *****************************************************************************/

#pragma once

#include <vector>
#include "mis_config.h"

std::vector<bool> getExactMIS(const std::vector<std::vector<int>> &_adj, MISConfig &config);
