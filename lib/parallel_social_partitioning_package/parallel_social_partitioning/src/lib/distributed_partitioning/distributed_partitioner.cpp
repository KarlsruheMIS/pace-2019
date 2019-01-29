/******************************************************************************
 * distributed_partitioner.cpp 
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

#include <sstream>
#include "communication/mpi_tools.h"
#include "distributed_partitioner.h"
#include "initial_partitioning/initial_partitioning.h"
#include "io/parallel_graph_io.h"
#include "parallel_contraction_projection/parallel_contraction.h"
#include "parallel_contraction_projection/parallel_block_down_propagation.h"
#include "parallel_contraction_projection/parallel_projection.h"
#include "parallel_label_compress/parallel_label_compress.h"
#include "stop_rule.h"
#include "tools/distributed_quality_metrics.h"
#include "tools/random_functions.h"
#include "data_structure/linear_probing_hashmap.h"

std::vector< NodeID > distributed_partitioner::m_cf = std::vector< NodeID >();
std::vector< NodeID > distributed_partitioner::m_sf = std::vector< NodeID >();
std::vector< NodeID > distributed_partitioner::m_lic = std::vector< NodeID >();

distributed_partitioner::distributed_partitioner() {
       m_total_graph_weight = std::numeric_limits< NodeWeight >::max(); 
       m_cur_rnd_choice = 0;
       m_level = -1;
       m_cycle = 0;
}

distributed_partitioner::~distributed_partitioner() {
}

void distributed_partitioner::generate_random_choices( PartitionConfig & config ){
        for( int i = 0; i < config.num_tries; i++) {
                for( int j = 0; j < config.num_vcycles; j++) {
                        m_cf.push_back(random_functions::nextDouble( 10, 25 ));
                        m_sf.push_back(random_functions::nextInt( 20, 500 ));
                        m_lic.push_back(random_functions::nextInt( 2, 15 ));
                }
        }
}

void distributed_partitioner::perform_partitioning( PartitionConfig & partition_config, parallel_graph_access & G) {
        timer t; 
        double elapsed = 0;
        m_cur_rnd_choice = 0;
        PartitionConfig config = partition_config;
        config.vcycle = false;
        

        PEID rank = MPI::COMM_WORLD.Get_rank();
        for( int cycle = 0; cycle < partition_config.num_vcycles; cycle++) {
                t.restart();
                m_cycle = cycle;

                if(cycle+1 == partition_config.num_vcycles && partition_config.no_refinement_in_last_iteration) {
                        config.label_iterations_refinement = 0;
                }

                vcycle( config, G );

                if( rank == ROOT ) {
                        std::cout <<  "log>cycle: " << m_cycle << " uncoarsening took " << m_t.elapsed()  << std::endl;
                }
#ifndef NDEBUG
                check_labels(config, G);
#endif

                elapsed += t.elapsed();

#ifndef NOOUTPUT
                distributed_quality_metrics qm;
                EdgeWeight edge_cut = qm.edge_cut( G );
                double balance      = qm.balance( config, G );

                if( rank == ROOT ) {
                        std::cout <<  "log>cycle: " << cycle << " k " <<  config.k << " cut " << edge_cut << " balance " << balance << " time " <<  elapsed  << std::endl;
                }
#endif 
                t.restart();
                m_t.restart();
                if( cycle+1 < config.num_vcycles ) {
                        forall_local_nodes(G, node) {
                                G.setSecondPartitionIndex(node, G.getNodeLabel(node));
                                G.setNodeLabel(node, G.getGlobalID(node));
                        } endfor

                        forall_ghost_nodes(G, node) {
                                G.setSecondPartitionIndex(node, G.getNodeLabel(node));
                                G.setNodeLabel(node, G.getGlobalID(node));
                        } endfor
                }

                config.vcycle = true;

                if( rank == ROOT && config.eco ) {
                        config.cluster_coarsening_factor = m_cf[m_cur_rnd_choice++];
                        ////config.stop_factor = m_sf[m_cur_rnd_choice];
                        ////config.label_iterations_coarsening = m_lic[m_cur_rnd_choice++];
                }

                if(config.eco) {
                        MPI::COMM_WORLD.Bcast(&(config.cluster_coarsening_factor), 1, MPI_DOUBLE, ROOT);
                        std::cout << "cf " << config.cluster_coarsening_factor  << std::endl;
                }
                config.evolutionary_time_limit = 0;
                //MPI::COMM_WORLD.Bcast(&(config.stop_factor), 1, MPI_INT, ROOT);
                //MPI::COMM_WORLD.Bcast(&(config.label_iterations_coarsening), 1, MPI_INT, ROOT);
                //if( cycle % 2 == 0 ) {
                        //config.cluster_coarsening_factor = 20000;
                //} else {
                        //config.cluster_coarsening_factor = 20;
                //}
                elapsed += t.elapsed();
                MPI::COMM_WORLD.Barrier();
        }
}
void distributed_partitioner::vcycle( PartitionConfig & partition_config, parallel_graph_access & G) {
        //MPI::COMM_WORLD.Barrier();
        PartitionConfig config = partition_config;

        mpi_tools mpitools;
        timer t; 

        if( m_total_graph_weight == std::numeric_limits< NodeWeight >::max() ) {
                m_total_graph_weight = G.number_of_global_nodes();
        }
        PEID rank = MPI::COMM_WORLD.Get_rank();
#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout << "log>" << "=====================================" << std::endl;
                std::cout << "log>" << "=============NEXT LEVEL==============" << std::endl;
                std::cout << "log>" << "=====================================" << std::endl;
        }
#endif
        t.restart();

        
        m_level++;
        config.label_iterations = config.label_iterations_coarsening;
        config.total_num_labels = G.number_of_global_nodes();
        //config.upper_bound_cluster = std::min(config.upper_bound_partition/config.cluster_coarsening_factor, 1000*m_total_graph_weight/G.number_of_global_nodes());
        //
        config.upper_bound_cluster = config.upper_bound_partition/(1.0*config.cluster_coarsening_factor);
        G.init_balance_management( config );

        //parallel_label_compress< std::unordered_map< NodeID, NodeWeight> > plc;
        parallel_label_compress< linear_probing_hashmap  > plc;
        plc.perform_parallel_label_compression ( config, G, true);

#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout <<  "log>cycle: " << m_cycle << " level: " << m_level  << " parallel label compression took " <<  t.elapsed() << std::endl;
        }
#endif

        parallel_graph_access Q;
        t.restart();

        {
                parallel_contraction parallel_contract;
                parallel_contract.contract_to_distributed_quotient( config, G, Q); // contains one Barrier

                parallel_block_down_propagation pbdp;
                if( config.vcycle ) {
                        // in this case we have to propagate the partitionindex down
                        pbdp.propagate_block_down( config, G, Q);
                }
        
                MPI::COMM_WORLD.Barrier();
        }
              

#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout <<  "log>cycle: " << m_cycle << " level: " << m_level << " contraction took " <<  t.elapsed() << std::endl;
                std::cout <<  "log>cycle: " << m_cycle << " level: " << m_level << " coarse nodes n=" << Q.number_of_global_nodes() << ", coarse edges m=" << Q.number_of_global_edges() << std::endl;
        }
#endif

        if( !contraction_stop_decision.contraction_stop(config, G, Q)) {
                vcycle( config, Q);
        } else {
#ifndef NOOUTPUT
                if( rank == ROOT ) {
                        std::cout << "log>" << "=====================================" << std::endl;
                        std::cout << "log>" << "================ IP =================" << std::endl;
                        std::cout << "log>" << "=====================================" << std::endl;
                        std::cout <<  "log>cycle: " << m_cycle << " total number of levels " <<  (m_level+1) << std::endl;
                        std::cout <<  "log>cycle: " << m_cycle << " number of coarsest nodes " <<  Q.number_of_global_nodes() << std::endl;
                        std::cout <<  "log>cycle: " << m_cycle << " number of coarsest edges " <<  Q.number_of_global_edges() << std::endl;
                        std::cout <<  "log>cycle: " << m_cycle << " coarsening took  " <<  m_t.elapsed()  << std::endl;
                }
#endif
                t.restart();

                //MPI::COMM_WORLD.Barrier();
                initial_partitioning_algorithm ip;
                ip.perform_partitioning( config, Q );

#ifndef NOOUTPUT
                if( rank == ROOT ) {
                        std::cout <<  "log>cycle: " << m_cycle << " initial partitioning took " <<  t.elapsed() << std::endl;
                }
                m_t.restart();
#endif
        }

#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout << "log>" << "=====================================" << std::endl;
                std::cout << "log>" << "============PREV LEVEL ==============" << std::endl;
                std::cout << "log>" << "=====================================" << std::endl;
        }
#endif

        t.restart();
        parallel_projection parallel_project;
        parallel_project.parallel_project( G, Q ); // contains a Barrier

#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout <<  "log>cycle: " << m_cycle << " level: " << m_level << " projection took " <<  t.elapsed() << std::endl;
        }
#endif

        t.restart();
        config.label_iterations = config.label_iterations_refinement;

        if( config.label_iterations != 0 ) {
                config.total_num_labels = config.k;
                config.upper_bound_cluster = config.upper_bound_partition;


                G.init_balance_management( config );
                PartitionConfig working_config = config;
                working_config.vcycle = false; // assure that we actually can improve the cut

                parallel_label_compress< std::vector< NodeWeight> > plc_refinement;
                plc_refinement.perform_parallel_label_compression( working_config, G, false, false);
        }
        //MPI::COMM_WORLD.Barrier();

#ifndef NOOUTPUT
        if( rank == ROOT ) {
                std::cout <<  "log>cycle: " << m_cycle <<" level: " << m_level << " label compression refinement took " <<  t.elapsed() << std::endl;
        }
#endif
        m_level--;
}

void distributed_partitioner::check_labels( PartitionConfig & config, parallel_graph_access & G) {
        PEID m_rank = MPI::COMM_WORLD.Get_rank();
        PEID m_size = MPI::COMM_WORLD.Get_size();
        std::vector< std::vector< NodeID > > send_buffers; // buffers to send messages
        send_buffers.resize(m_size);
        std::vector<bool> m_PE_packed;
                m_PE_packed.resize(m_size); 
                for( unsigned peID = 0; peID < m_PE_packed.size(); peID++) {
                        m_PE_packed[ peID ]           = false;
                }

        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                PEID peID = G.getTargetPE(target);
                                if( !m_PE_packed[peID] ) { // make sure a node is sent at most once
                                        send_buffers[peID].push_back(G.getGlobalID(node));
                                        send_buffers[peID].push_back(G.getNodeLabel(node));
                                        m_PE_packed[peID] = true;
                                }
                        }
                } endfor
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                m_PE_packed[G.getTargetPE(target)] = false;
                        }
                } endfor
        } endfor

        //send all neighbors their packages using Isends
        //a neighbor that does not receive something gets a specific token
        for( PEID peID = 0; peID < send_buffers.size(); peID++) {
                if( G.is_adjacent_PE(peID) ) {
                        //now we have to send a message
                        if( send_buffers[peID].size() == 0 ){
                                // length 1 encode no message
                                send_buffers[peID].push_back(0);
                        }

                        //std::cout << m_send_buffers[peID].size()  << std::endl;
                        MPI::COMM_WORLD.Isend( &send_buffers[peID][0], 
                                                send_buffers[peID].size(), MPI_UNSIGNED_LONG, peID, peID+17*m_size);
                }
        }

        //receive incomming
        PEID counter = 0;
        while( counter < G.getNumberOfAdjacentPEs()) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                unsigned int tag = m_rank+17*m_size;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,tag,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> message; message.resize(message_length);

                        MPI::COMM_WORLD.Recv( &message[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), tag); 
                        counter++;

                        // now integrate the changes
                        if(message_length == 1) continue; // nothing to do

                        for( unsigned long i = 0; i < message_length-1; i+=2) {
                                NodeID global_id = message[i];
                                NodeID label     = message[i+1];

                                if(G.getNodeLabel(G.getLocalID(global_id)) != label) {
                                        std::cout <<  "labels not ok"  << std::endl;
                                        exit(0);
                                }
                        }
                }
        }

        MPI::COMM_WORLD.Barrier();  
}


void distributed_partitioner::check( PartitionConfig & config, parallel_graph_access & G) {
        PEID m_rank = MPI::COMM_WORLD.Get_rank();
        PEID m_size = MPI::COMM_WORLD.Get_size();
        std::vector< std::vector< NodeID > > send_buffers; // buffers to send messages
        send_buffers.resize(m_size);
        std::vector<bool> m_PE_packed;
        m_PE_packed.resize(m_size); 
        for( unsigned peID = 0; peID < m_PE_packed.size(); peID++) {
                m_PE_packed[ peID ]           = false;
        }

        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                PEID peID = G.getTargetPE(target);
                                if( !m_PE_packed[peID] ) { // make sure a node is sent at most once
                                        send_buffers[peID].push_back(G.getGlobalID(node));
                                        send_buffers[peID].push_back(G.getSecondPartitionIndex(node));
                                        m_PE_packed[peID] = true;
                                }
                        }
                } endfor
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                m_PE_packed[G.getTargetPE(target)] = false;
                        }
                } endfor
        } endfor

        //send all neighbors their packages using Isends
        //a neighbor that does not receive something gets a specific token
        for( PEID peID = 0; peID < send_buffers.size(); peID++) {
                if( G.is_adjacent_PE(peID) ) {
                        //now we have to send a message
                        if( send_buffers[peID].size() == 0 ){
                                // length 1 encode no message
                                send_buffers[peID].push_back(0);
                        }

                        //std::cout << m_send_buffers[peID].size()  << std::endl;
                        MPI::COMM_WORLD.Isend( &send_buffers[peID][0], 
                                                send_buffers[peID].size(), MPI_UNSIGNED_LONG, peID, peID+17*m_size);
                }
        }

        //receive incomming
        PEID counter = 0;
        while( counter < G.getNumberOfAdjacentPEs()) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                unsigned int tag = m_rank+17*m_size;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,tag,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> message; message.resize(message_length);

                        MPI::COMM_WORLD.Recv( &message[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), tag); 
                        counter++;

                        // now integrate the changes
                        if(message_length == 1) continue; // nothing to do

                        for( unsigned long i = 0; i < message_length-1; i+=2) {
                                NodeID global_id = message[i];
                                NodeID label     = message[i+1];

                                if(G.getSecondPartitionIndex(G.getLocalID(global_id)) != label) {
                                        std::cout <<  "second partition index weird"  << std::endl;
                                        exit(0);
                                }
                        }
                }
        }

        MPI::COMM_WORLD.Barrier();  
}

