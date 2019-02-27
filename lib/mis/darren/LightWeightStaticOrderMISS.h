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

#ifndef LIGHTWEIGHT_STATIC_ORDER_MISS_H
#define LIGHTWEIGHT_STATIC_ORDER_MISS_H

#include "LightWeightMISQ.h"

#include <vector>
#include <list>

class LightWeightStaticOrderMISS : public LightWeightMISQ
{
public:
    LightWeightStaticOrderMISS(std::vector<std::vector<char>> const &vAdjacencyMatrix);

    virtual void InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors);

    virtual void GetNewOrder(std::vector<int> &vNewVertexOrder, std::vector<int> &vVertexOrder, std::vector<int> const &P, int const chosenVertex);

    virtual void ProcessOrderAfterRecursion(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors, int const chosenVertex);
};

#endif //LIGHTWEIGHT_STATIC_ORDER_MISS_H
