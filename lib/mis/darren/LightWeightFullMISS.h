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

#ifndef LIGHTWEIGHT_FULL_MISS_H
#define LIGHTWEIGHT_FULL_MISS_H

#include "LightWeightStaticOrderMISS.h"

#include <vector>
#include <list>

class LightWeightFullMISS : public LightWeightStaticOrderMISS
{
public:
    LightWeightFullMISS(std::vector<std::vector<char>> const &vAdjacencyMatrix);

    virtual void Color(std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors);
};

#endif //LIGHTWEIGHT_FULL_MISS_H
