/******************************************************************************
 * parallel_projection.cpp 
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

#include "parallel_projection.h"

parallel_projection::parallel_projection() {
                
}

parallel_projection::~parallel_projection() {
                
}

//TODO: do implement as an allToallv operation
//issue recv before send
void parallel_projection::parallel_project( parallel_graph_access & finer, parallel_graph_access & coarser ) {
        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();

        NodeID divisor = ceil(coarser.number_of_global_nodes() / (double)size);

        m_messages.resize(size);

        std::unordered_map< NodeID, std::vector< NodeID > > cnode_to_nodes;
        forall_local_nodes(finer, node) {
                NodeID cnode = finer.getCNode(node);
                //std::cout <<  "cnode " <<  cnode  << std::endl;
                if( coarser.is_local_node_from_global_id(cnode) ) {
                        NodeID new_label = coarser.getNodeLabel(coarser.getLocalID(cnode));
                        finer.setNodeLabel(node, new_label);
                } else {
                        //we have to request it from another PE
                        PEID peID = cnode / divisor; // cnode is 

                        if( cnode_to_nodes.find( cnode ) == cnode_to_nodes.end()) {
                                m_messages[peID].push_back(cnode); // we are requesting the label of this node 
                        }

                        cnode_to_nodes[cnode].push_back(node);
                }
        } endfor

        for( PEID peID = 0; peID < size; peID++) {
                if( peID != rank ) {
                        if( m_messages[peID].size() == 0 ){
                                m_messages[peID].push_back(std::numeric_limits<NodeID>::max());
                        }

                        MPI::COMM_WORLD.Isend( &m_messages[peID][0], 
                                        m_messages[peID].size(), 
                                        MPI_UNSIGNED_LONG, 
                                        peID, peID+size);
                }
        }

        std::vector< std::vector< NodeID > > out_messages;
        out_messages.resize(size);

        PEID counter = 0;
        while( counter < size - 1) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                unsigned long tag = rank+size;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,tag,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> incmessage; incmessage.resize(message_length);

                        MPI::COMM_WORLD.Recv( &incmessage[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), tag); 
                        counter++;

                        PEID peID = st.Get_source();
                        // now integrate the changes
                        if( incmessage[0] == std::numeric_limits< NodeID >::max()) {
                                out_messages[peID].push_back(std::numeric_limits< NodeID >::max());
                                MPI::COMM_WORLD.Isend( &out_messages[peID][0], 
                                                out_messages[peID].size(), 
                                                MPI_UNSIGNED_LONG, 
                                                peID, peID+2*size);

                                continue; // nothing to do
                        }


                        for( unsigned long i = 0; i < message_length; i++) {
                                NodeID cnode = coarser.getLocalID(incmessage[i]);
                                //out_messages[peID].push_back(incmessage[i]);
                                out_messages[peID].push_back(coarser.getNodeLabel(cnode));
                        }

                        MPI::COMM_WORLD.Isend( &out_messages[peID][0], 
                                                out_messages[peID].size(), 
                                                MPI_UNSIGNED_LONG, 
                                                peID, peID+2*size);

                }
        }

        counter = 0;
        while( counter < size - 1) {
                // wait for incomming message of an adjacent processor
                MPI::Status st;
                unsigned long tag = rank+2*size;
                while(MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE,tag,st)) {
                        unsigned long message_length = st.Get_count(MPI_UNSIGNED_LONG);
                        std::vector<NodeID> incmessage; incmessage.resize(message_length);

                        MPI::COMM_WORLD.Recv( &incmessage[0], message_length, MPI_UNSIGNED_LONG, st.Get_source(), tag); 
                        counter++;

                        // now integrate the changes
                        if( incmessage[0] == std::numeric_limits< NodeID >::max()) {
                                continue; // nothing to do
                        }

                        PEID peID = st.Get_source();
                        for( unsigned long i = 0; i < incmessage.size(); i++) {
                                std::vector< NodeID > & proj = cnode_to_nodes[m_messages[peID][i]];
                                NodeID label = incmessage[i];

                                for( unsigned long j = 0; j < proj.size(); j++) {
                                        finer.setNodeLabel(proj[j], label);
                                }
                        }
                }
        }

        finer.update_ghost_node_data_global(); // blocking
}

//initial assignment after initial partitioning
void parallel_projection::initial_assignment( parallel_graph_access & G, complete_graph_access & Q) {
        forall_local_nodes(G, node) {
                G.setNodeLabel(node, Q.getNodeLabel(G.getGlobalID(node)));
                if( G.is_interface_node(node) ) {
                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);
                                if( !G.is_local_node( target ) ) {
                                        G.setNodeLabel(target, Q.getNodeLabel(G.getGlobalID(target)));
                                }
                        } endfor
                }
        } endfor
}
