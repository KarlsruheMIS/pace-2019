#include <stdio.h>
#include <iostream>
#include "io/parallel_graph_io.h"

using namespace std;

const long fileTypeVersionNumber = 2;
const long header_count    = 3;

int main(int argn, char **argv)
{

        MPI::Init(argn, argv);    /* starts MPI */

        PEID rank = MPI::COMM_WORLD.Get_rank();
        PEID size = MPI::COMM_WORLD.Get_size();
        if(argn != 2) {
                if( rank == ROOT ) {
                        std::cout <<  "usage: " ;
                        std::cout <<  "readdgf dfg_file"  << std::endl;
                }
                MPI::Finalize();
                return 0;
        }


        if( rank == ROOT ) {
                std::cout <<  "program converts a METIS graph file into a DGF (distributed graph format) file. "  << std::endl;
        }
        string filename(argv[1]);

        parallel_graph_access G;
        parallel_graph_io pgio;
        pgio.readGraphWeightedDGF(G, filename, rank, size);

        
        //forall_local_nodes(G, node) {
                //forall_out_edges(G, e, node) {
                        //NodeID target = G.getEdgeTarget(e);
                        //std::cout <<  target  << std::endl;
                //} endfor
        //} endfor
        

        string output_filename("dummy");
        parallel_graph_io::writeGraphParallelSimple(G, output_filename);

        MPI::Finalize();
        return 0;
}

