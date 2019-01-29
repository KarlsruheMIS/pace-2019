/******************************************************************************
 * random_initial_partitioning.h 
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

#ifndef RANDOM_INITIAL_PARTITIONING_FM8LJSI0
#define RANDOM_INITIAL_PARTITIONING_FM8LJSI0

#include "partition_config.h"

class parallel_graph_access;

class random_initial_partitioning {
public:
        random_initial_partitioning();
        virtual ~random_initial_partitioning();
        
        void perform_partitioning( PartitionConfig & config, parallel_graph_access & G);
};


#endif /* end of include guard: RANDOM_INITIAL_PARTITIONING_FM8LJSI0 */
