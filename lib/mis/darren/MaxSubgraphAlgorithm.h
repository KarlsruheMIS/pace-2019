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

#ifndef MAX_SUBGRAPH_ALGORITHM_H
#define MAX_SUBGRAPH_ALGORITHM_H

#include "Algorithm.h"
#include "IndependentSetColoringStrategy.h"

#include <vector>
#include <list>
#include <ctime>

////#define PREPRUNE
////#define REMOVE_ISOLATES_BEFORE_ONLY
////#define ALWAYS_REMOVE_ISOLATES_AFTER
////#define NO_ISOLATES_P_LEFT_10

class MaxSubgraphAlgorithm : public Algorithm
{
public:
    MaxSubgraphAlgorithm(std::string const &name);
    virtual ~MaxSubgraphAlgorithm();

    virtual long Run(std::list<std::list<int>> &cliques);
////    virtual long Run(vector<int> const &startingVertices, std::list<std::list<int>> &cliques);

    virtual void Color(std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) = 0;

    virtual void InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors) = 0;
    virtual void GetNewOrder(std::vector<int> &vNewVertexOrder, std::vector<int> &vVertexOrder, std::vector<int> const &P, int const chosenVertex) = 0;
    virtual void ProcessOrderAfterRecursion(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors, int const chosenVertex) = 0;

    virtual void ProcessOrderBeforeReturn(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors) = 0;

    virtual void RunRecursive(std::vector<int> &P, std::vector<int> &vVertexOrder, std::list<std::list<int>> &cliques, std::vector<int> &vColors);

    virtual void SetQuiet(bool const quiet) { m_bQuiet = quiet; }

    virtual void PrintState() const;

    virtual void SetNodeCount(size_t const count) { nodeCount = count; }
    virtual size_t GetNodeCount() { return nodeCount; }

    void SetR(std::vector<int> const &newR) { R = newR; }
    void SetMaximumCliqueSize(size_t const newCliqueSize) { m_uMaximumCliqueSize = newCliqueSize; }

    void SetOnlyVertex(int const vertex) { m_iOnlyVertex = vertex; }

    void SetTimeOutInSeconds(double const timeout) { m_TimeOut = timeout*CLOCKS_PER_SEC; }

    bool GetTimedOut() const { return m_bTimedOut; }

protected:
    size_t m_uMaximumCliqueSize;
    std::vector<int> R;
    std::vector<std::vector<int>> stackP;
    std::vector<std::vector<int>> stackColors;
    std::vector<std::vector<int>> stackOrder;
    size_t nodeCount;
    int depth;
    clock_t startTime;
    clock_t timeToLargestClique;
    bool    m_bQuiet;
    std::vector<bool> stackEvaluatedHalfVertices;
////    bool m_bInvert;
    int m_iOnlyVertex;
    clock_t m_TimeOut;
    clock_t m_StartTime;
    bool    m_bTimedOut;
};
#endif // MAX_SUBGRAPH_ALGORITHM_H
