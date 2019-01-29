/******************************************************************************
 * mpi_tools.cpp 
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

#include <mpi.h>
#include <unistd.h>

#include "io/parallel_vector_io.h"
#include "io/parallel_graph_io.h"
#include "mpi_tools.h"

mpi_tools::mpi_tools() {
                
}

mpi_tools::~mpi_tools() {
                

}

// TODO: currently this method is for debugging purposses only
// later on this may be a parallel io routine
void mpi_tools::collect_and_write_labels( PartitionConfig & config, parallel_graph_access & G) {
        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();

        std::vector< NodeID > labels;

        if( rank == ROOT ) {
                labels.resize(G.number_of_global_nodes());
                forall_local_nodes(G, node) {
                        labels[node] = G.getNodeLabel(node);
                } endfor
        } else {
                //pack the data
                forall_local_nodes(G, node) {
                        labels.push_back(G.getGlobalID(node));
                        labels.push_back(G.getNodeLabel(node));
                } endfor
        }
        
        if( rank == ROOT) {
                PEID counter = 0;
                while( counter < size-1) {
                        // wait for incomming message of an adjacent processor
                        MPI::Status st;
                        while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,rank,st)) {
                                unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                                std::vector<NodeID> message; message.resize(message_length);

                                MPI::COMM_WORLD.Recv( &message[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), rank+12*size); 
                                counter++;

                                for( unsigned long i = 0; i < message_length-1; i+=2) {
                                        NodeID global_id = message[i];
                                        NodeID label     = message[i+1];

                                        labels[global_id] = label;
                                }
                        }
                }

        } else {
                MPI::COMM_WORLD.Isend( &labels[0], labels.size(), MPI_UNSIGNED_LONG, ROOT, rank+12*size);
        }

        if( rank == ROOT ) {
                std::string clustering_filename("tmpclustering");
                parallel_vector_io pvio;
                pvio.writeVectorSequentially(labels, clustering_filename);
        }
        MPI::COMM_WORLD.Barrier();
}


void mpi_tools::collect_parallel_graph_to_local_graph( PartitionConfig & config, 
                                                       parallel_graph_access & G,
                                                       complete_graph_access & Q) {

        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();

        std::vector< NodeID > message;

        if( rank == ROOT ) {
                Q.start_construction( G.number_of_global_nodes(), G.number_of_global_edges(), 
                                      G.number_of_global_nodes(), G.number_of_global_edges(), false); // no update of comm_rounds!
                Q.set_range(0, G.number_of_global_nodes()); // this graph should contain all global edges
                forall_local_nodes(G, node) {
                        NodeID cur_node = Q.new_node();
                        Q.setNodeWeight(cur_node, G.getNodeWeight(node));
                        Q.setSecondPartitionIndex(cur_node, G.getSecondPartitionIndex(node));
                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);
                                EdgeID e_bar = Q.new_edge(cur_node, G.getGlobalID(target));
                                Q.setEdgeWeight(e_bar, G.getEdgeWeight(e));
                        } endfor
                } endfor
        } else {
                // layout: no local  nodes, no local edges, node_1, pidx, weight, degree,its edges: e_1, w_1, ...,node_2, ... 
                message.push_back(G.number_of_local_nodes());
                forall_local_nodes(G, node) {
                        //message.push_back(G.getGlobalID(node));
                        message.push_back(G.getSecondPartitionIndex(node));
                        message.push_back(G.getNodeWeight(node));
                        message.push_back(G.getNodeDegree(node));
                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);
                                message.push_back(G.getGlobalID(target));
                                message.push_back(G.getEdgeWeight(e));
                        } endfor
                } endfor
        }
        
        //TODO: potentially use Irecv
        if( rank == ROOT) {
                for( unsigned i = 1; i < size; i++) {
                        MPI::Status st;
                        while(!MPI::COMM_WORLD.Iprobe(i,13*size,st)) {}

                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> rmessage; rmessage.resize(message_length);

                        MPI::COMM_WORLD.Recv( &rmessage[0], message_length, MPI_UNSIGNED_LONG, i, 13*size); 

                        NodeID no_nodes = rmessage[0];
                        NodeID pos = 1;
                        for( unsigned long node = 0; node < no_nodes; node++) {
                                NodeID cur_node = Q.new_node();
                                Q.setSecondPartitionIndex(cur_node, rmessage[pos++]);
                                Q.setNodeWeight(cur_node, rmessage[pos++]);

                                EdgeID degree = rmessage[pos++];
                                for( unsigned long e = 0; e < degree; e++) {
                                        EdgeID e_bar = Q.new_edge(cur_node, rmessage[pos++]);
                                        Q.setEdgeWeight(e_bar, rmessage[pos++]);
                                }
                        }

                }
        } else {
                MPI::COMM_WORLD.Isend( &message[0], message.size(), MPI_UNSIGNED_LONG, ROOT, 13*size);
        }

        if( rank == ROOT ) {
                Q.finish_construction();
        }

        MPI::COMM_WORLD.Barrier();
}



void mpi_tools::distribute_local_graph( PartitionConfig & config, 
                                        complete_graph_access & G) {

        int rank       = MPI::COMM_WORLD.Get_rank();

        //first B-Cast number of nodes and number of edges 
        unsigned number_of_nodes = 0;
        unsigned number_of_edges = 0;

        std::vector< int > buffer(2,0);
        if(rank == (int)ROOT) {
                buffer[0] = G.number_of_global_nodes();
                buffer[1] = G.number_of_global_edges();
        }
        MPI::COMM_WORLD.Bcast(&buffer[0], 2, MPI_INT, ROOT);

        number_of_nodes = buffer[0];
        number_of_edges = buffer[1];

        int* xadj;        
        int* adjncy;
        int* vwgt;        
        int* adjwgt;

        if( rank == (int)ROOT) {
                xadj           = G.UNSAFE_metis_style_xadj_array();
                adjncy         = G.UNSAFE_metis_style_adjncy_array();

                vwgt           = G.UNSAFE_metis_style_vwgt_array();
                adjwgt         = G.UNSAFE_metis_style_adjwgt_array();
        } else {
                xadj   = new int[number_of_nodes+1];
                adjncy = new int[number_of_edges];

                vwgt   = new int[number_of_nodes];
                adjwgt = new int[number_of_edges];
        }

        MPI::COMM_WORLD.Bcast(xadj,   number_of_nodes+1, MPI_INT, ROOT);
        MPI::COMM_WORLD.Bcast(adjncy, number_of_edges  , MPI_INT, ROOT);
        MPI::COMM_WORLD.Bcast(vwgt,   number_of_nodes  , MPI_INT, ROOT);
        MPI::COMM_WORLD.Bcast(adjwgt, number_of_edges  , MPI_INT, ROOT);

        G.build_from_metis_weighted( number_of_nodes, xadj, adjncy, vwgt, adjwgt); 

        delete[] xadj;
        delete[] adjncy;
        delete[] vwgt;
        delete[] adjwgt;
}
