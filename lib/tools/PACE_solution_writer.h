/** 
 * PACE_solution_writer.h
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>


void writePaceSolutionFromMIS(std::vector<bool> MIS, std::string filename) {
    std::ofstream f(filename.c_str());
    f << "s vc " <<  MIS.size() <<  " " << std::count(MIS.begin(), MIS.end(), false) << std::endl;

    for(int i = 0; i < MIS.size(); ++i) {
        if(MIS[i] == false) {
        f <<  i + 1 <<  std::endl;
        }
    }

    f.close();
}
