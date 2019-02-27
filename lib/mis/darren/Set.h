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

#ifndef SET_H
#define SET_H

#include <set>
#include <vector>
#include <iostream>

class Set
{
public:
    Set() : m_State(1)
    {
    }

    ~Set() {}

    Set(Set const &that) : m_State(that.m_State)
    {
    }

    Set &operator=(Set const &that)
    {
        if (this != &that) {
            this->m_State = that.m_State;
        }
        return *this;
    }

    void PrintSummary() const
    {
        std::cout << "P(States): {";
        for (std::set<int> const &P: m_State) {
            std::cout << "[";
            for (int const p : P) {
                std::cout << p << " ";
            }
            std::cout << "] ";
        }
        std::cout << "}" << std::endl;
    }

    bool Contains(int const x) const {
        return m_State.back().find(x) != m_State.back().end();
    }

    void Insert(int const x) {
        m_State.back().insert(x);
    }

    void Remove(int const x) {
        m_State.back().erase(x);
    }

    void MoveTo(int const x, Set &other) {
        Remove(x);
        other.Insert(x);
    }

    size_t Size() const { return m_State.back().size(); }

    bool Empty() const { return m_State.back().empty(); }

    void Clear() {m_State.back().clear(); }

    // make copy of current set, and make it the working copy
    void SaveState() {
        std::set<int> temp(m_State.back());
        m_State.emplace_back(std::move(temp));
        //PrintSummary();
    }

    // you have to make sure that you don't pop_back too many times...
    void RestoreState() {
        m_State.pop_back();
        //PrintSummary();
    }

    std::set<int>::iterator begin() { return m_State.back().begin(); }
    std::set<int>::iterator end()   { return m_State.back().end();   }

    std::set<int>::const_iterator begin() const { return m_State.back().begin(); }
    std::set<int>::const_iterator end()   const { return m_State.back().end();   }

    std::set<int> const &GetStdSet() const { return m_State.back(); }
    std::set<int>       &GetStdSet()       { return m_State.back(); }

private:
    std::vector<std::set<int>> m_State;
};

#endif // SET_H
