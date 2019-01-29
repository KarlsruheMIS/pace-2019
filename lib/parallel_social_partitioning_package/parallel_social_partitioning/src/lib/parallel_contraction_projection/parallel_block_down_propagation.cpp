/******************************************************************************
 * parallel_block_down_propagation.cpp 
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

#include "parallel_block_down_propagation.h"

parallel_block_down_propagation::parallel_block_down_propagation() {
                
}

parallel_block_down_propagation::~parallel_block_down_propagation() {
                
}

void parallel_block_down_propagation::propagate_block_down( PartitionConfig & config, 
                                                            parallel_graph_access & G, 
                                                            parallel_graph_access & Q) {


        std::unordered_map< NodeID, NodeID > coarse_block_ids; 

        forall_local_nodes(G, node) {
                NodeID cur_cnode = G.getCNode( node );
                coarse_block_ids[cur_cnode] = G.getSecondPartitionIndex( node );
        } endfor

        PEID size               = MPI::COMM_WORLD.Get_size();
        PEID rank               = MPI::COMM_WORLD.Get_rank();
        NodeID divisor          = ceil( Q.number_of_global_nodes()/(double)size);

        m_messages.resize(size);

        //now distribute the block idw
        //pack messages
        for( auto it = coarse_block_ids.begin(); it != coarse_block_ids.end(); it++) {
                NodeID node       = it->first;
                NodeID block      = it->second;
                PEID peID         = node / divisor;

                m_messages[ peID ].push_back( node );
                m_messages[ peID ].push_back( block );
        }

        for( PEID peID = 0; peID < size; peID++) {
                if( peID != rank ) {
                        if( m_messages[peID].size() == 0 ){
                                m_messages[peID].push_back(std::numeric_limits<NodeID>::max());
                        }

                        MPI::COMM_WORLD.Isend( &m_messages[peID][0], 
                                        m_messages[peID].size(), 
                                        MPI_UNSIGNED_LONG, 
                                        peID, peID+10*size);
                }
        }

        if( m_messages[ rank ].size() != 0 ) {
                for( unsigned long i = 0; i < m_messages[rank].size()-1; i+=2) {
                        NodeID globalID   = m_messages[rank][i];
                        NodeID node       = Q.getLocalID(globalID);
                        NodeWeight block  = m_messages[rank][i+1];
                        Q.setSecondPartitionIndex(node , block);
                }
        }

        PEID counter = 0;
        while( counter < size - 1) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,rank+10*size,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> incmessage; incmessage.resize(message_length);

                        MPI::COMM_WORLD.Recv( &incmessage[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), rank+10*size); 
                        counter++;

                        // now integrate the changes
                        if( incmessage[0] == std::numeric_limits< NodeID >::max()) continue; // nothing to do

                        for( unsigned long i = 0; i < incmessage.size()-1; i+=2) {
                                NodeID globalID   = incmessage[i];
                                NodeWeight block  = incmessage[i+1];
                                NodeID node       = Q.getLocalID(globalID);
                                Q.setSecondPartitionIndex( node , block);
                        }
                }
        }

        update_ghost_nodes_blocks( Q );
}

void parallel_block_down_propagation::update_ghost_nodes_blocks( parallel_graph_access & G ) {
        PEID size = MPI::COMM_WORLD.Get_size();
        PEID rank = MPI::COMM_WORLD.Get_rank();

        m_send_buffers.resize(size);
        std::vector< bool > PE_packed(size, false);
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                PEID peID = G.getTargetPE(target);
                                if( !PE_packed[peID] ) { // make sure a node is sent at most once
                                        m_send_buffers[peID].push_back(G.getGlobalID(node));
                                        m_send_buffers[peID].push_back(G.getSecondPartitionIndex(node));
                                        PE_packed[peID] = true;
                                }
                        }
                } endfor
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( !G.is_local_node(target)  ) {
                                PE_packed[G.getTargetPE(target)] = false;
                        }
                } endfor
        } endfor

        //send all neighbors their packages using Isends
        //a neighbor that does not receive something gets a specific token
        for( PEID peID = 0; peID < m_send_buffers.size(); peID++) {
                if( G.is_adjacent_PE(peID) ) {
                        //now we have to send a message
                        if( m_send_buffers[peID].size() == 0 ){
                                // length 1 encode no message
                                m_send_buffers[peID].push_back(0);
                        }

                        MPI::COMM_WORLD.Isend( &m_send_buffers[peID][0], 
                                                m_send_buffers[peID].size(), MPI_UNSIGNED_LONG, peID, peID+11*size);
                }
        }

        //receive incomming
        PEID counter = 0;
        while( counter < G.getNumberOfAdjacentPEs()) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,rank+11*size,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> message; message.resize(message_length);

                        MPI::COMM_WORLD.Recv( &message[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), rank+11*size); 
                        counter++;

                        // now integrate the changes
                        if(message_length == 1) continue; // nothing to do

                        for( unsigned long i = 0; i < message_length-1; i+=2) {
                                NodeID global_id   = message[i];
                                NodeWeight  block  = message[i+1];

                                G.setSecondPartitionIndex( G.getLocalID(global_id), block );
                        }
                }
        }

}
