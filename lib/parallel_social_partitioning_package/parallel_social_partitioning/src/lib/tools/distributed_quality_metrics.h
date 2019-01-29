/******************************************************************************
 * distributed_quality_metrics.h 
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


#ifndef DISTRIBUTED_QUALITY_METRICS_UAVSEXBT
#define DISTRIBUTED_QUALITY_METRICS_UAVSEXBT

#include "definitions.h"
#include "data_structure/parallel_graph_access.h"
#include "partition_config.h"

class distributed_quality_metrics {
public:
        distributed_quality_metrics();
        virtual ~distributed_quality_metrics();

        EdgeWeight local_edge_cut( parallel_graph_access & G, int * partition_map );
        EdgeWeight edge_cut( parallel_graph_access & G );
        EdgeWeight edge_cut_second( parallel_graph_access & G );
        NodeWeight local_max_block_weight( PartitionConfig & config, parallel_graph_access & G, int * partition_map );
        double balance( PartitionConfig & config, parallel_graph_access & G );
        double balance_second( PartitionConfig & config, parallel_graph_access & G );
};


#endif /* end of include guard: DISTRIBUTED_QUALITY_METRICS_UAVSEXBT */
