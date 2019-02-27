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

#ifndef _BI_DOUBLE_GRAPH_H_
#define _BI_DOUBLE_GRAPH_H_

#include <vector>
#include <set>
#include <cstddef>

class BiDoubleGraph
{
public:
    BiDoubleGraph(std::vector<std::vector<int>> adjacencyList);
    ~BiDoubleGraph();

    std::vector<int> const &Neighbors(int const vertex) const;

    bool InLeftSide(int const vertex) const;

    bool ComputeResidualPath(std::vector<int> const &vMatching, std::vector<int> &vPath);

    void ComputeMaximumMatching(std::vector<int> &vMatching);

    bool ComputeResidualPath(std::vector<int> const &vMatching, std::vector<int> &vPath, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);

    void ComputeMaximumMatching(std::vector<int> &vMatching, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);

    size_t Size() const { return m_AdjacencyList.size(); }

private:
    void PushOnStack(int const vertex);
    int  PopOffStack();
    bool IsEvaluated(int const vertex) const;

    std::vector<std::vector<int>> m_AdjacencyList;
    std::vector<int> m_Stack;
    std::vector<int> m_Evaluated;
    int m_iCurrentRound;
};

#endif // _BI_DOUBLE_GRAPH_H_
