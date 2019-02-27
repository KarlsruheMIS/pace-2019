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

#ifndef LIGHTWEIGHT_MISQ_H
#define LIGHTWEIGHT_MISQ_H

#include "MaxSubgraphAlgorithm.h"
#include "IndependentSetColoringStrategy.h"

#include <vector>
#include <list>
#include <ctime>

class LightWeightMISQ : public MaxSubgraphAlgorithm
{
public:
    LightWeightMISQ(std::vector<std::vector<char>> const &vAdjacencyMatrix);
////    virtual ~LightWeightMISQ();

////    virtual long Run(std::list<std::list<int>> &cliques);

    virtual void Color(std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors);

    virtual void InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors);
    virtual void GetNewOrder(std::vector<int> &vNewVertexOrder, std::vector<int> &vVertexOrder, std::vector<int> const &P, int const chosenVertex);
    virtual void ProcessOrderAfterRecursion(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors, int const chosenVertex);
    virtual void ProcessOrderBeforeReturn(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors) {}

////    void SetInvert(bool const invert);

protected:
    std::vector<std::vector<char>> const &m_AdjacencyMatrix;
    IndependentSetColoringStrategy coloringStrategy;
////    bool m_bInvert;
};
#endif
