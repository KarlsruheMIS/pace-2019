/******************************************************************************
 * distributed_quality_metrics.cpp 
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

#include "distributed_quality_metrics.h"

distributed_quality_metrics::distributed_quality_metrics() {
                
}

distributed_quality_metrics::~distributed_quality_metrics() {
                        
}

EdgeWeight distributed_quality_metrics::edge_cut_second( parallel_graph_access & G ) {
        EdgeWeight local_cut = 0;
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( G.getSecondPartitionIndex( node ) != G.getSecondPartitionIndex(target)) {
                                local_cut += G.getEdgeWeight(e);
                        }
                } endfor
        } endfor

        EdgeWeight global_cut = 0;
        MPI::COMM_WORLD.Allreduce(&local_cut, &global_cut, 1, MPI_UNSIGNED_LONG, MPI_SUM);

        return global_cut/2;
}

EdgeWeight distributed_quality_metrics::local_edge_cut( parallel_graph_access & G, int* partition_map ) {
        EdgeWeight local_cut = 0;
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( partition_map[ node ] != partition_map[ target ]) {
                                local_cut += G.getEdgeWeight(e);
                        }
                } endfor
        } endfor

        return local_cut/2;
}

EdgeWeight distributed_quality_metrics::edge_cut( parallel_graph_access & G ) {
        EdgeWeight local_cut = 0;
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( G.getNodeLabel( node ) != G.getNodeLabel(target)) {
                                local_cut += G.getEdgeWeight(e);
                        }
                } endfor
        } endfor

        EdgeWeight global_cut = 0;
        MPI::COMM_WORLD.Allreduce(&local_cut, &global_cut, 1, MPI_UNSIGNED_LONG, MPI_SUM);

        return global_cut/2;
}

NodeWeight distributed_quality_metrics::local_max_block_weight( PartitionConfig & config, parallel_graph_access & G, int * partition_map ) {
        std::vector<PartitionID> block_weights(config.k, 0);

        NodeWeight graph_vertex_weight = 0;

        forall_local_nodes(G, n) {
                PartitionID curPartition     = partition_map[n];
                if( curPartition >= config.k ) {
                        std::cout <<  "problem"  << std::endl;
                        std::cout <<  curPartition  << std::endl;
                        exit(0);
                }
                block_weights[curPartition] += G.getNodeWeight(n);
                graph_vertex_weight   += G.getNodeWeight(n);
        } endfor

        NodeWeight cur_max = 0;

        for( PartitionID block = 0; block < config.k; block++) {
                NodeWeight cur_weight = block_weights[block];
                if (cur_weight > cur_max) {
                        cur_max = cur_weight;
                }
        }

        return cur_max;
}

double distributed_quality_metrics::balance( PartitionConfig & config, parallel_graph_access & G ) {
        std::vector<PartitionID> block_weights(config.k, 0);

        NodeWeight local_graph_vertex_weight = 0;

        forall_local_nodes(G, n) {
                PartitionID curPartition     = G.getNodeLabel(n);
                block_weights[curPartition] += G.getNodeWeight(n);
                local_graph_vertex_weight   += G.getNodeWeight(n);
        } endfor

        std::vector<PartitionID> overall_weights(config.k, 0);
        MPI::COMM_WORLD.Allreduce(&block_weights[0], &overall_weights[0], config.k, MPI_UNSIGNED_LONG, MPI_SUM);

        NodeWeight graph_vertex_weight = 0;
        MPI::COMM_WORLD.Allreduce(&local_graph_vertex_weight, &graph_vertex_weight, 1, MPI_UNSIGNED_LONG, MPI_SUM);

        double balance_part_weight = ceil(graph_vertex_weight / (double)config.k);
        double cur_max             = -1;

        for( PartitionID block = 0; block < config.k; block++) {
                double cur = overall_weights[block];
                if (cur > cur_max) {
                        cur_max = cur;
                }
        }

        double percentage = cur_max/balance_part_weight;
        return percentage;
}

double distributed_quality_metrics::balance_second( PartitionConfig & config, parallel_graph_access & G ) {
        std::vector<PartitionID> block_weights(config.k, 0);

        NodeWeight local_graph_vertex_weight = 0;

        forall_local_nodes(G, n) {
                PartitionID curPartition     = G.getSecondPartitionIndex(n);
                block_weights[curPartition] += G.getNodeWeight(n);
                local_graph_vertex_weight   += G.getNodeWeight(n);
        } endfor

        std::vector<PartitionID> overall_weights(config.k, 0);
        MPI::COMM_WORLD.Allreduce(&block_weights[0], &overall_weights[0], config.k, MPI_UNSIGNED_LONG, MPI_SUM);

        NodeWeight graph_vertex_weight = 0;
        MPI::COMM_WORLD.Allreduce(&local_graph_vertex_weight, &graph_vertex_weight, 1, MPI_UNSIGNED_LONG, MPI_SUM);

        double balance_part_weight = ceil(graph_vertex_weight / (double)config.k);
        double cur_max             = -1;

        for( PartitionID block = 0; block < config.k; block++) {
                double cur = overall_weights[block];
                if (cur > cur_max) {
                        cur_max = cur;
                }
        }

        double percentage = cur_max/balance_part_weight;
        return percentage;
}
