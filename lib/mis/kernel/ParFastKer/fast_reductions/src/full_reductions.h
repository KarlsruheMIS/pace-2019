#ifndef FULL_REDUCTIONS_H
#define FULL_REDUCTIONS_H

#include <vector>
#include <memory>
#include "mis_config.h"
#include "parallel_reductions.h"
#include "mis/kernel/ParFastKer/LinearTime/MIS_sigmod_pub/Graph.h"


class full_reductions
{
public:
	full_reductions(const std::vector<std::vector<int>> &_adj, int _N);
	void reduce_graph();
	size_t get_current_is_size_with_folds();
	size_t number_of_nodes_remaining();
    void force_into_independent_set(std::vector<NodeID> &nodes_to_force);
    void convert_adj_lists(graph_access &G, std::vector<NodeID> &reverse_mapping);
    void extend_finer_is(std::vector<bool> &independent_set);

	static std::vector<int> partitions;
private:
	const std::vector<std::vector<int>> &adj;
	std::unique_ptr<parallel_reductions> parallel_reducer;
    std::vector<unsigned int> forced_vertices;
    int linearTimeOffset;
    LinearTime::Graph LineartimeKernelizer;
    std::vector<int> after_force_mapping;
    std::vector<int> after_force_reverse_mapping;
};


#endif // FULL_REDUCTIONS_H
