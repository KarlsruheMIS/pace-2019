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

#ifndef ALGORITHM_H
#define ALGORITHM_H

// system includes
#include <vector>
#include <list>
#include <string>
#include <functional>

class Algorithm
{
public:
    Algorithm(std::string const &name);
    virtual ~Algorithm();

    virtual long Run(std::list<std::list<int>> &cliques) = 0;
    virtual void Run() {}

    void SetName(std::string const &name);
    std::string GetName() const;

    void AddCallBack(std::function<void(std::list<int> const&)> callback);

    void ExecuteCallBacks(std::list<int> const &vertexSet) const;

    void SetQuiet(bool const quiet);
    bool GetQuiet() const;

private:
    std::string m_sName;
    bool m_bQuiet;
    std::vector<std::function<void(std::list<int> const&)>> m_vCallBacks;
};

#endif //ALGORITHM_H
