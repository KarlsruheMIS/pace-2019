/** 
 * PACE_graph_reader.h
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

vector<vector<int>> readPaceGraph(string fileName) {
    std::ifstream infile(fileName);

    int numVertices;
    int numEdges;
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        char firstSymbol;
        if (!(iss >> firstSymbol)) { break; } // error

        if(firstSymbol == 'p') {
            string td;
            iss >> td >> numVertices >> numEdges;
            break;
        }
    }

    vector<vector<int>> graph(numVertices);

    while (getline(infile, line)) {
        if (line.empty()) {
            continue;
        }

        if(line[0] == 'c') {
            continue;
        }

        istringstream iss(line);

        int u, v;
        iss >> u >> v;

        --u;
        --v;

        if(u < 0 || u >= numVertices) {
            std::cout << "Invalid node ID: " << u <<std::endl;
            exit(1);
        }
        if(v < 0 || v >= numVertices) {
            std::cout << "Invalid node ID: " << v <<std::endl;
            exit(1);
        }

        if(u == v) {
            continue;
        }

        if(find(graph[u].begin(), graph[u].end(), v) == graph[u]. end()) {
            graph[u].push_back(v);
        }

        if(find(graph[v].begin(), graph[v].end(), u) == graph[v]. end()) {
            graph[v].push_back(u);
        }
    }

    for(int i = 0; i < numVertices; ++i) {
        sort(graph[i].begin(), graph[i].end());
    }

    return graph;
}
