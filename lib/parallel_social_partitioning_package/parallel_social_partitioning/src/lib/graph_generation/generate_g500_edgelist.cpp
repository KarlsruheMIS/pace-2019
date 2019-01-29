/******************************************************************************
 * generate_g500_edgelist.cpp 
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

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#endif

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

#include <mpi.h>
#include "graph500_generator/make_graph.h"
#include "generate_g500_edgelist.h"

generate_g500_edgelist::generate_g500_edgelist() {
                
}

generate_g500_edgelist::~generate_g500_edgelist() {
                
}

void generate_g500_edgelist::generate_kronecker_edgelist( PartitionConfig & config, 
                                                          std::vector< source_target_pair > & edge_list ) {
        int log_numverts;
        int size, rank;
        unsigned long my_edges;
        unsigned long global_edges;
        double start, stop;
        size_t i;


        log_numverts = config.log_num_verts; /* In base 2 */

        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int64_t desired_edges = ((int64_t)config.edge_factor) << log_numverts;
        if (rank == 0) printf("graph size is %" PRId64 " vertices and %" PRId64 " edges\n", INT64_C(1) << log_numverts, desired_edges);

        /* Start of graph generation timing */
        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();
        int64_t nedges;

        packed_edge* result;
        make_graph(config.log_num_verts, desired_edges, 1, 2, &nedges, &result);

        MPI_Barrier(MPI_COMM_WORLD);
        stop = MPI_Wtime();
        /* End of graph generation timing */

        my_edges = nedges;

        for (i = 0; i < my_edges; ++i) {
                assert ((get_v0_from_edge(&result[i]) >> log_numverts) == 0);
                assert ((get_v1_from_edge(&result[i]) >> log_numverts) == 0);
        }

        int64_t j = 0;
        for(  j = 0; j < nedges; j++) {
                //ignore self loops
                if( get_v0_from_edge(&(result[j])) == get_v1_from_edge(&(result[j]))) continue;

                // we want the graph to be undirected
                source_target_pair st_forward;
                source_target_pair st_backward;

                st_forward.source = get_v0_from_edge(&(result[j]));
                st_forward.target = get_v1_from_edge(&(result[j]));

                st_backward.source = get_v1_from_edge(&(result[j]));
                st_backward.target = get_v0_from_edge(&(result[j]));
                
                edge_list.push_back(st_forward);
                edge_list.push_back(st_backward);
        }

        free(result);

        MPI_Reduce(&my_edges, &global_edges, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) {
                printf("%lu edge%s generated in %fs (%f Medges/s on %d processor%s)\n", global_edges, (global_edges == 1 ? "" : "s"), (stop - start), global_edges / (stop - start) * 1.e-6, size, (size == 1 ? "" : "s"));
        }


}
