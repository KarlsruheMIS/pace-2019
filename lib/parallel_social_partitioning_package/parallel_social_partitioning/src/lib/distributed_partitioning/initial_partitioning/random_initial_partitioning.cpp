/******************************************************************************
 * random_initial_partitioning.cpp   
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

#include "random_initial_partitioning.h"
#include "data_structure/parallel_graph_access.h"
#include "tools/random_functions.h"
#include "tools/distributed_quality_metrics.h"

random_initial_partitioning::random_initial_partitioning() {
                
}

random_initial_partitioning::~random_initial_partitioning() {
                
}


void random_initial_partitioning::perform_partitioning( PartitionConfig & config, parallel_graph_access & G) {

        forall_local_nodes(G, node) {
                G.setNodeLabel(node, random_functions::nextInt(0, config.k-1));
        } endfor
        
        G.update_ghost_node_data_global(); // exchange the labels of ghost nodes

        distributed_quality_metrics qm;
        EdgeWeight edgecut = qm.edge_cut(G);
        double balance = qm.balance(config, G);

        if( MPI::COMM_WORLD.Get_rank() == (int)ROOT) {
                std::cout <<  "log>initial edge edge cut " <<  edgecut  << std::endl;
                std::cout <<  "log>initial imbalance "     <<  balance << std::endl;
        }

}
