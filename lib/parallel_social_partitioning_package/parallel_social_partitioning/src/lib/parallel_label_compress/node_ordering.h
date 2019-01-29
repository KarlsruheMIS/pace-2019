/******************************************************************************
 * node_ordering.h 
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

#ifndef NODE_ORDERING_HM1YMLB1
#define NODE_ORDERING_HM1YMLB1

#include <algorithm>

#include "definitions.h"
#include "partition_config.h"
#include "data_structure/parallel_graph_access.h"
#include "tools/random_functions.h"

class node_ordering {
public:
        node_ordering();
        virtual ~node_ordering();

        void order_nodes(const PartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) {
                forall_local_nodes(G, node) {
                        ordered_nodes[node] = node;
                } endfor

                switch( config.node_ordering ) {
                        case RANDOM_NODEORDERING:
                                order_nodes_random(config, G, ordered_nodes);
                             break;
                        case DEGREE_NODEORDERING:
                                order_nodes_degree(config, G, ordered_nodes);
                             break;
                        case LEASTGHOSTNODESFIRST_DEGREE_NODEODERING:
                                order_leastghostnodes_nodes_degree(config, G, ordered_nodes);
                             break;
                        case DEGREE_LEASTGHOSTNODESFIRST_NODEODERING:
                                order_nodes_degree_leastghostnodes(config, G, ordered_nodes);
                             break;
                 }
        }

        void order_nodes_random(const PartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                random_functions::permutate_vector_fast(ordered_nodes, false);
        }

        void order_nodes_degree(const PartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                std::sort( ordered_nodes.begin(), ordered_nodes.end(), 
                           [&]( const NodeID & lhs, const NodeID & rhs) -> bool {
                                return (G.getNodeDegree(lhs) < G.getNodeDegree(rhs));
                           });
        }

	//TODO:??
        void order_leastghostnodes_nodes_degree(const PartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                std::sort( ordered_nodes.begin(), ordered_nodes.end(), 
                           [&]( const NodeID & lhs, const NodeID & rhs) -> bool {
                                return (G.getNodeDegree(lhs) < G.getNodeDegree(rhs));
                           });
        }

	//TODO:??
	void order_nodes_degree_leastghostnodes(const PartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                std::sort( ordered_nodes.begin(), ordered_nodes.end(), 
                           [&]( const NodeID & lhs, const NodeID & rhs) -> bool {
				return (G.getNodeDegree(lhs) < G.getNodeDegree(rhs));
                           });
        }


 
};


#endif /* end of include guard: NODE_ORDERING_HM1YMLB1 */
