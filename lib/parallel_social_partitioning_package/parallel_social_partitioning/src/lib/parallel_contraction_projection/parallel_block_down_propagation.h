/******************************************************************************
 * parallel_block_down_propagation.h 
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

#ifndef PARALLEL_BLOCK_DOWN_PROPAGATION_SRTCMH8F
#define PARALLEL_BLOCK_DOWN_PROPAGATION_SRTCMH8F

#include "data_structure/parallel_graph_access.h"
#include "partition_config.h"

class parallel_block_down_propagation {
public:
        parallel_block_down_propagation();
        virtual ~parallel_block_down_propagation();

        void propagate_block_down( PartitionConfig & config, 
                                   parallel_graph_access & G, 
                                   parallel_graph_access & Q);

private:

        void update_ghost_nodes_blocks( parallel_graph_access & G ); 

        std::vector< std::vector< NodeID > > m_messages;
        std::vector< std::vector< NodeID > > m_send_buffers; // buffers to send messages
};


#endif /* end of include guard: PARALLEL_BLOCK_DOWN_PROPAGATION_SRTCMH8F */
