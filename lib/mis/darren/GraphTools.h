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

#ifndef GRAPH_TOOLS_H
#define GRAPH_TOOLS_H

#include "SparseArraySet.h"
#include "ArraySet.h"

#include <vector>
#include <set>
#include <map>

namespace GraphTools
{
    void ComputeInducedSubgraph(std::vector<std::vector<int>> const &adjacencyList, std::set<int> const &vertices, std::vector<std::vector<int>> &subraph, std::map<int,int> &remapping);
    template<typename IsolatesType>
    void ComputeInducedSubgraphIsolates(IsolatesType const &isolates, std::set<int> const &vertices, std::vector<std::vector<int>> &subraph, std::map<int,int> &remapping);
    std::vector<int> OrderVerticesByDegree(std::vector<std::vector<int>> const &adjacencyList, bool const ascending);
    std::vector<int> OrderVerticesByDegree(ArraySet const &inGraph, std::vector<SparseArraySet> const &neighborSets, bool const ascending);
    std::vector<int> OrderVerticesByDegree(ArraySet const &inGraph, std::vector<ArraySet> const &neighborSets, bool const ascending);
    std::vector<int> OrderVerticesByDegree(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> const &vDegree, bool const ascending);
    std::vector<int> OrderVerticesByDegree(std::vector<std::vector<char>> const &adjacencyMatrix, bool const ascending);
////    void RemoveVertices(vector<vector<int>> &adjacencyList, vector<int> const &vVertices);

    std::vector<std::vector<int>> ComputeBiDoubleGraph(std::vector<std::vector<int>> const &adjacencyArray);

    void PrintGraphInEdgesFormat(std::vector<std::vector<int>> const &adjacencyArray);
    void PrintGraphInSNAPFormat(std::vector<std::vector<int>> const &adjacencyArray);

    template<typename IsolatesType>
    void ComputeConnectedComponents(IsolatesType const &isolates, std::vector<std::vector<int>> &vComponents, size_t const uNumVertices);

    void ComputeConnectedComponents(std::vector<std::vector<int>> const &adjacencyList, std::vector<std::vector<int>> &vComponents);
};

#endif //GRAPH_TOOLS_H
