/******************************************************************************
 * parallel_label_compress_reps.cpp 
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

#include <argtable2.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <mpi.h>
#include <regex.h>
#include <sstream>
#include <stdio.h>
#include <string.h> 

#include "communication/mpi_tools.h"
#include "communication/dummy_operations.h"
#include "data_structure/parallel_graph_access.h"
#include "distributed_partitioning/distributed_partitioner.h"
#include "graph_generation/generate_kronecker.h"
#include "io/parallel_graph_io.h"
#include "io/parallel_vector_io.h"
#include "macros_assertions.h"
#include "parse_parameters.h"
#include "partition_config.h"
#include "random_functions.h"
#include "timer.h"
#include "tools/distributed_quality_metrics.h"
#include "graph_generation/generate_rgg.h"

int main(int argn, char **argv) {

        MPI::Init(argn, argv);    /* starts MPI */

        PartitionConfig partition_config;
        std::string graph_filename;

        int ret_code = parse_parameters(argn, argv, 
                                        partition_config, 
                                        graph_filename); 

        if(ret_code) {
                MPI::Finalize();
                return 0;
        }

        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();

        if(rank == ROOT) {
                std::cout <<  "log> cluster coarsening factor is set to " <<  partition_config.cluster_coarsening_factor  << std::endl;
        }

        partition_config.stop_factor /= partition_config.k;
        if(rank != 0) partition_config.seed = partition_config.seed*size+rank; 

        srand(partition_config.seed);

        parallel_graph_access G;
        timer t;
        if(partition_config.generate_kronecker) {
                if( rank == ROOT ) std::cout <<  "generating a kronecker graph"  << std::endl;
                t.restart();

                generate_kronecker gk;
                gk.generate_kronecker_graph( partition_config, G);
                if( rank == ROOT ) std::cout <<  "generation of kronecker graph took " <<  t.elapsed()  << std::endl;
        } else if(partition_config.generate_rgg) {
                if( rank == ROOT ) std::cout <<  "generating a rgg graph"  << std::endl;
                t.restart();

                generator_rgg grgg;
                PartitionConfig copy = partition_config;
                copy.seed = 1;
                grgg.generate(partition_config, G);
                if( rank == ROOT ) std::cout <<  "generation of rgg graph took " <<  t.elapsed()  << std::endl;

        } else {
                parallel_graph_io::readGraphWeighted(G, graph_filename, rank, size);
                if( rank == ROOT ) std::cout <<  "took " <<  t.elapsed()  << std::endl;
        }

        random_functions::setSeed(partition_config.seed);
        parallel_graph_access::set_comm_rounds( partition_config.comm_rounds/size );
        parallel_graph_access::set_comm_rounds_up( partition_config.comm_rounds/size);
        distributed_partitioner::generate_random_choices( partition_config );

        G.printMemoryUsage(std::cout);

        std::cout <<  G.number_of_local_edges()  << std::endl;

        MPI::COMM_WORLD.Barrier();
        {
                t.restart();
                if( rank == ROOT ) std::cout <<  "running collective dummy operations ";
                dummy_operations dop;
                dop.run_collective_dummy_operations();
        }
        MPI::COMM_WORLD.Barrier();

        if( rank == ROOT ) {
                std::cout <<  "took " <<  t.elapsed()  << std::endl;
        }

        //compute some stats
        EdgeWeight interPEedges = 0;
        EdgeWeight localEdges = 0;
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if(!G.is_local_node(target)) {
                                interPEedges++;
                        } else {
                                localEdges++;
                        }
                } endfor
        } endfor

        EdgeWeight globalInterEdges = 0;
        EdgeWeight globalIntraEdges = 0;
        MPI::COMM_WORLD.Reduce( &interPEedges, &globalInterEdges, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT);
        MPI::COMM_WORLD.Reduce( &localEdges,   &globalIntraEdges, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT);

        if( rank == ROOT ) {
                std::cout <<  "log> ghost edges " <<  globalInterEdges/(double)G.number_of_global_edges() << std::endl;
                std::cout <<  "log> local edges " <<  globalIntraEdges/(double)G.number_of_global_edges() << std::endl;
        }

        t.restart();
	double epsilon = (partition_config.inbalance)/100.0;
        partition_config.number_of_overall_nodes = G.number_of_global_nodes();
        partition_config.upper_bound_partition   = (1+epsilon)*ceil(G.number_of_global_nodes()/(double)partition_config.k);

        double avg_running_time = 0;
        EdgeWeight avg_cut = 0;
        EdgeWeight best_cut = std::numeric_limits<EdgeWeight>::max();
        double best_balance = 0;
        double avg_balance = 0;
        for( int i = 0; i < partition_config.num_tries; i++) {
                MPI::COMM_WORLD.Barrier();
                t.restart();
                distributed_partitioner dpart;
                PartitionConfig working_config = partition_config;
                dpart.perform_partitioning( working_config, G);
                MPI::COMM_WORLD.Barrier();
                double cur_time = t.elapsed();
                avg_running_time += cur_time;

                distributed_quality_metrics qm;
                EdgeWeight cur_cut = qm.edge_cut( G );
                double cur_balance = qm.balance( partition_config, G );
                avg_cut += cur_cut;
                avg_balance += cur_balance ;

                if( rank == ROOT ) {
                        std::cout <<  "log> trie " << i << " cut " << cur_cut << " balance " << cur_balance << " time " << cur_time << std::endl;
                }
                if(cur_cut < best_cut) {
                        best_cut = cur_cut;
                        best_balance = cur_balance;
                }
                if( i != partition_config.num_tries - 1) {
                        forall_local_nodes(G, node) {
                                G.setSecondPartitionIndex(node, 0);
                                G.setNodeLabel(node, G.getGlobalID(node));
                        } endfor

                        forall_ghost_nodes(G, node) {
                                G.setSecondPartitionIndex(node, 0);
                                G.setNodeLabel(node, G.getGlobalID(node));
                        } endfor
                }
                
                partition_config.vcycle=false;
        }

        if( rank == 0 ) {
                std::cout << "log>" << "=====================================" << std::endl;
                std::cout << "log>" << "============AND WE R DONE============" << std::endl;
                std::cout << "log>" << "=====================================" << std::endl;
                std::cout <<  "log>avg partitioning time elapsed " << std::setprecision(5) << 1.0*avg_running_time/partition_config.num_tries << std::endl;
                std::cout <<  "log>avg edge cut " << std::setprecision(20) << 1.0*avg_cut/partition_config.num_tries << std::endl;
                std::cout <<  "log>avg balance "  << std::setprecision(5) << 1.0*avg_balance/partition_config.num_tries << std::endl;
                std::cout <<  "log>best edge cut " <<  best_cut << std::endl;
                std::cout <<  "log>best balance "  <<  best_balance << std::endl;
        }

        parallel_vector_io pvio;
        std::string filename("tmppartition");
        pvio.writePartitionSimpleParallel<NodeID>(G, filename);


#ifndef NDEBUG
        MPI::Status st;
        while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,MPI::ANY_TAG,st)){
                std::cout <<  "attention: still incoming messages! rank " <<  rank <<  " from " <<  st.Get_source()  << std::endl;

                unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                std::vector<NodeID> message; message.resize(message_length);
                MPI::COMM_WORLD.Recv( &message[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), st.Get_tag()); 
        } 
#endif

        MPI::Finalize();
}
