/******************************************************************************
 * dummy_operations.cpp 
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
#include <vector>
#include "dummy_operations.h"

dummy_operations::dummy_operations() {
                
}

dummy_operations::~dummy_operations() {
                        
}

void dummy_operations::run_collective_dummy_operations() {
        // Run Broadcast
        {
                int x = MPI::COMM_WORLD.Get_rank();
                MPI::COMM_WORLD.Bcast(&x, 1, MPI::INTEGER, 0);
        }
        // Run Allgather.
        {
                int x = MPI::COMM_WORLD.Get_rank();
                std::vector<int> rcv(MPI::COMM_WORLD.Get_size());
                MPI::COMM_WORLD.Allgather(&x, 1, MPI::INTEGER, &rcv[0], 1, MPI::INTEGER);
        }

        // Run Allreduce.
        {
                int x  = MPI::COMM_WORLD.Get_rank();
                long y = 0;
                MPI::COMM_WORLD.Allreduce(&x, &y, 1, MPI_UNSIGNED_LONG, MPI_SUM);
        }

        // Dummy Prefix Sum
        {
                int x  = 1;
                int y  = 0;

                MPI::COMM_WORLD.Scan(&x, &y, 1, MPI_UNSIGNED_LONG, MPI_SUM); 
        }

        // Run Alltoallv.
        {
                std::vector<int> snd(MPI::COMM_WORLD.Get_size());
                std::vector<int> rcv(MPI::COMM_WORLD.Get_size());
                std::vector<int> scounts(MPI::COMM_WORLD.Get_size(), 1);
                std::vector<int> rcounts(MPI::COMM_WORLD.Get_size(), 1);
                std::vector<int> sdispls(MPI::COMM_WORLD.Get_size());
                std::vector<int> rdispls(MPI::COMM_WORLD.Get_size());
                for (int i = 0, iend = sdispls.size(); i < iend; ++i) {
                        sdispls[i] = rdispls[i] = i;
                }
                MPI::COMM_WORLD.Alltoallv(&snd[0], &scounts[0], &sdispls[0], MPI::INTEGER,
                                &rcv[0], &rcounts[0], &rdispls[0], MPI::INTEGER);
        }
        

}
