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

#include "Isolates4.h"
#include "ArraySet.h"
#include "SparseArraySet.h"

#include <vector>
#include <set>
#include <iostream>
#include <ctime>
#include <cassert>
#include <climits>

using namespace std;

#define NEW_ISOLATE_CHECKS
////#define DOMINATION_CHECKS
#define NON_PATH

template <typename NeighborSet>
Isolates4<NeighborSet>::Isolates4(vector<vector<int>> const &adjacencyArray)
 : m_AdjacencyArray(adjacencyArray)
 , neighbors(adjacencyArray.size())
 , inGraph(adjacencyArray.size())
 , isolates(adjacencyArray.size())
 , remaining(adjacencyArray.size())
 , vMarkedVertices(adjacencyArray.size(), false)
 , m_AlternativeVertices()
#ifdef TIMERS
 , timer(0)
 , removeTimer(0)
 , replaceTimer(0)
 , sortDuringNextTimer(0)
 , removeOneDuringNextTimer(0)
 , removeDuringNextTimer(0)
 , replaceDuringNextTimer(0)
 #endif // TIMERS
 , m_bConnectedComponentMode(false)
 , foldedVertexCount(0)
 , m_bAllowVertexFolds(true)
{
    for (size_t u=0; u < adjacencyArray.size(); ++u) {
        remaining.Insert(u);
        inGraph.Insert(u);
#ifdef SPARSE
        neighbors[u].Resize(m_AdjacencyArray[u].size());
#else
        neighbors[u].InitializeFromAdjacencyArray(m_AdjacencyArray, u);
#endif // SPARSE
        for (int const vertex : m_AdjacencyArray[u]) {
            neighbors[u].Insert(vertex);
        }
    }
}

template <typename NeighborSet>
Isolates4<NeighborSet>::~Isolates4()
{

#ifdef TIMERS
    cout << "Total time spent computing next vertex: " << (timer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    Sort      : " << (sortDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    RemoveOne : " << (removeOneDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    RemoveRest: " << (removeDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "    ReplaceAll: " << (replaceDuringNextTimer/(double)CLOCKS_PER_SEC) << endl;
   
    cout << "Total time spent applying reductions  : " << (removeTimer/(double)CLOCKS_PER_SEC) << endl;
    cout << "Total time spent undoing  reductions  : " << (replaceTimer/(double)CLOCKS_PER_SEC) << endl;
#endif // TIMERS
}

////void Isolates4::RemoveEdges(vector<pair<int,int>> const &vEdges)
////{
////    for (pair<int,int> const &edge : vEdges) {
////        neighbors[edge.first].Remove(edge.second);
////        neighbors[edge.second].Remove(edge.first);
////
////        m_AdjacencyArray[edge.first].pop_back();
////        m_AdjacencyArray[edge.second].pop_back();
////    }
////}

template <typename NeighborSet>
bool Isolates4<NeighborSet>::RemoveIsolatedClique(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<Reduction> &vReductions)
{
////    bool const debug(vertex==40653);
////    if (vertex == 31) {
////        cout << "Removing isolated clique with vertex " << vertex << endl << flush;
////        if (!inGraph.Contains(31)) cout << "Vertex 31 is not in the graph!" << endl << flush;
////    }

    size_t neighborCount(0);
    for (int const neighbor : neighbors[vertex]) {
        if (neighbors[neighbor].Size() < neighbors[vertex].Size()) {
            return false;
        }
    }

    bool superSet(true);

////    bool debug(vertex == 113);
////
////    if (debug) {
////        cout << "vertex" << vertex << " has " << neighbors[vertex].size() << " neighbors." << endl << flush;
////    }

#if 1
////    bool const debug(true); //vertex==12);
////    if (debug) {
////        cout << "vertex " << vertex << " has neighbors: ";
////    }
////    if (debug) {
////        for (int const neighbor : neighbors[vertex]) {
////            cout << neighbor << " " << flush;
////        }
////        cout << endl << flush;
////        cout << "(";
////
////        for (int const neighbor : m_AdjacencyArray[vertex]) {
////            cout << neighbor << " " << flush;
////        }
////        cout << ")" << endl << flush;
////    }
////
    for (int const neighbor : neighbors[vertex]) {
////        if (debug) {
////            cout << "Considering neighbor " <<  neighbor  << flush;
////        }
        neighborCount++;

        for (int const nNeighbor : neighbors[neighbor]) {
            vMarkedVertices[nNeighbor] = true;
        }
        vMarkedVertices[neighbor] = true;

        for (int const neighbor2 : neighbors[vertex]) {
////            if (debug && superSet && !vMarkedVertices[neighbor2]) {
////                cout << "(missing neighbor " << neighbor2 << ") ";
////            }
            superSet = superSet && vMarkedVertices[neighbor2];
        }

        //superSet = superSet && (neighborCount == neighbors[vertex].size()); // TODO/DS : put back?

        for (int const nNeighbor : neighbors[neighbor]) {
            vMarkedVertices[nNeighbor] = false;
        }
        vMarkedVertices[neighbor] = false;

        if (!superSet) {
////            if (debug) {
////                cout << "[x] " << endl;
////            }
            return false;
        }

////        if (debug) cout << endl << flush;

        ////            cout << "Iteration(" << neighborCount << ": Intersection=";
        ////            for (int const vertex : intersection) {
        ////                cout << vertex << " ";
        ////            }
        ////            cout << endl << flush;
    }
#else

    for (int const neighbor : neighbors[vertex]) {
////        cout << "Evaluating neighbor: " << neighbor << endl << flush;
        neighborCount++;

        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << "Marking nNeighbor: " << nNeighbor << endl << flush;
            vMarkedVertices[nNeighbor] = true;
        }
////        cout << "Marking neighbor: " << neighbor << endl << flush;
        vMarkedVertices[neighbor] = true;

        for (int const neighbor2 : neighbors[vertex]) {
////            cout << "Checking neighbor: " << neighbor2 << endl << flush;
            superSet = superSet && vMarkedVertices[neighbor2];
        }

        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << "Unmarking nNeighbor: " << nNeighbor << endl << flush;
            vMarkedVertices[nNeighbor] = false;
        }
////        cout << "Unmarking neighbor: " << neighbor << endl << flush;
        vMarkedVertices[neighbor] = false;

        if (!superSet) return false;

        ////            cout << "Iteration(" << neighborCount << ": Intersection=";
        ////            for (int const vertex : intersection) {
        ////                cout << vertex << " ";
        ////            }
        ////            cout << endl << flush;
    }
#endif

////    cout << "Done evaluating neighbors" << endl << flush;

    ////        cout << "Iteration(" << neighborCount << ": Intersection=";
    ////        for (int const vertex : intersection) {
    ////            cout << vertex << " ";
    ////        }
    ////        cout << endl << flush;

    if (superSet) {
////        if (debug) {
////            cout << "Removing Clique: [" << vertex;
////            for (int const neighbor : neighbors[vertex]) {
////                cout << " " << neighbor;
////            }
////            cout << "]" << endl;
////        }
////        for (int const neighbor : neighbors[vertex]) {
////            cout << neighbor << " ";
////        }
////        cout << endl;

        Reduction reduction(ISOLATED_VERTEX);
        reduction.SetVertex(vertex);

////        cout << "Removing isolated vertex " << vertex << " with neighbors ";
        for (int const neighbor : neighbors[vertex]) {
////            if (!inGraph.Contains(neighbor)) {
////                cout << "Trying to remove non-existant neighbor " << neighbor << " from " << vertex << " neighbor list!" << endl << flush;
////            }
////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(neighbor);
            remaining.Remove(neighbor);
            reduction.AddNeighbor(neighbor);
            reduction.AddRemovedEdge(vertex,   neighbor);
            reduction.AddRemovedEdge(neighbor, vertex);
////            cout << neighbor << " ";
        }
////        cout << endl << flush;

////        if (!inGraph.Contains(vertex)) {
////            cout << "Trying to remove non-existant vertex " << vertex << " from graph!" << endl << flush;
////        }

////        cout << __LINE__ << ": calling remove" << endl << flush;
        inGraph.Remove(vertex);
        isolates.Insert(vertex);
        vIsolateVertices.push_back(vertex);
////        if (vertex == 0) {
////            cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////        }
        vOtherRemovedVertices.insert(vOtherRemovedVertices.end(), neighbors[vertex].begin(), neighbors[vertex].end());

        for (int const neighbor : neighbors[vertex]) {
            ////                cout << "   Expunging neighbor " << neighbor << " with " << neighbors[neighbor].size() << " neighbors" << endl << flush;
            for (int const nNeighbor : neighbors[neighbor]) {
                if (inGraph.Contains(nNeighbor)) {
                    remaining.Insert(nNeighbor);
                }

                if (nNeighbor != vertex) {
                    neighbors[nNeighbor].Remove(neighbor);
                    reduction.AddRemovedEdge(nNeighbor, neighbor);
                    reduction.AddRemovedEdge(neighbor, nNeighbor);
                }
            }
            neighbors[neighbor].Clear();
        }
        neighbors[vertex].Clear();

////    if (vertex == 21952) {
////        cout << "vertex" << vertex << " is being removed." << endl << flush;
////    }

        vReductions.emplace_back(std::move(reduction));
        
        return true;
    }
    return false;
}

#ifdef NEW_ISOLATE_CHECKS
// TODO/DS: need to remember added edge, so we can remove it later.
// TODO/DS: not currently working, proceeding without it.

int numDominatedVertices(0);
template <typename NeighborSet>
bool Isolates4<NeighborSet>::RemoveDominatedVertex(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<Reduction> &vReductions)
{
    if (neighbors[vertex].Empty()) return false;
////    if (numDominatedVertices > 303) return false;

    size_t smallestNeighborhood(ULONG_MAX);
    int vertexWithSmallestNeighborhood(-1);
    vMarkedVertices[vertex] = true;
    for (int const neighbor : neighbors[vertex]) {
////        if (neighbor == vertex)
////            cout << "Something is very wrong... vertex " << vertex << " is neighbor of itself" << endl << flush;
        vMarkedVertices[neighbor] = true;
        if (neighbors[neighbor].Size() < smallestNeighborhood) {
            smallestNeighborhood = neighbors[neighbor].Size();
            vertexWithSmallestNeighborhood = neighbor;
        }
    }

    for (int const neighbor : neighbors[vertex]) {
        numDominatedVertices++;
        // does neighbor dominate vertex?
        int commonNeighborCount(0);
        for (int const nNeighbor : neighbors[neighbor]) {
            if (vMarkedVertices[nNeighbor]) commonNeighborCount++;
        }

        // has every neighbor of vertex, + vertex - neighbor
        if (commonNeighborCount == neighbors[vertex].Size()) {

            Reduction reduction(DOMINATED_VERTEX);
            reduction.SetVertex(neighbor);

            vMarkedVertices[vertex] = false;
            for (int const neighbor : neighbors[vertex]) {
                vMarkedVertices[neighbor] = false;
            }

            // remove vertex from graph
            inGraph.Remove(neighbor);
            remaining.Remove(neighbor);
            for (int const nNeighbor : neighbors[neighbor]) {
                remaining.Insert(nNeighbor);
                neighbors[nNeighbor].Remove(neighbor);
                reduction.AddRemovedEdge(nNeighbor, neighbor);
                reduction.AddRemovedEdge(neighbor, nNeighbor);
            }

            neighbors[neighbor].Clear();
            vOtherRemovedVertices.push_back(neighbor);
////            vMarkedVertices[nNeighbor] = false;

            vReductions.emplace_back(std::move(reduction));
////            cout << "Removing dominated vertex " << neighbor << endl;
            return true;
        }
    }

    vMarkedVertices[vertex] = false;
    for (int const neighbor : neighbors[vertex]) {
        vMarkedVertices[neighbor] = false;
    }

////    if (removed) {
////        neighbors[vertex].Clear();
////    }

    return false;
}

size_t numFoldedVertices(0);

template <typename NeighborSet>
bool Isolates4<NeighborSet>::FoldVertex(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<Reduction> &vReductions)
{
#ifdef NON_PATH
    if (neighbors[vertex].Size() != 2) return false;
    if (neighbors[neighbors[vertex][0]].Contains(neighbors[vertex][1])) return false; // neighbors can't be adjacent.

    foldedVertexCount++;

////    cout << "Folding vertex " << vertex << ":" << endl << flush;

    numFoldedVertices++;

////    if (numFoldedVertices > 100) return false;

    int const vertex1(neighbors[vertex][0]);
    int const vertex2(neighbors[vertex][1]);

    Reduction reduction(FOLDED_VERTEX);
    reduction.SetVertex(vertex);
    reduction.AddNeighbor(vertex1);
    reduction.AddNeighbor(vertex2);

////    bool const debug(numFoldedVertices == 83);
////    bool const debug(vertex == 1480);
////    if (debug) {
////        cout << "Folding: " << vertex << ", " << vertex1 << ", " << vertex2 << endl;
////        cout << "BEFORE:" << endl;
////        cout << vertex << ":";
////        for (int const neighbor : neighbors[vertex]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////
////        cout << vertex1 << ":";
////        for (int const neighbor : neighbors[vertex1]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////
////        cout << vertex2 << ":";
////        for (int const neighbor : neighbors[vertex2]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////    }

    neighbors[vertex].Clear();
    neighbors[vertex].Resize(neighbors[vertex1].Size() + neighbors[vertex2].Size());
    neighbors[vertex1].Remove(vertex);
    neighbors[vertex2].Remove(vertex);

    reduction.AddRemovedEdge(vertex, vertex1);
    reduction.AddRemovedEdge(vertex1, vertex);
    reduction.AddRemovedEdge(vertex, vertex2);
    reduction.AddRemovedEdge(vertex2, vertex);

    for (int const neighbor1 : neighbors[vertex1]) {
        if (neighbor1 == vertex) continue;
        neighbors[neighbor1].Remove(vertex1);
        reduction.AddRemovedEdge(neighbor1, vertex1);
        reduction.AddRemovedEdge(vertex1, neighbor1);
////        neighbors[neighbor1].Insert(vertex);
        neighbors[vertex].Insert(neighbor1);
        remaining.Insert(neighbor1);
    }
    neighbors[vertex1].Clear();

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    for (int const neighbor2 : neighbors[vertex2]) {
        if (neighbor2 == vertex) continue;
        neighbors[neighbor2].Remove(vertex2);
        reduction.AddRemovedEdge(neighbor2, vertex2);
        reduction.AddRemovedEdge(vertex2, neighbor2);
////        neighbors[neighbor2].Insert(vertex);
        neighbors[vertex].Insert(neighbor2);
        remaining.Insert(neighbor2);
    }

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    neighbors[vertex2].Clear();
    for (int const neighbor : neighbors[vertex]) {
////        if (neighbor == vertex) {
////            cout << "Something is very wrong..." << endl;
////        }
        neighbors[neighbor].Insert(vertex);
    }

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    remaining.Insert(vertex);

    vReductions.emplace_back(std::move(reduction));

    // which one goes in independent set? Need to update...
    vOtherRemovedVertices.push_back(vertex1);
    vOtherRemovedVertices.push_back(vertex2);

    remaining.Remove(vertex1);
    remaining.Remove(vertex2);
    inGraph.Remove(vertex2);
    inGraph.Remove(vertex1);

////    if (debug) {
////        cout << "AFTER:" << endl;
////        cout << vertex << ":";
////        for (int const neighbor : neighbors[vertex]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////
////        cout << vertex1 << ":";
////        for (int const neighbor : neighbors[vertex1]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////
////        cout << vertex2 << ":";
////        for (int const neighbor : neighbors[vertex2]) {
////            cout << " " << neighbor;
////        }
////        cout << endl;
////    }
////    cout << "Done folding..." << endl << flush;
    return true;
#else
    return false;
#endif 
}


#if 0
template <typename NeighborSet>
bool Isolates4<NeighborSet>::RemoveIsolatedPath(int const vertex, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<pair<int,int>> &vAddedEdges)
{
    if (neighbors[vertex].Size() != 2) return false;
#ifdef NON_PATH
    if (neighbors[neighbors[vertex][0]].Contains(neighbors[vertex][1])) return false; // neighbors can't be adjacent.

    int const vertex1(neighbors[vertex][0]);
    int const vertex2(neighbors[vertex][1]);

////    cout << "Folding: " << vertex << ", " << vertex1 << ", " << vertex2 << endl;
////    cout << "BEFORE:" << endl;
////    cout << vertex << ":";
////    for (int const neighbor : neighbors[vertex]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;
////
////    cout << vertex1 << ":";
////    for (int const neighbor : neighbors[vertex1]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;
////
////    cout << vertex2 << ":";
////    for (int const neighbor : neighbors[vertex2]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;

    neighbors[vertex].Clear();
    neighbors[vertex].Resize(neighbors[vertex1].Size() + neighbors[vertex2].Size());
    neighbors[vertex1].Remove(vertex);
    neighbors[vertex2].Remove(vertex);
    for (int const neighbor1 : neighbors[vertex1]) {
        if (neighbor1 == vertex) continue;
        neighbors[neighbor1].Remove(vertex1);
////        neighbors[neighbor1].Insert(vertex);
        neighbors[vertex].Insert(neighbor1);
        remaining.Insert(neighbor1);
    }
    neighbors[vertex1].Clear();

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    for (int const neighbor2 : neighbors[vertex2]) {
        if (neighbor2 == vertex) continue;
        neighbors[neighbor2].Remove(vertex2);
////        neighbors[neighbor2].Insert(vertex);
        neighbors[vertex].Insert(neighbor2);
        remaining.Insert(neighbor2);
    }

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    neighbors[vertex2].Clear();
    for (int const neighbor : neighbors[vertex]) {
////        if (neighbor == vertex) {
////            cout << "Something is very wrong..." << endl;
////        }
        neighbors[neighbor].Insert(vertex);
    }

////    cout << "vertex " << vertex << " contains self?=" << (neighbors[vertex].Contains(vertex)) << endl;

    remaining.Insert(vertex);

    // which one goes in independent set? Need to update...
    vOtherRemovedVertices.push_back(vertex1);
    vOtherRemovedVertices.push_back(vertex2);

    remaining.Remove(vertex1);
    remaining.Remove(vertex2);
    inGraph.Remove(vertex2);
    inGraph.Remove(vertex1);

////    cout << "AFTER:" << endl;
////    cout << vertex << ":";
////    for (int const neighbor : neighbors[vertex]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;
////
////    cout << vertex1 << ":";
////    for (int const neighbor : neighbors[vertex1]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;
////
////    cout << vertex2 << ":";
////    for (int const neighbor : neighbors[vertex2]) {
////        cout << " " << neighbor;
////    }
////    cout << endl;

#else

    for (int const neighbor : neighbors[vertex]) {
        if (neighbors[neighbor].Size() == 2) {
            isolates.Insert(vertex);

            int endVertex1(-1);
            int endVertex2(-1);

            // remove from other neighbor's list...
            for (int const nNeighbor : neighbors[neighbor]) {
                if (nNeighbor != vertex) {
                    endVertex1 = nNeighbor;
                    neighbors[nNeighbor].Remove(neighbor);
////                    cout << __LINE__ << ": Removing edge " << nNeighbor << "," << neighbor << endl;
                    remaining.Insert(nNeighbor);
                    break;
                }
            }

            for (int const otherNeighbor : neighbors[vertex]) {
                if (otherNeighbor != neighbor) {
                    endVertex2 = otherNeighbor;
                    neighbors[otherNeighbor].Remove(vertex);
////                    cout << __LINE__ << ": Removing edge " << otherNeighbor << "," << vertex << endl;
                    remaining.Insert(otherNeighbor);

                    if (!neighbors[otherNeighbor].Contains(endVertex1)) {

                        m_AdjacencyArray[otherNeighbor].push_back(endVertex1);
                        m_AdjacencyArray[endVertex1].push_back(otherNeighbor);

                        neighbors[otherNeighbor].Insert(endVertex1);
                        neighbors[endVertex1].Insert(otherNeighbor);
                        vAddedEdges.push_back(make_pair(endVertex1, otherNeighbor));
                        break;
                    }
                }
            }

            neighbors[vertex].Clear();
            neighbors[neighbor].Clear();

            vIsolateVertices.push_back(vertex);
////        if (vertex == 0) {
////            cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////        }
            vOtherRemovedVertices.push_back(neighbor);
////        if (neighbor == 0) {
////            cout << __LINE__ << ": Removing " << neighbor << " from graph." << endl << flush;
////        }

            remaining.Remove(vertex);
            remaining.Remove(neighbor);

            m_AlternativeVertices[vertex]   = neighbor;
            m_AlternativeVertices[neighbor] = vertex;
            ////cout << "Vertex " << vertex << " has alternative " << neighbor << endl;

////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(vertex);
////            cout << __LINE__ << ": calling remove" << endl << flush;
            inGraph.Remove(neighbor);
////            cout << "Removing Path: [" << vertex << " " << neighbor << "]" << " (" << endVertex1 << " " << endVertex2 << ")" << endl << flush;

            return true;
        }
    }
#endif
    return false;
}
#endif // 0
#endif // NEW_ISOLATE_CHECKS


// TODO/DS: need to add 2-neighbors to remaining.
template <typename NeighborSet>
void Isolates4<NeighborSet>::RemoveVertex(int const vertex, vector<Reduction> &vReductions)
{
////    cout << __LINE__ << ": Removing vertex " << vertex << endl << flush;
////    cout << __LINE__ << ": calling remove" << endl << flush;
    inGraph.Remove(vertex);
    remaining.Remove(vertex);
    isolates.Remove(vertex);

    Reduction reduction(REMOVED_VERTEX);
    reduction.SetVertex(vertex);

    for (int const neighbor : neighbors[vertex]) {
        neighbors[neighbor].Remove(vertex);
        remaining.Insert(neighbor);
        reduction.AddRemovedEdge(vertex, neighbor);
        reduction.AddRemovedEdge(neighbor, vertex);
    }

    vReductions.emplace_back(reduction);

    neighbors[vertex].Clear();
}

//TODO/DS: need to add 2-neighbors to remaining.
template <typename NeighborSet>
void Isolates4<NeighborSet>::RemoveVertexAndNeighbors(int const vertex, vector<int> &vRemoved, vector<Reduction> &vReductions)
{
////    cout << __LINE__ << ": Removing vertex " << vertex << endl << flush;
////    cout << __LINE__ << ": calling remove" << endl << flush;
    inGraph.Remove(vertex);
    remaining.Remove(vertex);
    isolates.Insert(vertex);
    vRemoved.push_back(vertex);

    Reduction reduction(REMOVED_VERTEX_AND_NEIGHBORS);
    reduction.SetVertex(vertex);
////    if (vertex == 0) {
////        cout << __LINE__ << ": Removing " << vertex << " from graph." << endl << flush;
////    }

    for (int const neighbor : neighbors[vertex]) {
////        cout << __LINE__ << ":     Removing neighbor " << neighbor << endl << flush;
////        cout << __LINE__ << ": calling remove" << endl << flush;
        inGraph.Remove(neighbor);
        remaining.Remove(neighbor);
        vRemoved.push_back(neighbor);

        reduction.AddNeighbor(neighbor);
        reduction.AddRemovedEdge(vertex, neighbor);
        reduction.AddRemovedEdge(neighbor, vertex);
////        if (neighbor == 0) {
////            cout << __LINE__ << ": Removing " << neighbor << " from graph." << endl << flush;
////        }

        for (int const nNeighbor : neighbors[neighbor]) {
////            cout << __LINE__ << ":         Removing from neighbor's neighbor "<< nNeighbor << endl << flush;
            if (nNeighbor == vertex) continue;
            neighbors[nNeighbor].Remove(neighbor);
////            cout << __LINE__ << ":         Done removing" << endl << flush;
            if (inGraph.Contains(nNeighbor)) {
                remaining.Insert(nNeighbor);
                reduction.AddRemovedEdge(neighbor, nNeighbor);
                reduction.AddRemovedEdge(nNeighbor, neighbor);
            }
        }
////        cout << __LINE__ << ":     Done Removing neighbor" << neighbor << endl << flush;
        neighbors[neighbor].Clear();
////        cout << __LINE__ << ": Cleared neighbors: " << neighbor << endl << flush;
    }

////    cout << __LINE__ << ": Done Removing vertex " << vertex << endl << flush;
    neighbors[vertex].Clear();

    vReductions.emplace_back(std::move(reduction));
////    cout << __LINE__ << ": Cleared neighbors: " << vertex << endl << flush;
}

template <typename NeighborSet>
void Isolates4<NeighborSet>::RemoveAllIsolates(int const independentSetSize, vector<int> &vIsolateVertices, vector<int> &vOtherRemovedVertices, vector<Reduction> &vReductions, bool bConsiderAllVertices)
{
////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "Calling RemoveAllIsolates with 31 in the isolate set!" << endl;
#ifdef TIMERS
    clock_t startClock = clock();
#endif // TIMERS
////    remaining = inGraph; // TODO/DS : We can optimize this by knowing which vertex (and neighbors where removed last.
////    if (vOtherRemovedVertices.empty()) {

        // TODO/DS: Put this in; it saves us from having to consider obvious non-candidates. Only works if we establish
        // the invariant that the graph contains no vertices that can be reduced.
////        if (bConsiderAllVertices) { ////true) { //bConsiderAllVertices) {
        if (true) { //bConsiderAllVertices) {
            remaining.Clear();
            for (int const vertex : inGraph) {
                remaining.Insert(vertex);
            }
        }
////    } else {
////        remaining.clear();
////        for (int const removedVertex : vOtherRemovedVertices) {
////            remaining.insert(neighbors[removedVertex].begin(), neighbors[removedVertex].end());
////        }
////    }
////    cout << "Removing all isolates." << endl << flush;
    int iterations(0);
    while (!remaining.Empty()) {
////        size_t const numRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if ((vIsolateVertices.size() + vOtherRemovedVertices.size()) %10000 == 0)
////        cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others" << endl << flush;
        int const vertex = *(remaining.begin());
        remaining.Remove(vertex);

    // can prune out high-degree vertices too. // but this is slow right now.

////        if (inGraph.size() - neighbors[vertex].size() < independentSetSize) {
////            remaining.insert(neighbors[vertex].begin(), neighbors[vertex].end());
////            RemoveVertex(vertex);
////            vOtherRemovedVertices.push_back(vertex);
////            continue;
////        }

////        cout << "Attempting to remove vertex " << vertex << endl << flush;

        bool reduction = RemoveIsolatedClique(vertex, vIsolateVertices, vOtherRemovedVertices, vReductions);
#ifdef NEW_ISOLATE_CHECKS
#if 1 ////def FOLD_VERTEX
        if (!reduction && m_bAllowVertexFolds) {
////            reduction = RemoveIsolatedPath(vertex, vIsolateVertices, vOtherRemovedVertices, vAddedEdges);
            reduction = FoldVertex(vertex, vIsolateVertices, vOtherRemovedVertices, vReductions);
        }
#endif // 0
#ifdef DOMINATION_CHECKS
        if (!reduction) {
            reduction = RemoveDominatedVertex(vertex, vIsolateVertices, vOtherRemovedVertices, vReductions);
        }
#endif //DOMINATION_CHECKS

#endif // NEW_ISOLATE_CHECKS

////    if (find (vIsolateVertices.begin(), vIsolateVertices.end(), 31) != vIsolateVertices.end())
////        cout << "31 was added to the isolate set!" << endl;
////        if (!inGraph.Contains(31)) cout << "And it's not in the graph..." << endl << flush;

        iterations++;

////        size_t const numNewRemoved(vIsolateVertices.size() + vOtherRemovedVertices.size());
////        if (numNewRemoved != numRemoved) {
////            cout << "Progress: Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;
////            iterations = 0;
////            cout << "Remaining graph has " << inGraph.Size() << " vertices." << endl << flush;
////        }
    }

////    cout << "Removed " << isolates.size() - isolateSize << " isolates." << endl << flush;
////    cout << "Removed: " << vIsolateVertices.size() << " isolates, and " << vOtherRemovedVertices.size() << " others, in " << iterations << " iterations." << endl << flush;

#ifdef TIMERS
    clock_t endClock = clock();
    removeTimer += (endClock - startClock);
#endif // TIMERS
}

template <typename NeighborSet>
void Isolates4<NeighborSet>::ReplaceAllRemoved(vector<Reduction> const &vReductions)
{
#ifdef TIMERS
    clock_t startClock = clock();
#endif //TIMERS
#if 1
    for (size_t index = vReductions.size(); index > 0; index--) {
        Reduction const &reduction(vReductions[index-1]);

////        bool const debug(m_AdjacencyArray.size() == 2738);

        switch(reduction.GetType()) {
            case ISOLATED_VERTEX:
////        if (debug) {
////            cout << "Undoing ISOLATED_VERTEX reduction" << endl;
////        }
                inGraph.Insert(reduction.GetVertex());
                isolates.Remove(reduction.GetVertex());
                for (int const neighbor : reduction.GetNeighbors()) {
                    inGraph.Insert(neighbor);
                }
                for (pair<int,int> const &edge : reduction.GetRemovedEdges()) {
                    neighbors[edge.first].Insert(edge.second);
                }
            break;
            case DOMINATED_VERTEX:
////        if (debug) {
////            cout << "Undoing DOMINATED_VERTEX reduction" << endl;
////        }
                inGraph.Insert(reduction.GetVertex());
                for (pair<int,int> const &edge : reduction.GetRemovedEdges()) {
                    neighbors[edge.first].Insert(edge.second);
                }
            break;
            case FOLDED_VERTEX:
////        if (debug) {
////            cout << "Undoing FOLDED_VERTEX reduction" << endl;
////        }
                foldedVertexCount--;
                inGraph.Insert(reduction.GetNeighbors()[0]);
                inGraph.Insert(reduction.GetNeighbors()[1]);
                // first remove all added edges
                for (int const neighbor : neighbors[reduction.GetVertex()]) {
                    neighbors[neighbor].Remove(reduction.GetVertex());
                }
                neighbors[reduction.GetVertex()].Clear();
                // then replace all removed edges
                for (pair<int,int> const &edge : reduction.GetRemovedEdges()) {
                    neighbors[edge.first].Insert(edge.second);
                }
            break;
            case REMOVED_VERTEX:
////        if (debug) {
////            cout << "Undoing REMOVED_VERTEX reduction" << endl;
////        }
                inGraph.Insert(reduction.GetVertex());
                // then replace all removed edges
                for (pair<int,int> const &edge : reduction.GetRemovedEdges()) {
                    neighbors[edge.first].Insert(edge.second);
                }
            break;
            case REMOVED_VERTEX_AND_NEIGHBORS:
////        if (debug) {
////            cout << "Undoing REMOVED_VERTEX_AND_NEIGHBORS reduction" << endl;
////        }
////                cout << "Inserting into graph..." << endl << flush;
                inGraph.Insert(reduction.GetVertex());
                isolates.Remove(reduction.GetVertex());
////                cout << "Inserting neighbors..." << endl << flush;
                for (int const neighbor : reduction.GetNeighbors()) {
                    inGraph.Insert(neighbor);
                }
                // then replace all removed edges
////                cout << "Inserting removed edges..." << endl << flush;
                for (pair<int,int> const &edge : reduction.GetRemovedEdges()) {
                    neighbors[edge.first].Insert(edge.second);
                }
            break;
            default:
                cout << "ERROR!: Unsupported reduction type used..." << endl << flush;
                exit(1);
            break;
        };
    }
#else
    for (int const removedVertex : vRemoved) {
        inGraph.Insert(removedVertex);
        isolates.Remove(removedVertex);
        m_AlternativeVertices.erase(removedVertex);
    }
////    cout << "Replacing all removed vertices." << endl << flush;
    for (int const removedVertex : vRemoved) {
////        if (remaining.size() %10000 == 0)
////            cout << "Remaining: " << remaining.size() << ", Removed: " << removed.size() << endl << flush;
        for (int const neighbor : m_AdjacencyArray[removedVertex]) {
            if (!inGraph.Contains(neighbor)) continue;
            neighbors[removedVertex].Insert(neighbor);
            neighbors[neighbor].Insert(removedVertex);
        }
    }
////    cout << "Done replacing vertices..." << endl << flush;
#endif // 1

#ifdef TIMERS
    clock_t endClock = clock();
    replaceTimer += (endClock - startClock);
    #endif // TIMERS
}

#if 0
template <typename NeighborSet>
int Isolates4<NeighborSet>::NextVertexToRemove(std::vector<int> &vVertices)
{
#ifdef TIMERS
    clock_t startClock = clock();
    #endif // TIMERS

#if 1
    if (vVertices.empty()) return -1;

////    set<int> setVertices; setVertices.insert(vVertices.begin(), vVertices.end());
////    cout << "Choices: ";
////    for (int const vertex : setVertices) {
////        cout << vertex << "(degree=" << neighbors[vertex].Size() << ") ";
////    }
////    cout << endl << flush;
    int index(0);
    size_t currentMaxSize(neighbors[vVertices[index]].Size());
    for (int i = 1; i < vVertices.size(); ++i) {
        size_t const newCandidateSize(neighbors[vVertices[i]].Size());
////        cout << vVertices[i] << "(" << newCandidateSize << " neighbors) ";

        // break ties in favor of vertex with smaller id
        if ((currentMaxSize < newCandidateSize) || ((currentMaxSize == newCandidateSize) && (vVertices[i] < vVertices[index])))
        {
            currentMaxSize = newCandidateSize;
            index = i;
        }
    }
////    cout << endl << flush;

    int const vertexWithMaxReductions = vVertices[index];
////    cout << "Choosing to remove " << vertexWithMaxReductions << " next, with " << neighbors[vVertices[index]].Size() << " neighbors " << endl; 

    vVertices[index]= vVertices.back();
    vVertices.pop_back();

#ifdef TIMERS
    clock_t endClock = clock();
    timer += (endClock - startClock);
#endif // TIMERS

    return vertexWithMaxReductions;
#else

    remaining.Clear();
    for (int const vertex : inGraph) {
        remaining.Insert(vertex); // only consider neighbors of removed vertices... makes it faster.
    }

    vector<int> &vVerticesOrderedByDegree(vVertices);

#ifdef TIMERS
    clock_t const startSort(clock());
#endif // TIMERS
    auto sortByDegree = [this](int const &leftVertex, int const &rightVertex) { return neighbors[leftVertex].Size() > neighbors[rightVertex].Size(); };
    sort(vVerticesOrderedByDegree.begin(), vVerticesOrderedByDegree.end(), sortByDegree);
#ifdef TIMERS
    sortDuringNextTimer += (clock() - startSort);
#endif // TIMERS

    if (vVerticesOrderedByDegree.empty()) return -1;

    int vertexWithMaxReductions(vVerticesOrderedByDegree[0]); // default to max degree vertex
    int maxIsolates(-1);
    int maxRemoved(-1);
    int maxIndex(0);
    size_t index(0); // default to index of maximum degree vertex

    for (int const vertex : inGraph) {
#ifdef SPARSE
        SparseArraySet &neighborSet(neighbors[vertex]);
#else
        NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
        neighborSet.SaveState(); // checkpoint the neighbors so we can easily restore them after test-removing vertices.
    }

////    cout << "Testing remaining " << inGraph.size() << " vertices, to maximize isolate removal" << endl << flush;
    for (int const vertex : vVerticesOrderedByDegree) {
////        cout << "Starting loop..." << endl << flush;
////        set<int> tempRemaining(remaining); // TODO/DS: put back?

        // TODO/DS: Remove when vertex selection becomes faster, this is a compromise between fast vertex selection and good vertex selection.
        if (index >= vVerticesOrderedByDegree.size()/10) break;
////        if (index >= 2) break;

#ifdef DEBUG
#ifdef SPARSE
        vector<SparseArraySet> savedNeighbors(neighbors);
#else
        vector<NeighborSet> savedNeighbors(neighbors);
#endif // SPARSE
#endif // DEBUG

        remaining.Clear();

        inGraph.SaveState();

#ifdef DEBUG
        ArraySet savedInGraph(inGraph);
#endif // DEBUG

#ifdef SPARSE 
        SparseArraySet tempIsolates(m_AdjacencyArray.size());
#else
        ArraySet tempIsolates(m_AdjacencyArray.size());
#endif // SPARSE
        for (int const isolateVertex : isolates) {
            tempIsolates.Insert(isolateVertex);
        }

#ifdef TIMERS
        clock_t startRemoveOne(clock());
        #endif // TIMERS
        vector<int> vRemoved;
////        cout << "Try removing vertex " << vertex << endl << flush;
#ifdef OPTIMIZE_BRANCHING
        RemoveVertex(vertex); vRemoved.push_back(vertex);
#else
        RemoveVertexAndNeighbors(vertex, vRemoved);
#endif // OPTIMIZE_BRANCHING
////        cout << "And the new isolates..." << vertex << endl << flush;
#ifdef TIMERS
        removeOneDuringNextTimer += (clock()-startRemoveOne);
#endif // TIMERS
        vector<pair<int,int>> vAddedEdges;

#ifdef TIMERS
        clock_t startremove(clock());
        clock_t startRealRemove(clock());
        #endif // TIMERS
        RemoveAllIsolates(0, vRemoved, vRemoved, vAddedEdges);
#ifdef TIMERS
        removeTimer -= (clock() - startRealRemove);
        removeDuringNextTimer += (clock() - startremove);

        clock_t startReplace(clock()); // restoring all sets is considered replace
        #endif // TIMERS
        int const newNumRemoved(m_AdjacencyArray.size() - inGraph.Size());

        inGraph.RestoreState();
        int const oldNumRemoved(m_AdjacencyArray.size() - inGraph.Size());
        int const deltaRemoved(newNumRemoved - oldNumRemoved);
#if 0 //def OPTIMIZE_ISOLATES
        if (static_cast<int>(isolates.size() - tempIsolates.size()) > maxIsolates) {
            maxIsolates = static_cast<int>(isolates.size() - tempIsolates.size());
            vertexWithMaxReductions = vertex;
        }
#else // optimize removed vertices
        if (deltaRemoved > maxRemoved) {
            maxRemoved = deltaRemoved;
            vertexWithMaxReductions = vertex;
            maxIndex = index;
        }
#endif

////        remaining = std::move(tempRemaining); // TODO/DS: put back?

        isolates.Clear();
        for (int const isolatedVertex : tempIsolates) {
            isolates.Insert(isolatedVertex);
        }

        // need to do this after restoring variable inGraph
////        RemoveEdges(vAddedEdges); // TODO/DS: Put back...


////        for (int const removedVertex : vRemoved) {
////            inGraph.Insert(removedVertex);
////            isolates.Remove(removedVertex);
////            m_AlternativeVertices.erase(removedVertex);
////        }

        for (int const vertex : inGraph) {
#ifdef SPARSE
            SparseArraySet &neighborSet(neighbors[vertex]);
#else
            NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
            neighborSet.RestoreState(); // checkpoint the neighbors so we can easily restore them after test-removing vertices.
            neighborSet.SaveState();
        }

#ifdef DEBUG
        if (savedInGraph != inGraph) {
            std::cout << "graph size changed..." << inGraph.Size() << "->" << savedInGraph.Size() << std::endl;
        }

        for (int vertex = 0; vertex < m_AdjacencyArray.size(); vertex++) {
            if (savedNeighbors[vertex].Size() != neighbors[vertex].Size()) {
                std::cout << "vertex " << vertex << "'s degree changed!" << std::endl << std::flush;
                std::cout << "before: " << savedNeighbors[vertex].Size() << ", after: " << neighbors[vertex].Size() << endl;
                std::cout << "before: ";
                for (int const neighbor : savedNeighbors[vertex]) {
                    cout << neighbor << " ";
                }
                std::cout << "after: ";
                for (int const neighbor : neighbors[vertex]) {
                    cout << neighbor << " ";
                }
            }
        }
#endif // DEBUG

////        clock_t startreplace(clock());
////        ReplaceAllRemoved(vRemoved);
////        replaceTimer -= (clock() - startreplace);

        index++;

////        cout << "Done with loop" << endl;
    }

#ifdef TIMERS
    clock_t endClock = clock();
    timer += (endClock - startClock);

    clock_t startReplace(clock());
#endif // TIMERS
    for (int const vertex : inGraph) {
#ifdef SPARSE
        SparseArraySet &neighborSet(neighbors[vertex]);
#else
        NeighborSet &neighborSet(neighbors[vertex]);
#endif // SPARSE
        neighborSet.RestoreState();
    }
#ifdef TIMERS
    replaceDuringNextTimer += (clock() - startReplace);
    #endif // TIMERS

    // inefficient, should be a better way to elmininate it before getting here.
#ifdef SLOW
    for (int &vertex : vVertices) {
        if (vertex == vertexWithMaxReductions) {
            vertex = vVertices.back();
            vVertices.pop_back();
            return vertexWithMaxReductions;
        }
    }
#else
    vVertices[maxIndex] = vVertices.back();
    vVertices.pop_back();
    return vertexWithMaxReductions;
#endif // SLOW
#endif // 0

    return -1;
}

template <typename NeighborSet>
int Isolates4<NeighborSet>::NextVertexToRemove()
{
    vector<int> vVertices;
    vVertices.insert(vVertices.end(), inGraph.begin(), inGraph.end());
    return NextVertexToRemove(vVertices);
}
#endif // 0

template <typename NeighborSet>
int Isolates4<NeighborSet>::GetAlternativeVertex(int const vertex) const
{
    map<int,int>::const_iterator cit(m_AlternativeVertices.find(vertex));
    if (cit != m_AlternativeVertices.end()) {
        return cit->second;
    }
    return -1;
}

// vVertices needs to be a subset of the graph, otherwise this will
// fail horribly.
template <typename NeighborSet>
void Isolates4<NeighborSet>::SetConnectedComponent(vector<int> const &vVertices)
{
    inGraph.Clear();
    for (int const vertex : vVertices) {
        inGraph.Insert(vertex);
    }

    m_bConnectedComponentMode = true;
}

template class Isolates4<ArraySet>;
template class Isolates4<SparseArraySet>;

