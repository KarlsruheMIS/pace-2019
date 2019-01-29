#ifndef MAXIMUM_MATCHING_H
#define MAXIMUM_MATCHING_H

#include "SparseArraySet.h"
#include "SimpleSet.h"
#include <vector>
#include <atomic>

typedef struct /* the bipartite graph data structure */
{
	long n; // numver of vertices in both sides
	long nrows; // number of vertices in the left side
	long m; // number of edges
	long* vtx_pointer; // an array of size n+1 storing the pointer in endV array
    long* endV; //an array of size m that stores the second vertex of an edge.
	double* weight; // not used in unweighted graph
} graph;

void free_graph (graph* bGraph);

class MaximumMatching{
public:

	MaximumMatching(std::vector<std::vector<int>> const &adjacencyArray);
	~MaximumMatching();
	void LoadGraph(std::vector<SparseArraySet> &neighbors, SimpleSet &inGraph, std::vector<std::atomic_int> &vertexDegree);
	long* MS_BFS_Graft();
	long KarpSipserInit(SimpleSet &inGraph);
	void MarkReachableVertices();

	std::vector<int> reachableVertices;

protected:
	void findMate(long u, graph* G, long* flag,long* mate, long* degree);
	int VertexDegree(const int vertex, std::vector<SparseArraySet> &neighbors, SimpleSet &inGraph, std::vector<std::atomic_int> &vertexDegree);
	long KarpSipserInit1();
	long KarpSipserInit2(SimpleSet &inGraph);

	// Just for testing
	bool IsValidVertexCover();
	bool CheckVertexCoverAndMatchingSize();

	graph* G;

	long* QF;
    long* QFnext;
	long* __restrict__ flag;
	long* __restrict__ parent;
	long* __restrict__ leaf;
    long* __restrict__ root;
	long* __restrict__ mate;
    long* unmatchedU;
    long* nextUnmatchedU;

    long * degree;
	long* degree1Vtx;

	std::vector<std::vector<long>> stacks;

	bool firstKarpSipser;

};

#endif //MAXIMUM_MATCHING_H
