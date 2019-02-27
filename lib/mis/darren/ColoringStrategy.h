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

#ifndef COLORING_STRATEGY_H
#define COLORING_STRATEGY_H

#include <vector>

class ColoringStrategy
{
public:
    ColoringStrategy() {}
    virtual void Color(std::vector<std::vector<int>>  const &adjacencyList, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) {};
    virtual void Color(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) {};

    virtual void Color(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) {};
    virtual int ColorWithoutReorder(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) { return 0; }
    virtual void Recolor(std::vector<std::vector<char>> const &adjacencyMatrix, std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) {};
    virtual void Recolor(std::vector<std::vector<int>> const &adjacencyArray, std::vector<int> const &vVertexOrder, std::vector<int> &vVerticesToReorder, std::vector<int> &vColors) {};
////    virtual void Recolor() = 0;
////    virtual void RemoveVertex(int const vertex) = 0;
////    virtual void PeekAtNextVertexAndColor(int &vertex, int &color) = 0;
};

#endif //COLORING_STRATEGY_H
