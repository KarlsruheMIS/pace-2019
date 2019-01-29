/******************************************************************************
 * yahoo_to_metis.cpp 
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


#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <mpi.h>
#include <argtable2.h>
#include "partition_config.h"
#include "parse_parameters.h"
#include "data_structure/hashed_graph.h"
#include "data_structure/parallel_graph_access.h"
#include "io/parallel_graph_io.h"

using namespace std;

int main(int argn, char **argv)
{
       
        MPI::Init();
        PartitionConfig partition_config;
        std::string graph_filename;

        int ret_code = parse_parameters(argn, argv, 
                                        partition_config, 
                                        graph_filename); 

        if(ret_code) {
                MPI::Finalize();
                return 0;
        }

 
        std::ifstream in(graph_filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << graph_filename << std::endl;
                return 1;
        }

        std::string line;

        std::unordered_map< NodeID, std::unordered_map< NodeID, int> > source_targets;
                        
        std::cout <<  "starting io"  << std::endl;
        EdgeID edge_counter = 0;
        EdgeID selfloops = 0;

        NodeID source;
        NodeID target;
        while( !in.eof() ) {
                std::getline(in, line);
                std::stringstream ss(line);

                ss >> source;
                long dummy;
                ss >> dummy;

                long target;

                ss >> target;
                while( ss >> target ) {
                        if( source == target ) {
                                std::getline(in, line);
                                selfloops++;
                                continue;
                        }

                        if( source_targets[source].find(target) == source_targets[source].end() ) {
                                source_targets[source][target] = 0;
                        }
                        if( source_targets[target].find(source) == source_targets[target].end() ) {
                                source_targets[target][source] = 0;
                        }

                        source_targets[source][target] += 1;
                        source_targets[target][source] += 1;
                }
        }

        std::cout <<  "selfloops " <<  selfloops  << std::endl;
        std::cout <<  "io done"  << std::endl;

        NodeID distinct_nodes = source_targets.size();
        std::unordered_map< NodeID, NodeID > map_orignal_id_to_consequtive;
        //std::unordered_map< NodeID, std::unordered_map< NodeID, bool > >::iterator it;
        NodeID counter = 0;
        for( auto it = source_targets.begin(); it != source_targets.end(); it++) {
                if( map_orignal_id_to_consequtive.find(it->first) ==  map_orignal_id_to_consequtive.end()) {
                        map_orignal_id_to_consequtive[it->first] = counter++;
                } 
                edge_counter += it->second.size();

        }
        std::cout <<  "starting construction"  << std::endl;

        complete_graph_access G;
        G.start_construction( distinct_nodes, edge_counter, distinct_nodes, edge_counter);
        G.set_range(0, distinct_nodes);

        EdgeID my_count = 0;
        for( auto it = source_targets.begin(); it != source_targets.end(); it++) {
                NodeID node = G.new_node();

                for( auto edge_it = source_targets[it->first].begin(); 
                     source_targets[it->first].end() != edge_it; 
                     edge_it++) {
                        G.new_edge(node, map_orignal_id_to_consequtive[edge_it->first]);
                        my_count += edge_it->second;
                }
        }
        G.finish_construction();
        std::cout <<  "my_count " <<  my_count << std::endl;
        std::cout <<  "my_count/2+selfloops " <<  (my_count/2+selfloops) << std::endl;

        std::string outputfilename("converted.graph");
        parallel_graph_io::writeGraphSequentially(G, outputfilename);


        return 0;
}
