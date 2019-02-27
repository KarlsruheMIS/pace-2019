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

#include "Algorithm.h"

// system includes
#include <string>

using namespace std;

Algorithm::Algorithm(std::string const &name)
 : m_sName(name)
 , m_bQuiet(false)
 , m_vCallBacks()
{
}

Algorithm::~Algorithm()
{
}

void Algorithm::AddCallBack(std::function<void(std::list<int> const&)> callback)
{
    m_vCallBacks.push_back(callback);
}

void Algorithm::ExecuteCallBacks(std::list<int> const &vertexSet) const
{
    for (auto &function : m_vCallBacks) {
        function(vertexSet);
    }
}

void Algorithm::SetName(string const &name)
{
    m_sName = name;
}

string Algorithm::GetName() const
{
    return m_sName;
}

void Algorithm::SetQuiet(bool const quiet)
{
    m_bQuiet = quiet;
}

bool Algorithm::GetQuiet() const
{
    return m_bQuiet;
}
