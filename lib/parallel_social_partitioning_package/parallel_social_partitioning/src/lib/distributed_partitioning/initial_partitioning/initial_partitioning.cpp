/******************************************************************************
 * initial_partitioning.cpp 
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

#include "distributed_evolutionary_partitioning.h"
#include "initial_partitioning.h"
#include "random_initial_partitioning.h"

initial_partitioning_algorithm::initial_partitioning_algorithm() {
                
}

initial_partitioning_algorithm::~initial_partitioning_algorithm() {
                
}


void initial_partitioning_algorithm::perform_partitioning( PartitionConfig & config, parallel_graph_access & Q) {
        if( config.initial_partitioning_algorithm == RANDOMIP) {
                random_initial_partitioning dist_rpart;
                dist_rpart.perform_partitioning( config, Q );
        } else {
                distributed_evolutionary_partitioning dist_epart;
                dist_epart.perform_partitioning( config, Q);
        }
}

