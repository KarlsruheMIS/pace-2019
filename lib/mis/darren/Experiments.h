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

#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <string>
#include <vector>

class Experiments
{
public:
    Experiments(std::string const sDataSetName, double const dTimeout, bool const bOutputLatex, bool const bPrintHeader, std::vector<std::vector<int>> const &adjacencyArray);

    void RunKernelSize() const;
    int KernelizeAndRunComponentWiseMISS() const;
    void RunComponentsMISS() const;
    void ComputeCriticalIndependentSet() const;
    void ComputeCriticalIndependentSetKernel() const;
    void ComputeMaximumCriticalIndependentSetKernel() const;
    void ComputeMaximumCriticalIndependentSet() const;

private:
    std::string m_sDataSetName;
    bool const m_bOutputLatex;
    bool const m_bPrintHeader;
    double const m_dTimeout;
    std::vector<std::vector<int>> const &m_AdjacencyArray;
};
#endif // EXPERIMENTS_H
