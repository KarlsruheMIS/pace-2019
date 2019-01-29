/******************************************************************************
 * parallel_label_compress.h 
 *
 * Source of the Parallel Partitioning Program
 ******************************************************************************
 * Copyright (C) 2014 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef PARALLEL_LABEL_COMPRESS_9ME4H8DK
#define PARALLEL_LABEL_COMPRESS_9ME4H8DK

#include "data_structure/parallel_graph_access.h"
#include "partition_config.h"
#include "tools/random_functions.h"
#include "hmap_wrapper.h"
#include "node_ordering.h"


template <typename T> 
class parallel_label_compress {
public:
        parallel_label_compress() {};
        virtual ~parallel_label_compress() {};

        //void perform_parallel_label_compression( PartitionConfig & config, 
                                                 //parallel_graph_access & G, bool balance, bool for_coarsening = true);


void perform_parallel_label_compression( PartitionConfig & config, 
                parallel_graph_access & G, bool balance, bool for_coarsening = true) {

        if( config.label_iterations == 0) return;
        NodeWeight cluster_upperbound = config.upper_bound_cluster;

        std::vector< NodeID > permutation( G.number_of_local_nodes() );
        if( for_coarsening ) {
                node_ordering no; 
                no.order_nodes( config, G, permutation); 
        } else {
                random_functions::permutate_vector_fast( permutation, true);
        }

        //std::unordered_map<NodeID, NodeWeight> hash_map;
        hmap_wrapper< T > hash_map(config);
        hash_map.init( G.get_max_degree() );
        for( unsigned i = 0; i < config.label_iterations; i++) {
                NodeID prev_node = 0;
                forall_local_nodes(G, rnode) {
                        NodeID node = permutation[rnode]; // use the current random node

                        //move the node to the cluster that is most common in the neighborhood

                        //forall_out_edges(G, e, node) {
                        //NodeID target = G.getEdgeTarget(e);
                        //hash_map[G.getNodeLabel(target)] += G.getEdgeWeight(e);
                        //} endfor

                        //second sweep for finding max and resetting array
                        PartitionID max_block   = G.getNodeLabel(node);
                        PartitionID old_block   = G.getNodeLabel(node);
                        PartitionID max_value   = 0;
                        NodeWeight  node_weight = G.getNodeWeight(node);
                        bool own_block_balanced = G.getBlockSize(old_block) <= cluster_upperbound || !balance;

                        if( G.getNodeDegree(node) == 0) {
                                // find a block to assign it to
                                if(config.vcycle) {
                                        NodeWeight prev_block_size = G.getBlockSize( G.getNodeLabel( prev_node ) );
                                        bool same_block = G.getSecondPartitionIndex(prev_node)==G.getSecondPartitionIndex(node);
                                        if( prev_block_size  + node_weight <= cluster_upperbound && same_block ) {
                                                max_block = G.getNodeLabel( prev_node );
                                        }
                                } else {
                                        NodeWeight prev_block_size = G.getBlockSize( G.getNodeLabel( prev_node ) );
                                        if( prev_block_size  + node_weight <= cluster_upperbound) {
                                                max_block = G.getNodeLabel( prev_node );
                                        }
                                }

                        } else {
                                forall_out_edges(G, e, node) {
                                        NodeID target             = G.getEdgeTarget(e);
                                        PartitionID cur_block     = G.getNodeLabel(target);
                                        hash_map[cur_block] += G.getEdgeWeight(e);
                                        PartitionID cur_value     = hash_map[cur_block];

                                        bool improvement = cur_value > max_value;
                                        improvement |= cur_value == max_value && random_functions::nextBool();

                                        bool sizeconstraint = G.getBlockSize(cur_block) + node_weight <= cluster_upperbound;
                                        sizeconstraint |= cur_block == old_block;

                                        bool cycle = !config.vcycle;
                                        cycle |= G.getSecondPartitionIndex( node ) == G.getSecondPartitionIndex(target);

                                        bool balancing = own_block_balanced || cur_block != old_block;
                                        if( improvement  && sizeconstraint && cycle && balancing) {
                                                max_value = cur_value;
                                                max_block = cur_block;
                                        }
                                } endfor
                        }

                        if( old_block != max_block ) {
                                G.setNodeLabel(node, max_block);

                                G.setBlockSize(old_block, G.getBlockSize(old_block) - node_weight);
                                G.setBlockSize(max_block, G.getBlockSize(max_block) + node_weight);
                        }

                        prev_node = node;
                        G.update_ghost_node_data(); 
                        hash_map.clear();

                } endfor
                G.update_ghost_node_data_finish(); 
        }
}

};


#endif /* end of include guard: PARALLEL_LABEL_COMPRESS_9ME4H8DK */
