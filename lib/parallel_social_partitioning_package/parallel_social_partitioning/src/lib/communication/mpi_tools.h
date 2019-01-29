/******************************************************************************
 * mpi_tools.h 
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

#ifndef MPI_TOOLS_HMESDXF2
#define MPI_TOOLS_HMESDXF2

#include "data_structure/parallel_graph_access.h"
#include "partition_config.h"

class mpi_tools {
public:
        mpi_tools();
        virtual ~mpi_tools();

        void collect_and_write_labels( PartitionConfig & config, 
                                       parallel_graph_access & G);

        void collect_parallel_graph_to_local_graph( PartitionConfig & config, 
                                                    parallel_graph_access & G,
                                                    complete_graph_access & Q);

        // G is input (only on ROOT)
        // G is output (on every other PE)
        void distribute_local_graph( PartitionConfig & config, complete_graph_access & G);
};


#endif /* end of include guard: MPI_TOOLS_HMESDXF2 */
