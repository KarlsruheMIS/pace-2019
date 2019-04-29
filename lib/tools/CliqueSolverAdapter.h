/**
 * CliqueSolverAdapter.h
 *
 * Copyright (C) 2019 Demian Hespe <hespe@kit.edu>
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "MoMC2016.c"


static int build_simple_graph_instance_from_pointer_to_edge_list(int* inputEdgesFrom, int* inputEdgesTo, int numVertices, int numEdges) {
	char ch, words[WORD_LENGTH];
	int i, j, e, nb1, nb2, node, left_node, right_node, nb_edge = 0;
    NB_NODE = numVertices;
    NB_EDGE = numEdges;

	for (i = 0; i < NB_EDGE; i++) {
        left_node = inputEdgesFrom[i];
        right_node = inputEdgesTo[i];
		if (left_node == right_node) {
            printf("ERROR: Self loop!");
            exit(1);
		} else {
			if (left_node > right_node) {
				e = right_node;
				right_node = left_node;
				left_node = e;
			}
			if (matrice[left_node][right_node] == FALSE) {
				matrice[left_node][right_node] = TRUE;
				matrice[right_node][left_node] = TRUE;
				static_degree[left_node]++;
				static_degree[right_node]++;
				nb_edge++;
			}
		}
	}
	NB_EDGE = nb_edge;
	for (i = 1; i <= NB_NODE; i++) {
		node_state[i] = ACTIVE;
		active_degree[i] = static_degree[i];

		node_neibors[i] = (int *) malloc((static_degree[i] + 1) * sizeof(int));

		none_degree[i] = NB_NODE - static_degree[i] - 1;
		// none_neibors[i] = (int *) malloc(
		// 		(NB_NODE - static_degree[i]) * sizeof(int));
		none_neibors[i] = (int *) malloc(
                                         (NB_NODE) * sizeof(int));
		nb1 = 0;
		nb2 = 0;
		for (j = 1; j <= NB_NODE; j++) {
			if (matrice[i][j] == TRUE)
				node_neibors[i][nb1++] = j;
			else if (i != j) {
				none_neibors[i][nb2++] = j;
			}
		}
		node_neibors[i][nb1] = NONE;
		none_neibors[i][nb2] = NONE;
	}

	ptr(INIT_Stack) = 0;
	INIT_Stack = (int *) malloc((NB_NODE + 1) * sizeof(int));

	static_matrix = (char *) malloc(
			(NB_NODE + 1) * (NB_NODE + 1) * sizeof(char));
	for (node = 1; node <= NB_NODE; node++) {
		qsort(none_neibors[node], none_degree[node], sizeof(int),
				none_degree_inc);
		qsort(node_neibors[node], static_degree[node], sizeof(int),
				static_degree_dec);
	}
	printf("c Instance Information: #node=%d, #edge=%d density= %5.4f \n",
			NB_NODE, NB_EDGE,
			((float) NB_EDGE * 2) / (NB_NODE * (NB_NODE - 1)));

	return TRUE;
}

std::vector<int> solveMISInstanceWithCliqueSolver(std::vector<std::vector<int>> &MISInstance, long limit, bool docheck=false) {
        std::cout <<  "c"  << std::endl;
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

    std::vector<int> edgesFrom, edgesTo;
    int numEdgesReal = 0;
    for(int i = 0; i < numVertices; i++) {
        for(int j = 0; j < numVertices; j++) {
            if(adjMatrix[i][j]) {
                edgesFrom.push_back(i + 1);
                edgesTo.push_back(j + 1);
                ++numEdgesReal;
            }
        }
    }

    if(numEdges != numEdgesReal) {
        std::cout << "Wrong number of edges!!" << std::endl;
        exit(1);
    }
std::cout <<  "d"  << std::endl;
	struct rusage starttime, endtime;
	long sec, usec, sec_p, usec_p;
	int i, ordering = -1, _all = FALSE;
	INIT_CLIQUE = 0;
	LIST_ALL = FALSE;
	getrusage(RUSAGE_SELF, &starttime);
    build_simple_graph_instance_from_pointer_to_edge_list(edgesFrom.data(), edgesTo.data(), numVertices, numEdges);
std::cout <<  "e"  << std::endl;
    search_initial_maximum_clique();
    init_for_maxclique(ordering, _all);
    re_code();

    std::cout <<  "a"  << std::endl;
    bool finished = search_maxclique(0, FALSE, limit, docheck);
    std::cout <<  "b"  << std::endl;
    std::vector<int> solution;
    if(!finished) return solution;
        	for (i = 0; i < MAX_CLQ_SIZE; i++) {
                solution.push_back(NEW_OLD[MaxCLQ_Stack[i]]);
          }

    return solution;
}
