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

#ifndef _MATCHING_TOOLS_H_
#define _MATCHING_TOOLS_H_

#include "BiDoubleGraph.h"

#include <set>
#include <vector>

namespace MatchingTools
{
    enum LastEdge {NO_LAST_EDGE, MATCHED_LAST_EDGE, UNMATCHED_LAST_EDGE, BOTH_LAST_EDGE, NULL_LAST_EDGE};

    std::set<int> ComputeBiDoubleMIS(std::vector<std::vector<int>> const &biDoubleGraph);
    std::set<int> ComputeLeftMIS(std::vector<std::vector<int>> const &biDoubleGraph);

    void ComputeAlternatingPaths(BiDoubleGraph const &biDouble, std::vector<int> const &vMatching, std::vector<LastEdge> &vOnAlternatingPath);
    void ComputeAlternatingPathsOptimized(BiDoubleGraph const &biDouble, std::vector<int> const &vMatching, std::vector<LastEdge> &vOnAlternatingPath);
    std::set<int> GetLeftVerticesOnAlternatingPaths(BiDoubleGraph const &biDouble, std::vector<int> const &vMatching, std::vector<MatchingTools::LastEdge> &vOnAlternatingPath);
    std::set<int> ComputeCriticalSet(std::vector<std::vector<int>> const &adjacencyList);
    std::set<int> ComputeBiDoubleMIS(std::vector<std::vector<int>> const &biDoubleGraph, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);
    std::set<int> ComputeLeftMIS(std::vector<std::vector<int>> const &biDoubleGraph, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);

// store vertices in graph, for repeated calls to matching.
    void ComputeAlternatingPathsOptimized(BiDoubleGraph const &biDouble, std::vector<int> const &vMatching, std::vector<LastEdge> &vOnAlternatingPath, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);

    std::set<int> GetVerticesOnAlternatingPaths(BiDoubleGraph const &biDouble, std::vector<int> const &vMatching, std::vector<MatchingTools::LastEdge> &vOnAlternatingPath, std::set<int> const &setInGraph);
    std::set<int> ComputeBiDoubleMISOptimized(BiDoubleGraph &biDouble, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);
    std::set<int> ComputeBiDoubleMISOptimizedWithMatching(BiDoubleGraph &biDouble, std::vector<int> &matching, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);
    std::set<int> ComputeLeftMISOptimizedWithMatching(BiDoubleGraph &biDouble, std::vector<int> &matching, std::vector<bool> const &vInGraph, std::set<int> const &setInGraph);


};
#endif // _MATCHING_TOOLS_H_
