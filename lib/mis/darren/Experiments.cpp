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

#include "Experiments.h"
#include "Isolates4.h"
#include "SparseArraySet.h"
#include "LightWeightFullMISS.h"
#include "GraphTools.h"
#include "Tools.h"
#include "CliqueTools.h"

#include <list>
#include <cstdlib>

using namespace std;

Experiments::Experiments(string const sDataSetName, double const dTimeout, bool const bOutputLatex, bool const bPrintHeader, vector<vector<int>> const &adjacencyArray)
 : m_sDataSetName(sDataSetName)
 , m_dTimeout(dTimeout)
 , m_bOutputLatex(bOutputLatex)
 , m_bPrintHeader(bPrintHeader)
 , m_AdjacencyArray(adjacencyArray)
{
}

void Experiments::RunKernelSize() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & $k$ & $c$ & $l$ \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\tk\tc\tl" << endl << flush;
        }
    }

    size_t const numVertices(m_AdjacencyArray.size());
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    Isolates4<SparseArraySet> isolates(m_AdjacencyArray);
    vector<int> vIsolates;
    vector<int> vRemoved;
    vector<Reduction> vReductions;

    clock_t startTime(clock());
    isolates.RemoveAllIsolates(0, vIsolates, vRemoved, vReductions, true /* consider all vertices for removal */);
    clock_t endTime(clock());

    size_t const kernelSize(isolates.GetInGraph().Size());

    vector<vector<int>> vComponents;
    GraphTools::ComputeConnectedComponents(isolates, vComponents, m_AdjacencyArray.size());

    size_t const numComponents(vComponents.size());
    size_t largestComponentSize(0);
    for (vector<int> const &vComponent : vComponents) {
        largestComponentSize = max(vComponent.size(), largestComponentSize);
    }

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << "&" << kernelSize << " & " << numComponents << " & " << largestComponentSize << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << kernelSize << "\t" << numComponents << "\t" << largestComponentSize << endl << flush;
    }
}

int Experiments::KernelizeAndRunComponentWiseMISS() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & $k$ & c & l & OPT \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\tk\tc\tl\tOPT" << endl << flush;
        }
    }


    size_t const numVertices(m_AdjacencyArray.size());
    // std::cout << "num vertices " << numVertices << std::endl;
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    Isolates4<SparseArraySet> isolates(m_AdjacencyArray);
    vector<int> vIsolates;
    vector<int> vRemoved;
    vector<Reduction> vReductions;
    clock_t const startTime(clock());
    // isolates.RemoveAllIsolates(0, vIsolates, vRemoved, vReductions, true /* consider all vertices for removal */);

    size_t const kernelSize(isolates.GetInGraph().Size());
    // cout << "Kernel size " << kernelSize << endl;

    vector<vector<int>> vComponents;
    GraphTools::ComputeConnectedComponents(isolates, vComponents, m_AdjacencyArray.size());

    size_t const numComponents(vComponents.size());
    // cout << "Total comps " << numComponents << endl;
    size_t largestComponentSize(0);
    for (vector<int> const &vComponent : vComponents) {
        largestComponentSize = max(vComponent.size(), largestComponentSize);
    }
    // cout << "Largest comp " << largestComponentSize << endl;

    size_t solutionDelta(0);

    // TODO: Compute IS size for each component and pass size down to algorithm.SetMaximumCliqueSize(size)
    for (vector<int> const &vComponent : vComponents) {

        if (vComponent.size() > 20000) {
            cerr << "ERROR!: unable to compute adjacencyMatrix, since the graph is too large: " << vComponent.size() << endl << flush;
            exit(1);
        }

        vector<vector<char>> vAdjacencyMatrix;
        vAdjacencyMatrix.resize(vComponent.size());

        int is_size = 0;

        map<int,int> vertexRemap;
////        map<int,int> reverseMap;
        size_t uNewIndex = 0;

        for (int const vertex : vComponent) {
            vertexRemap[vertex] = uNewIndex++;
            // cout << "v " << vertex << endl;
////            reverseMap[uNewIndex-1] = vertex;
        }

        for (pair<int,int> const &mapPair : vertexRemap) {
            int const oldVertex(mapPair.first);
            int const newVertex(mapPair.second);

            vAdjacencyMatrix[newVertex].resize(vComponent.size(), 0);
            for (int const neighbor : isolates.Neighbors()[oldVertex]) {
                if (vertexRemap.find(neighbor) != vertexRemap.end()) {
                    //cout << "newVertex           =" << newVertex << endl; 
                    vAdjacencyMatrix[newVertex][vertexRemap[neighbor]] = 1;
                }
            }
        }

        ////cout << "vAdjacencyArray.size=" << vAdjacencyArray.size() << endl;
        LightWeightFullMISS algorithm(vAdjacencyMatrix);
        algorithm.SetQuiet(true);

        list<list<int>> indsets;
        algorithm.Run(indsets);

#ifdef VERIFY
        cout << "Verifying independent set:" << endl;
        set<int> const indepset(indsets.back().begin(), indsets.back().end());
        for (int const vertex : indepset) {
            for (int const otherVertex : indepset) {
                if (vAdjacencyMatrix[vertex][otherVertex])
                    cout << "    ERROR: " << vertex << " and " << otherVertex << " are neighbors" << endl << flush;
            }
        }
#endif // VERIFY

        solutionDelta += indsets.back().size();
    }

    clock_t endTime(clock());

    int const solutionSize(solutionDelta + vReductions.size());

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << "&" << kernelSize << " & " << numComponents << " & " << largestComponentSize << " & " << solutionSize << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << kernelSize << "\t" << numComponents << "\t" << largestComponentSize << "\t" << solutionSize << endl << flush;
    }

    return solutionSize;
}

void Experiments::RunComponentsMISS() const
{
    Isolates4<SparseArraySet> isolates(m_AdjacencyArray);
    vector<int> vIsolates;
    vector<int> vRemoved;
    vector<Reduction> vReductions;
    isolates.RemoveAllIsolates(0, vIsolates, vRemoved, vReductions, true /* consider all vertices for removal */);

    clock_t startTime(clock());
    vector<vector<int>> vComponents;
    GraphTools::ComputeConnectedComponents(isolates, vComponents, m_AdjacencyArray.size());
////    cerr << "# vertices remaining in graph: " << isolates.GetInGraph().Size() << "/" << m_AdjacencyArray.size() << endl << flush;
////    cerr << "# connected components       : " << vComponents.size() << endl << flush;
////    cerr << "size of connected components : ";
////    cout << "[ ";
////    for (size_t index = 0; index < vComponents.size(); ++index) {
////        vector<int> const& vComponent(vComponents[index]);
////        cout << vComponent.size() << " ";
////    }
////    cout << "]" << endl;

    size_t totalCliqueSize(isolates.size() + isolates.GetFoldedVertexCount());

////    cout << "Initial independent set size: " << totalCliqueSize << endl << flush;

    for (vector<int> const &vComponent : vComponents) {
        if (vComponent.empty()) continue;
        size_t const previousTotalCliqueSize(totalCliqueSize);
        vector<vector<int>> componentAdjacencyList;
        set<int> componentSet(vComponent.begin(), vComponent.end());
        map<int,int> mapUnused;
        GraphTools::ComputeInducedSubgraphIsolates(isolates, componentSet, componentAdjacencyList, mapUnused);

////        cout << "component-size=" << componentAdjacencyList.size() << " " << Tools::GetTimeInSeconds(clock() - startTime) << endl << flush;

        vector<vector<char>> componentAdjacencyMatrix(componentAdjacencyList.size());
        for (size_t index = 0; index < componentAdjacencyList.size(); ++index) {
            componentAdjacencyMatrix[index].resize(componentAdjacencyList.size(), 0);
            componentAdjacencyMatrix[index][index] = 1;
            for (int const neighbor : componentAdjacencyList[index]) {
                componentAdjacencyMatrix[index][neighbor] = 1;
            }
        }

        clock_t newTime(clock());
        LightWeightFullMISS algorithm(componentAdjacencyMatrix);
        algorithm.SetQuiet(false); 
            ////        algorithm.SetOnlyVertex(vOrdering[splitPoint]);
        algorithm.SetTimeOutInSeconds(m_dTimeout);
        list<list<int>> cliques;
////        algorithm.SetQuiet(true);
        algorithm.Run(cliques);

////            if (algorithm)

        if (previousTotalCliqueSize + cliques.back().size() > totalCliqueSize) {
            totalCliqueSize = cliques.back().size() + previousTotalCliqueSize;
////            cout << "Found a better independent set: size=" << totalCliqueSize << "=" << cliques.back().size() << "+" << previousTotalCliqueSize << endl;
        }
    }

    clock_t const endTime(clock());

////    cout << "Found independent set of size " << totalCliqueSize << endl << flush;

    if (m_bOutputLatex) {
        cout << " & " << Tools::GetTimeInSeconds(endTime - startTime, false /* no brackets */) << flush;
    } else {
        cout << "\t" << Tools::GetTimeInSeconds(endTime - startTime, false /* no brackets */) << flush;
    }
}

void Experiments::ComputeCriticalIndependentSet() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & $k$ & c & l \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\tk\tc\tl" << endl << flush;
        }
    }


    size_t const numVertices(m_AdjacencyArray.size());
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    Isolates4<SparseArraySet> isolates(m_AdjacencyArray);

    clock_t startTime(clock());

    set<int> const criticalSet(CliqueTools::ComputeCriticalIndependentSet(m_AdjacencyArray));
    cout << "Critical set (" << criticalSet.size() << " elements):" << endl;

#if 0
    size_t const kernelSize(isolates.GetInGraph().Size());

    vector<vector<int>> vComponents;
    GraphTools::ComputeConnectedComponents(isolates, vComponents, m_AdjacencyArray.size());

    size_t const numComponents(vComponents.size());
    size_t largestComponentSize(0);
    for (vector<int> const &vComponent : vComponents) {
        largestComponentSize = max(vComponent.size(), largestComponentSize);
    }

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << "&" << kernelSize << " & " << numComponents << " & " << largestComponentSize << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << kernelSize << "\t" << numComponents << "\t" << largestComponentSize << endl << flush;
    }
#endif
}

void Experiments::ComputeCriticalIndependentSetKernel() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & |I| & $k$ \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\t|I|\tk" << endl << flush;
        }
    }


    size_t const numVertices(m_AdjacencyArray.size());
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    clock_t startTime(clock());

    set<int> independentVertices;
    set<int> const remainingVertices(CliqueTools::IterativelyRemoveCriticalIndependentSets(m_AdjacencyArray, independentVertices));
////    cout << "Remaining graph (" << remainingVertices.size() << " elements):" << endl;

    clock_t endTime(clock());

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << "&" << independentVertices.size() << " &" << remainingVertices.size()/2 << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << independentVertices.size() << "\t" << remainingVertices.size()/2 << endl << flush;
    }
}

void Experiments::ComputeMaximumCriticalIndependentSetKernel() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & $|I|$ & $k$ \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\t|I|\tk" << endl << flush;
        }
    }


    size_t const numVertices(m_AdjacencyArray.size());
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    clock_t startTime(clock());

    set<int> independentVertices;
    set<int> const remainingVertices(CliqueTools::IterativelyRemoveMaximumCriticalIndependentSets(m_AdjacencyArray, independentVertices));
////    cout << "Remaining graph (" << remainingVertices.size() << " elements):" << endl;

    clock_t endTime(clock());

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << " & " << independentVertices.size() << " & " << remainingVertices.size()/2 << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << independentVertices.size() << "\t" << remainingVertices.size()/2 << endl << flush;
    }
}

void Experiments::ComputeMaximumCriticalIndependentSet() const
{
    if (m_bPrintHeader) {
        if (m_bOutputLatex) {
            cout << "Graph Name & $n$ & $m$ & $t$ & $|I|$ & $k$ \\\\ \\hline" << endl << flush;
        } else {
            cout << "Graph Name\tn\tm\tt\t|I|\tk" << endl << flush;
        }
    }


    size_t const numVertices(m_AdjacencyArray.size());
    size_t numEdges(0);
    for (vector<int> const &neighbors : m_AdjacencyArray) {
        numEdges+= neighbors.size();
    }
    numEdges >>=1;

    clock_t startTime(clock());

    set<int> independentVertices;
    set<int> const remainingVertices(CliqueTools::ComputeMaximumCriticalIndependentSet(m_AdjacencyArray, independentVertices));
////    cout << "Remaining graph (" << remainingVertices.size() << " elements):" << endl;

    clock_t endTime(clock());

    if (m_bOutputLatex) {
        cout << m_sDataSetName << " & " << numVertices << " & " << numEdges << " & " << Tools::GetTimeInSeconds(endTime-startTime) << " & " << independentVertices.size() << " & " << remainingVertices.size() << " \\\\ " << endl << flush;
    } else {
        cout << m_sDataSetName << "\t" << numVertices << "\t" << numEdges << "\t" << Tools::GetTimeInSeconds(endTime-startTime) << "\t" << independentVertices.size() << "\t" << remainingVertices.size() << endl << flush;
    }
}
