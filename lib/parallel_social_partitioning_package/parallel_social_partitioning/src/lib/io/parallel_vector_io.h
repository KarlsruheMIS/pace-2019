/******************************************************************************
 * parallel_vector_io.h 
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

#ifndef PARALLEL_VECTOR_IO_BZVNZ570A
#define PARALLEL_VECTOR_IO_BZVNZ570A

#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "parallel_graph_io.h"

class parallel_vector_io {
public:
        parallel_vector_io();
        virtual ~parallel_vector_io();

        template<typename vectortype> 
        void readVectorSequentially(std::vector<vectortype> & vec, std::string filename);

        template<typename vectortype> 
        void writeVectorSequentially(std::vector<vectortype> & vec, std::string filename);

        template<typename vectortype> 
        void writePartitionSimpleParallel(parallel_graph_access & G, std::string filename);

};

template<typename vectortype> 
void parallel_vector_io::writeVectorSequentially(std::vector<vectortype> & vec, std::string filename) {
        std::ofstream f(filename.c_str());
        for( unsigned long i = 0; i < vec.size(); ++i) {
                f << vec[i] <<  std::endl;
        }

        f.close();
}

template<typename vectortype> 
void parallel_vector_io::readVectorSequentially(std::vector<vectortype> & vec, std::string filename) {

        std::string line;

        // open file for reading
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening vectorfile" << filename << std::endl;
                return;
        }

        unsigned long pos = 0;
        std::getline(in, line);
        while( !in.eof() ) {
                if (line[0] == '%') { //Comment
                        continue;
                }

                vectortype value = (vectortype) atof(line.c_str());
                vec[pos++] = value;
                std::getline(in, line);
        }

        in.close();
}

template<typename vectortype> 
void parallel_vector_io::writePartitionSimpleParallel(parallel_graph_access & G, 
                                                      std::string filename) {
        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();
        if( rank == ROOT ) {
                std::ofstream f(filename.c_str());

                forall_local_nodes(G, node) {
                        f <<  G.getNodeLabel(node) ;
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
                                f <<  G.getNodeLabel(node) ;
                                f <<  std::endl;
                        } endfor
                        f.close();
                }
        }
        MPI::COMM_WORLD.Barrier();
}




#endif /* end of include guard: PARALLEL_VECTOR_IO_BZVNZ570 */
