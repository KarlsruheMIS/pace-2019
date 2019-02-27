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

#include "LightWeightMISR.h"
#include "OrderingTools.h"
#include "GraphTools.h"

#include <cmath>
#include <iostream>

using namespace std;

LightWeightMISR::LightWeightMISR(vector<vector<char>> const &vAdjacencyMatrix)
: LightWeightMISQ(vAdjacencyMatrix)
{
    SetName("mcr");
}

void LightWeightMISR::InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors)
{
    OrderingTools::InitialOrderingMISR(m_AdjacencyMatrix, P, vColors, m_uMaximumCliqueSize);
    vVertexOrder = P;
}
