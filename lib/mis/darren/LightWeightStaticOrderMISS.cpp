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

#include "LightWeightStaticOrderMISS.h"
#include "OrderingTools.h"
#include "GraphTools.h"

#include <cmath>
#include <algorithm>
#include <iostream>

using namespace std;

LightWeightStaticOrderMISS::LightWeightStaticOrderMISS(vector<vector<char>> const &vAdjacencyMatrix)
: LightWeightMISQ(vAdjacencyMatrix)
{
    SetName("static-order-miss");
}

void LightWeightStaticOrderMISS::InitializeOrder(std::vector<int> &P, std::vector<int> &vVertexOrder, std::vector<int> &vColors)
{
    OrderingTools::InitialOrderingMISR(m_AdjacencyMatrix, P, vColors, m_uMaximumCliqueSize);
    vVertexOrder = P; //std::move(GraphTools::OrderVerticesByDegree(m_AdjacencyMatrix, true /* non-decreasing */)); //// = P; //?
}

void LightWeightStaticOrderMISS::GetNewOrder(vector<int> &vNewVertexOrder, vector<int> &vVertexOrder, vector<int> const &P, int const chosenVertex)
{
////    cout << "Old order: ";
////    for (int const vertex : vVertexOrder) {
////        cout << vertex << " ";
////    }
////    cout << endl;
    vNewVertexOrder.resize(vVertexOrder.size());
    {
        size_t uNewIndex(0);
        for (int const candidate : vVertexOrder) {
////            cout << depth << ": accessing " << chosenVertex << "," << candidate << " in " << m_AdjacencyMatrix.size() << "," << m_AdjacencyMatrix[chosenVertex].size() << endl << flush;
////            cout << depth << ": put in index " << uNewIndex << "/" << vNewVertexOrder.size() << endl << flush;
////            cout << depth << ": size of stackP : " << stackP.size() << endl;
////            cout << depth << ": size of stackOrder: " << stackOrder.size() << endl;
            if (chosenVertex == candidate) continue;
            if (!m_AdjacencyMatrix[chosenVertex][candidate]) vNewVertexOrder[uNewIndex++] = candidate;
        }
        vNewVertexOrder.resize(uNewIndex);
    }

    R.push_back(chosenVertex);

////    cout << "New order: ";
////    for (int const vertex : vNewVertexOrder) {
////        cout << vertex << " ";
////    }
////    cout << endl;
}

void LightWeightStaticOrderMISS::ProcessOrderAfterRecursion(std::vector<int> &vVertexOrder, std::vector<int> &P, std::vector<int> &vColors, int const chosenVertex)
{
////    cout << "LightWeightStaticOrderMISS::ProcessOrderAfterRecursion" << endl;
    if (chosenVertex == -1) return;
////    cout << "    order: ";
////    for (int const vertex : vVertexOrder) {
////        cout << vertex << " ";
////    }
////    cout << endl;
////    cout << "# vertices=" << vVertexOrder.size() << endl << flush;
    ////        vVertexOrder.erase(find(vVertexOrder.begin(), vVertexOrder.end(), chosenVertex));
    // try searching from end, might be faster in general...
////    if (find(vVertexOrder.begin(), vVertexOrder.end(), chosenVertex) != vVertexOrder.end()) {
////        cout << "vertex " << chosenVertex << " is in ordering..." << endl << flush;
////    } else {
////        cout << "vertex " << chosenVertex << " is not in ordering..." << endl << flush;
////    }
    size_t indexAfterVertex(0);
    for (indexAfterVertex = vVertexOrder.size(); indexAfterVertex >= 1; indexAfterVertex--) {
        if (vVertexOrder[indexAfterVertex-1] == chosenVertex) {
            break;
        }
    }

    for (; indexAfterVertex < vVertexOrder.size(); indexAfterVertex++) {
        vVertexOrder[indexAfterVertex-1] = vVertexOrder[indexAfterVertex];
    }

    vVertexOrder.pop_back();
    R.pop_back();
}
