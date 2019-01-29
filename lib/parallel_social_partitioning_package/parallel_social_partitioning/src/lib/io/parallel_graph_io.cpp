/******************************************************************************
 * parallel_graph_io.cpp
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


#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <mpi.h>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <vector>

#include "parallel_graph_io.h"

const long fileTypeVersionNumber = 2;
const long header_count    = 3;

bool hasEnding (std::string const &string, std::string const &ending)
{
    if (string.length() >= ending.length()) {
        return (0 == string.compare (string.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

inline bool file_exists(const std::string& name) {
        std::ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}
parallel_graph_io::parallel_graph_io() {
                
}

parallel_graph_io::~parallel_graph_io() {
                
}

int parallel_graph_io::readGraphWeighted(parallel_graph_access & G, 
                                         std::string filename, 
                                         PEID peID, PEID comm_size) {

        std::string metis_ending(".graph");
        std::string dgf_ending(".dgf");

        if( hasEnding(filename, metis_ending) ) {
                //std::stringstream ss; 
                //ss << filename << dgf_ending;
                //if(file_exists(ss.str())) {
                        //return readGraphWeightedDGF(G, ss.str(), peID, comm_size);
                //} else {
                        return readGraphWeightedMETISFast(G, filename, peID, comm_size);
                //}
        }

        if( hasEnding(filename, dgf_ending) ) {
                return readGraphWeightedDGF(G, filename, peID, comm_size);
        }

        //non of both is true -- try metis format
        return readGraphWeightedMETIS(G, filename, peID, comm_size);
}

int parallel_graph_io::readGraphWeightedMETISFast(parallel_graph_access & G, 
                                         std::string filename, 
                                         PEID peID, PEID comm_size) {
        std::string line;

        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << filename << std::endl;
                return 1;
        }

        NodeID nmbNodes;
        EdgeID nmbEdges;

        std::getline(in,line);
        //skip comments
        while( line[0] == '%' ) {
                std::getline(in, line);
        }

        int ew = 0;
        std::stringstream ss(line);
        ss >> nmbNodes;
        ss >> nmbEdges;
        ss >> ew;

        // pe p reads the lines p*ceil(n/size) to (p+1)floor(n/size) lines of that file
        unsigned long from           = peID     * ceil(nmbNodes / (double)comm_size);
        unsigned long to             = (peID+1) * ceil(nmbNodes / (double)comm_size) - 1;
        to = std::min(to, nmbNodes-1);

        unsigned long local_no_nodes = to - from + 1;
        std::cout <<  "peID " <<  peID <<  " from " <<  from <<  " to " <<  to  <<  " amount " <<  local_no_nodes << std::endl;

        std::vector< std::vector< NodeID > > local_edge_lists;
        local_edge_lists.resize(local_no_nodes);
        

        //std::getline(in, line);
        unsigned long counter      = 0;
        NodeID node_counter = 0;
        EdgeID edge_counter = 0;


        char *oldstr, *newstr;
        while( std::getline(in, line) ) {
                if( counter > to ) {
                        break;
                }
                if (line[0] == '%') { // a comment in the file
                        continue;
                }

                if( counter >= from ) {
                        oldstr = &line[0];
                        newstr = 0;

                        for (;;) {
                                NodeID target;
                                target = (NodeID) strtol(oldstr, &newstr, 10);

                                if (target == 0) {
                                        break;
                                }

                                oldstr = newstr; 

                                local_edge_lists[node_counter].push_back(target);
                                edge_counter++;

                        }

                        node_counter++;
                }

                counter++;

                if( in.eof() ) {
                        break;
                }
        }

        MPI::COMM_WORLD.Barrier();
        G.start_construction(local_no_nodes, 2*edge_counter, nmbNodes, 2*nmbEdges);
        G.set_range(from, to);

        for (NodeID i = 0; i < local_no_nodes; ++i) {
                NodeID node = G.new_node();
                G.setNodeWeight(node, 1);
                G.setNodeLabel(node, from+node);
                G.setSecondPartitionIndex(node, 0);
        
                for( unsigned j = 0; j < local_edge_lists[i].size(); j++) {
                        NodeID target = local_edge_lists[i][j]-1; // -1 since there are no nodes with id 0 in the file
                        EdgeID e = G.new_edge(node, target);
                        G.setEdgeWeight(e, 1);
                }
        }

        G.finish_construction();
        MPI::COMM_WORLD.Barrier();
        return 0;


}
// we start with the simplest version of IO 
// where each process reads the graph sequentially
// TODO write weighted code and fully parallel io code
int parallel_graph_io::readGraphWeightedMETIS(parallel_graph_access & G, 
                                         std::string filename, 
                                         PEID peID, PEID comm_size) {
        std::string line;

        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << filename << std::endl;
                return 1;
        }

        NodeID nmbNodes;
        EdgeID nmbEdges;

        std::getline(in,line);
        //skip comments
        while( line[0] == '%' ) {
                std::getline(in, line);
        }

        int ew = 0;
        std::stringstream ss(line);
        ss >> nmbNodes;
        ss >> nmbEdges;
        ss >> ew;

        // pe p reads the lines p*ceil(n/size) to (p+1)floor(n/size) lines of that file
        unsigned long from           = peID     * ceil(nmbNodes / (double)comm_size);
        unsigned long to             = (peID+1) * ceil(nmbNodes / (double)comm_size) - 1;
        to = std::min(to, nmbNodes-1);

        unsigned long local_no_nodes = to - from + 1;
        std::cout <<  "peID " <<  peID <<  " from " <<  from <<  " to " <<  to  <<  " amount " <<  local_no_nodes << std::endl;

        std::vector< std::vector< NodeID > > local_edge_lists;
        local_edge_lists.resize(local_no_nodes);
        

        //std::getline(in, line);
        unsigned long counter      = 0;
        NodeID node_counter = 0;
        EdgeID edge_counter = 0;

        while( std::getline(in, line) ) {
                if( counter > to ) {
                        break;
                }
                if (line[0] == '%') { // a comment in the file
                        continue;
                }

                if( counter >= from ) {
                        std::stringstream ss(line);

                        NodeID target;
                        while( ss >> target ) {
                                local_edge_lists[node_counter].push_back(target);
                                edge_counter++;
                        }
                        node_counter++;
                }

                counter++;

                if( in.eof() ) {
                        break;
                }
        }

        MPI::COMM_WORLD.Barrier();
        G.start_construction(local_no_nodes, 2*edge_counter, nmbNodes, 2*nmbEdges);
        G.set_range(from, to);

        for (NodeID i = 0; i < local_no_nodes; ++i) {
                NodeID node = G.new_node();
                G.setNodeWeight(node, 1);
                G.setNodeLabel(node, from+node);
                G.setSecondPartitionIndex(node, 0);
        
                for( unsigned j = 0; j < local_edge_lists[i].size(); j++) {
                        NodeID target = local_edge_lists[i][j]-1; // -1 since there are no nodes with id 0 in the file
                        EdgeID e = G.new_edge(node, target);
                        G.setEdgeWeight(e, 1);
                }
        }

        G.finish_construction();
        return 0;


}
int parallel_graph_io::readGraphWeightedDGF(parallel_graph_access & G, 
                                         std::string filename, 
                                         PEID peID, PEID size) { 

        MPI::File f = MPI::File::Open(MPI::COMM_WORLD, filename.c_str(), MPI::MODE_RDONLY, MPI::INFO_NULL);

        long version;
        NodeID n;
        NodeID m;

        //// The master writes the header.
        if( peID == ROOT) std::cout <<  "Reading graph ..."  << std::endl;

        //write version number
        f.Read(&version, 1, MPI::LONG);

        //write number of nodes etc
        f.Read(&n, 1, MPI::LONG);
        f.Read(&m, 1, MPI::LONG);

        if(peID == ROOT) std::cout <<  "version: " <<  version <<  " n: "<<  n <<  " m: " <<  m  << std::endl;
        if( version != fileTypeVersionNumber ) {
                if(peID == ROOT) {
                        std::cout <<  "filetype version missmatch"  << std::endl;
                }
                MPI::Finalize();
                return 0;
        }

        unsigned long from           = peID * ceil(n / (double)size);
        unsigned long to             = (peID +1) * ceil(n / (double)size) - 1;
        to = std::min(to, n-1);

        unsigned long local_no_nodes = to - from + 1;
        std::cout <<  "peID " <<  peID <<  " from " <<  from <<  " to " <<  to  <<  " amount " <<  local_no_nodes << std::endl;

        // read the offsets
        MPI::Offset start_pos = (header_count + from)*static_cast<MPI::Offset>(sizeof(unsigned long));
        NodeID* vertex_offsets = new NodeID[local_no_nodes+1]; // we also need the next vertex offset
        f.Read_at(start_pos, vertex_offsets, local_no_nodes+1, MPI::LONG);

        MPI::Offset edge_start_pos = vertex_offsets[0];
        EdgeID num_reads           = vertex_offsets[local_no_nodes]-vertex_offsets[0];
        EdgeID num_edges_to_read   = num_reads/static_cast<MPI::Offset>(sizeof(unsigned long));

        EdgeID* edges = new EdgeID[num_edges_to_read]; // we also need the next vertex offset
        f.Read_at(edge_start_pos, edges, num_edges_to_read, MPI::LONG);


        MPI::COMM_WORLD.Barrier();
        f.Close();

        G.start_construction(local_no_nodes, num_edges_to_read, n, m);
        G.set_range(from, to);

        long pos = 0;

        for (NodeID i = 0; i < local_no_nodes; ++i) {
                NodeID node = G.new_node();
                G.setNodeWeight(node, 1);
                G.setNodeLabel(node, from+node);
                G.setSecondPartitionIndex(node, 0);

                NodeID degree =  (vertex_offsets[i+1] - vertex_offsets[i]) / sizeof(unsigned long);
                for( unsigned long j = 0; j < degree; j++, pos++) {
                        NodeID target = edges[pos]; 
                        std::cout <<  target  << std::endl;
                        EdgeID e = G.new_edge(node, target);
                        G.setEdgeWeight(e, 1);
                }
        }

        G.finish_construction();
        MPI::COMM_WORLD.Barrier();

        delete[] vertex_offsets;
        delete[] edges;

        return 0;
}

int parallel_graph_io::writeGraphParallelSimple(parallel_graph_access & G, 
                                                std::string filename) {
        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();
        if( rank == ROOT ) {
                std::ofstream f(filename.c_str());
                f << G.number_of_global_nodes() <<  " " <<  G.number_of_global_edges()/2 <<   std::endl;

                forall_local_nodes(G, node) {
                        forall_out_edges(G, e, node) {
                                f << (G.getGlobalID(G.getEdgeTarget(e))+1) << " " ;
                        } endfor 
                        f <<  std::endl;
                } endfor

                f.close();
        } 

        for( unsigned i = 1; i < size; i++) {
                MPI::COMM_WORLD.Barrier();
                if( rank == i ) {
                        std::ofstream f;
                        f.open(filename, std::ofstream::out | std::ofstream::app);
                        forall_local_nodes(G, node) {
                                forall_out_edges(G, e, node) {
                                        f <<  (G.getGlobalID(G.getEdgeTarget(e))+1) << " " ;
                                } endfor 
                                f <<  std::endl;
                        } endfor
                        f.close();
                }
        }
        MPI::COMM_WORLD.Barrier();

        return 0;
}

int parallel_graph_io::writeGraphWeightedParallelSimple(parallel_graph_access & G, 
                                                               std::string filename) {
        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();
        if( rank == ROOT ) {
                std::ofstream f(filename.c_str());
                f << G.number_of_global_nodes() <<  " " <<  G.number_of_global_edges()/2 << " 11" <<  std::endl;

                forall_local_nodes(G, node) {
                        f <<  G.getNodeWeight(node) ;
                        forall_out_edges(G, e, node) {
                                f << " " <<   (G.getGlobalID(G.getEdgeTarget(e))+1) <<  " " <<  G.getEdgeWeight(e) ;
                        } endfor 
                        f <<  std::endl;
                } endfor

                f.close();
        } 

        for( unsigned i = 1; i < size; i++) {
                MPI::COMM_WORLD.Barrier();
                if( rank == i ) {
                        std::ofstream f;
                        f.open(filename, std::ofstream::out | std::ofstream::app);
                        forall_local_nodes(G, node) {
                                f <<  G.getNodeWeight(node) ;
                                forall_out_edges(G, e, node) {
                                        f << " " <<   (G.getGlobalID(G.getEdgeTarget(e))+1) <<  " " <<  G.getEdgeWeight(e) ;
                                } endfor 
                                f <<  std::endl;
                        } endfor
                        f.close();
                }
        }
        MPI::COMM_WORLD.Barrier();

        return 0;
}

int parallel_graph_io::writeGraphWeightedSequentially(complete_graph_access & G, std::string filename) {
        std::ofstream f(filename.c_str());
        f << G.number_of_global_nodes() <<  " " <<  G.number_of_global_edges()/2 <<  " 11" <<  std::endl;

        forall_local_nodes(G, node) {
                f <<  G.getNodeWeight(node) ;
                forall_out_edges(G, e, node) {
                        f << " " <<   (G.getEdgeTarget(e)+1) <<  " " <<  G.getEdgeWeight(e) ;
                } endfor 
                f <<  std::endl;
        } endfor

        f.close();
        return 0;
}

int parallel_graph_io::writeGraphSequentially(complete_graph_access & G, std::ofstream & f) {
        f << G.number_of_global_nodes() <<  " " <<  G.number_of_global_edges()/2 <<   std::endl;

        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        f << " " <<   (G.getEdgeTarget(e)+1)  ;
                } endfor 
                f <<  std::endl;
        } endfor
        return 0;
}

int parallel_graph_io::writeGraphSequentially(complete_graph_access & G, std::string filename) {
        std::ofstream f(filename.c_str());
        writeGraphSequentially(G, f);
        f.close();
        return 0;

}

int parallel_graph_io::writeGraphSequentiallyDGF(complete_graph_access & G, std::string filename) {
        PEID size = MPI::COMM_WORLD.Get_rank();
        if( size > 1 ) {
                std::cout <<  "currently only one process supported."  << std::endl;
                return 0;
        }

        MPI::File f = MPI::File::Open(MPI::COMM_WORLD, filename.c_str(),
                        MPI::MODE_WRONLY | MPI::MODE_CREATE,
                        MPI::INFO_NULL);

        long number_of_longs = header_count + G.number_of_global_nodes()+1+G.number_of_global_edges();
        MPI::Offset fsize    = number_of_longs * static_cast<MPI::Offset>(sizeof(unsigned long));
        f.Set_size(fsize);

        std::cout <<  "Writing graph " << filename  << std::endl;

        printf("Writing graph with n = %ld, m = %ld, file size = %lld\n", G.number_of_global_nodes(), G.number_of_global_edges(), fsize);
        f.Seek(0, MPI_SEEK_SET);

        //write version number
        f.Write(&fileTypeVersionNumber, 1, MPI::LONG);

        //write number of nodes etc
        NodeID n = G.number_of_global_nodes();
        NodeID m = G.number_of_global_edges();

        f.Write(&n, 1, MPI::LONG);
        f.Write(&m, 1, MPI::LONG);

        NodeID * offset_array = new NodeID[n+1];
        long pos              = 0;
        NodeID offset         = (header_count + G.number_of_global_nodes() + 1) * static_cast<MPI::Offset>(sizeof(unsigned long));

        forall_local_nodes(G, node) {
                offset_array[pos++] = offset;
                offset += G.getNodeDegree(node) * static_cast<MPI::Offset>(sizeof(unsigned long)); 

        } endfor

        offset_array[pos] = offset;
        f.Write(offset_array, n+1, MPI::LONG);

        delete[] offset_array;

        NodeID * edge_array = new NodeID[m];
        pos = 0;
        // now write the edges 
        forall_local_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        edge_array[pos++] = target;
                } endfor
        } endfor
        f.Write(edge_array, m, MPI::LONG);
        MPI::COMM_WORLD.Barrier();
        f.Close();
        delete[] edge_array;

        MPI::COMM_WORLD.Barrier();

        return 0;
}
