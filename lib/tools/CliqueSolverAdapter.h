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
            //printf("ERROR: Self loop!");
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
	//printf("c Instance Information: #node=%d, #edge=%d density= %5.4f \n", NB_NODE, NB_EDGE, ((float) NB_EDGE * 2) / (NB_NODE * (NB_NODE - 1)));

	return TRUE;
}

void nullout() {
        FORMAT = 1;
        for( int i = 0; i < 2*tab_node_size; i++) {
//static int FORMAT = 1, DENSITY, NB_NODE, ADDED_NODE, NB_EDGE, MAX_CLQ_SIZE,
		//MAX_ISET_SIZE, NB_BACK_CLIQUE;
//static char node_state[2 * tab_node_size];
        node_state[i] = 0; 
////static int node_value[2 * tab_node_size];
        //node_value[i] = 0;
        node_reason[i] = 0;
//static int FIXED_NODE_STACK[2 * tab_node_size];
        FIXED_NODE_STACK[i] = 0;
        Tmp_Stack[i] = 0;
        }
//static int node_reason[2 * tab_node_size];
////static char static_matrix[tab_node_size][tab_node_size];
//static char *static_matrix;
for( unsigned i = 0; i < tab_node_size; i++) {
        for( unsigned j = 0; j < tab_node_size; j++) {
                matrice[i][j] = false;
                iSET[i][j] = 0;
                /* code */
        }
}
//static char matrice[tab_node_size][tab_node_size];
//static int iSET[tab_node_size][tab_node_size];
iSET_COUNT = 0;
for( unsigned i = 0; i < tab_node_size; i++) {
iSET_Size[i] =0 ;
//iSET_OLD_Size[i]=0;
//iSET_ADD_Size[i] = 0;
iSET_State[i] = 0;
iSET_Used[i] = 0 ;
iSET_Tested[i] = 0;
iSET_Index[i]= 0;
//
}
for( unsigned i = 0; i < 10*tab_node_size; i++) {
        REDUCED_iSET_STACK[i] =0;
        /* code */
}
//static int REDUCED_iSET_STACK[tab_node_size * 10];
REDUCED_iSET_STACK_fill_pointer = 0;
PASSIVE_iSET_STACK_fill_pointer = 0;
FIXED_NODE_STACK_fill_pointer = 0;
UNIT_STACK_fill_pointer = 0;
NEW_UNIT_STACK_fill_pointer = 0;
Clique_Stack_fill_pointer = 0;
for( unsigned i = 0; i < tab_node_size; i++) {
PASSIVE_iSET_STACK[i] = 0;
UNIT_STACK[i] = 0;
NEW_UNIT_STACK[i] = 0;
//*node_neibors[i] = 0;
//*none_neibors[i] = 0;
static_degree[i] = 0;
active_degree[i] = 0;
none_degree[i] = 0;
//CACHED_REASON_STACK[i] = 0;
Clique_Stack[i] = 0;
MaxCLQ_Stack[i] = 0;
NEW_OLD[i] = 0;
OLD_NEW[i] = 0;
}
//static int *INIT_Stack;
INIT_Stack_fill_pointer = 0;
//static int Candidate_Stack[tab_node_size * max_expand_depth];
//static int Vertex_UB[tab_node_size * max_expand_depth];
//static int Cursor_Stack[max_expand_depth];
Candidate_Stack_fill_pointer = 0;
Cursor_Stack_fill_pointer = 0;
//static int Rollback_Point;
//static int Branching_Point;
Tmp_Stack_fill_pointer = 0;
//static int NEW_OLD[tab_node_size];
//static int OLD_NEW[tab_node_size];
NB_CANDIDATE = 0, REBUILD_MATRIX = FALSE;
FIRST_INDEX = 0;

//static int Extra_Node_Stack[1000];
//Extra_Node_Stack_fill_pointer = 0;
////static int Extra_Node_Index[tab_node_size];
Last_Idx = 0;
cut_ver = 0, total_cut_ver = 0;
cut_inc = 0, total_cut_inc = 0;
cut_iset = 0, total_cut_iset = 0;
cut_satz = 0, total_cut_satz = 0;
//static long long Branches_Nodes[6];
STATIC_ORDERING = TRUE, INIT_CLIQUE = 0;
Dynamic_Radio = 0.6;
Mean_Dynamic_Radio = 0.0;
Dynamic_Count = 0;
LIST_ALL = FALSE;
//INIT_SORTING = FALSE;
////static int CACHED_REASON_STACK_fill_pointer = 0;
//static int Branches[1200];
MAX_COUNT = 0, SHOW_COUNT = 0;
//static int *APPEND_STACK;
APPEND_STACK_SIZE = 0;
APPEND_STACK_USED = 0;

}

std::vector<int> solveMISInstanceWithCliqueSolver(std::vector<std::vector<int>> &MISInstance, long limit, bool docheck=false) {


        nullout();




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
	struct rusage starttime, endtime;
	long sec, usec, sec_p, usec_p;
	int i, ordering = -1, _all = FALSE;
	INIT_CLIQUE = 0;
	LIST_ALL = FALSE;
	getrusage(RUSAGE_SELF, &starttime);
    build_simple_graph_instance_from_pointer_to_edge_list(edgesFrom.data(), edgesTo.data(), numVertices, numEdges);
    search_initial_maximum_clique();
    init_for_maxclique(ordering, _all);
    re_code();

    bool finished = search_maxclique(0, FALSE, limit, docheck);
    std::vector<int> solution;
    if(!finished) return solution;
        	for (i = 0; i < MAX_CLQ_SIZE; i++) {
                solution.push_back(NEW_OLD[MaxCLQ_Stack[i]]);
          }

    return solution;
}
