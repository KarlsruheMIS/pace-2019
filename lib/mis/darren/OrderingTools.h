/* 
    This program is free software: you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation, either version 3 of the License, or 
    (at your option) any later version. 
 
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
 
    You should have received a copy of the GNU General Public License 
    along with this program.  If not, see <http://www.gnu.org/licenses/> 
*/

#ifndef ORDERING_TOOLS_H
#define ORDERING_TOOLS_H

#include <vector>
#include <cstddef>
#include <string>

#include "Isolates4.h"
#include "SparseArraySet.h"
#include "ArraySet.h"

/*! \struct NeighborListArray

    \brief For a given ordering, this stores later neighbors and earlier neighbors
           for a given vertex in arrays.

    This version of the NeighborList structure is more cache efficient.
*/

class NeighborListArray
{
public:
    NeighborListArray()
    : vertex(-1)
    , earlier()
    , earlierDegree(-1)
    , later()
    , laterDegree(-1)
    , orderNumber(-1) {}

    int vertex; //!< the vertex that owns this neighbor list
    std::vector<int> earlier; //!< an array of neighbors that come before this vertex in an ordering
    int earlierDegree; //!< the number of neighbors in earlier
    std::vector<int> later; //!< an array of neighbors that come after this vertex in an ordering
    int laterDegree; //!< an array of neighbors that come after this vertex in an ordering
    int orderNumber; //!< the position of this verex in the ordering
};

typedef struct NeighborListArray NeighborListArray;



namespace OrderingTools
{

    void InitialOrderingMISQ(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> &vOrderedVertices, std::vector<int> &vColoring);

    void InitialOrderingMISR(std::vector<std::vector<int>> const &adjacencyArray, std::vector<int> &vOrderedVertices, std::vector<int> &vColoring, size_t &cliqueSize);

    template <typename IsolatesType>
    void InitialOrderingMISR(std::vector<std::vector<int>> const &adjacencyArray, IsolatesType const &isolates, std::vector<int> &vOrderedVertices, std::vector<int> &vColoring, size_t &cliqueSize);

    void InitialOrderingMISR(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> &vOrderedVertices, std::vector<int> &vColoring, size_t &cliqueSize);

};

#endif //ORDERING_TOOLS_H
