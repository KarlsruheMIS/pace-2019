/******************************************************************************
 * configuration.h 
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


#ifndef CONFIGURATION_3APG5V7ZA
#define CONFIGURATION_3APG5V7ZA

#include "partition_config.h"

class configuration {
        public:
                configuration() {} ;
                virtual ~configuration() {};

                void standard( PartitionConfig & config );
                void ultrafast( PartitionConfig & config );
                void fast( PartitionConfig & config );
                void eco( PartitionConfig & config );
                void strong( PartitionConfig & config );
};

inline void configuration::ultrafast( PartitionConfig & partition_config ) {
        partition_config.initial_partitioning_algorithm  = KAFFPAEULTRAFASTSNW;
        partition_config.no_refinement_in_last_iteration = true;
        partition_config.stop_factor                     = 18000;
        partition_config.num_vcycles                     = 1;
        }


inline void configuration::fast( PartitionConfig & partition_config ) {
        partition_config.initial_partitioning_algorithm  = KAFFPAEULTRAFASTSNW;
        partition_config.no_refinement_in_last_iteration = true;
        partition_config.stop_factor                     = 18000;
}

inline void configuration::eco( PartitionConfig & partition_config ) {
        partition_config.initial_partitioning_algorithm  = KAFFPAEFASTSNW;
        partition_config.no_refinement_in_last_iteration = true;
        partition_config.stop_factor                     = 18000;
        partition_config.evolutionary_time_limit         = 2048/MPI::COMM_WORLD.Get_size();
        partition_config.eco                             = true;
        partition_config.num_vcycles                     = 6;
}

inline void configuration::strong( PartitionConfig & partition_config ) {
        partition_config.initial_partitioning_algorithm = KAFFPAESTRONGSNW;

}
inline void configuration::standard( PartitionConfig & partition_config ) {
        partition_config.seed                                   = 0;
        partition_config.k                                      = 2;
        partition_config.inbalance                              = 3;
        partition_config.epsilon                                = 3;
        partition_config.time_limit 				= 0; 
        partition_config.evolutionary_time_limit 	        = 0; 
        partition_config.log_num_verts                          = 16;
        partition_config.edge_factor                            = 16;
        partition_config.generate_kronecker                     = false; 
        partition_config.generate_rgg                           = false; 
        partition_config.comm_rounds                            = 128; 
        partition_config.label_iterations                       = 4;
        partition_config.label_iterations_coarsening            = 3;
        partition_config.label_iterations_refinement            = 6;
        partition_config.cluster_coarsening_factor              = 14;
        partition_config.initial_partitioning_algorithm         = KAFFPAEFASTSNW;
        partition_config.stop_factor                            = 14000;
        partition_config.vcycle                                 = false;
        partition_config.num_vcycles                            = 2;
        partition_config.num_tries                              = 10;
        partition_config.node_ordering                          = DEGREE_NODEORDERING;
        partition_config.no_refinement_in_last_iteration        = false;
        partition_config.ht_fill_factor                         = 1.6;
        partition_config.eco                                    = false;
}

#endif /* end of include guard: CONFIGURATION_3APG5V7Z */
