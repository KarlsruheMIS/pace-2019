/******************************************************************************
 * distributed_partitioner.h       
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

#ifndef DISTRIBUTED_PARTITIONER_ZYL2XF6R
#define DISTRIBUTED_PARTITIONER_ZYL2XF6R

#include <vector>
#include "partition_config.h"
#include "data_structure/parallel_graph_access.h"
#include "stop_rule.h"

class distributed_partitioner {
public:
        distributed_partitioner();
        virtual ~distributed_partitioner();

        void perform_partitioning( PartitionConfig & config, parallel_graph_access & G);
        void check( PartitionConfig & config, parallel_graph_access & G);
        void check_labels( PartitionConfig & config, parallel_graph_access & G);
        static void generate_random_choices( PartitionConfig & config ) ;
private: 
        void vcycle( PartitionConfig & config, parallel_graph_access & G );
        stop_rule contraction_stop_decision;
        NodeWeight m_total_graph_weight;
        NodeID m_cur_rnd_choice;

        static std::vector< NodeID > m_cf;
        static std::vector< NodeID > m_sf;
        static std::vector< NodeID > m_lic;
        int m_level;
        int m_cycle;
        timer m_t; 
};


#endif /* end of include guard: DISTRIBUTED_PARTITIONER_ZYL2XF6R */
