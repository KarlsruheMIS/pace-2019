/** 
 * CliqueInstanceWriter.h
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

void writeCliqueInstance(std::vector<std::vector<int>> &MISInstance, std::ostream &f) {
    int numVertices = MISInstance.size();
    int numEdges = numVertices * (numVertices - 1);
    std::vector<std::vector<bool>> adjMatrix(numVertices);
    for(int i = 0; i < numVertices; i++) {
        adjMatrix[i] = std::vector<bool>(numVertices, true);
        adjMatrix[i][i] = false;
        for(const auto &neighbor: MISInstance[i]) {
            adjMatrix[i][neighbor] = false;
            --numEdges;
        }
    }
    f << "p edge " <<  numVertices <<  " " << numEdges << std::endl;

    for(int i = 0; i < numVertices; i++) {
        for(int j = 0; j < numVertices; j++) {
            if(adjMatrix[i][j]) {
                f << "e " << i + 1 <<  " " << j + 1 << std::endl;
            }
        }
    }


    // f.close();
}


void writeCliqueInstanceToFile(std::vector<std::vector<int>> &MISInstance, std::string filename) {
    std::ofstream f(filename.c_str());
    writeCliqueInstance(MISInstance, f) ;
    f.close();
}
