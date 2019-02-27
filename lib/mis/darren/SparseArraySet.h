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

#ifndef SPARSE_ARRAY_SET_H
#define SPARSE_ARRAY_SET_H

#include <set>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>

class SparseArraySet
{
public:
    SparseArraySet(size_t const size) : m_Elements(size, -1), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
    }

    SparseArraySet() : m_Elements(), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
    }

    ~SparseArraySet() {}

    void Resize(size_t const size)
    {
        m_Elements.resize(size, -1);
    }

    void InitializeFromAdjacencyArray(std::vector<std::vector<int>> const &adjacencyArray, int const vertex)
    {
        Resize(adjacencyArray[vertex].size());
        for (int const neighbor : adjacencyArray[vertex]) {
            Insert(neighbor);
        }
    }

    void PrintSummary() const
    {
        std::cout << "Array[" << m_iBegin << ":" << m_iEnd << "] : ";
        for (int const p : *this) {
            std::cout << p << (Contains(p)? "":"<-inconsistency") << " ";
        }
        std::cout << std::endl;
    }

    bool Contains(int const x) const {
        for (int const value : *this) {
            if (value == x) return true;
        }
        return false;
    }

    // Inserts are not allowed after saving state, as it is currently not supported.
    void Insert(int const x) {
        if (Contains(x)) return;
        assert(!m_bRemoved); // not allowed to insert and remove when saving states
        if (!m_States.empty()) m_bInserted = true;
        m_iEnd++;
        m_Elements[m_iEnd] = x;
    }

    void Remove(int const x) {
        for (int index = m_iBegin; index <= m_iEnd; index++) {
            if (m_Elements[index] == x) {
                if (!m_States.empty()) m_bRemoved = true;
                assert(!m_bInserted); // not allowed to insert and remove when saving states
                m_Elements[index] = m_Elements[m_iEnd];
                m_Elements[m_iEnd] = x;
                m_iEnd--;
                return;
            }
        }
        return;
    }

    void MoveTo(int const x, SparseArraySet &other) {
        Remove(x);
        other.Insert(x);
    }

    size_t Size()  const { return m_iEnd - m_iBegin + 1; }
    bool   Empty() const { return m_iEnd < m_iBegin; }

    std::vector<int>::iterator begin() { return m_Elements.begin() + m_iBegin;   }
    std::vector<int>::iterator end()   { return m_Elements.begin() + m_iEnd + 1; }

    std::vector<int>::const_iterator begin() const { return m_Elements.begin() + m_iBegin;   }
    std::vector<int>::const_iterator end()   const { return m_Elements.begin() + m_iEnd + 1; }

    int At(size_t const index) const
    {
        return m_Elements[index];
    }

    int operator[](size_t const index) const { return At(index); }


    void SaveState()
    {
////        std::cout << "Saving State" << std::endl << std::flush;
        m_States.push_back(std::make_pair(m_iBegin, m_iEnd));
    }

    void RestoreState()
    {
////        std::cout << "Restoring State" << std::endl << std::flush;
        std::pair<int,int> const &range(m_States.back());
        m_iBegin = range.first;
        m_iEnd   = range.second;
        m_States.pop_back();
////        std::cout << "#States = " << m_States.size() << std::endl << std::flush;
        if (m_States.empty()) {
            m_bRemoved  = false;
            m_bInserted = false;
        }
    }

    void Clear()
    {
        m_iBegin = 0;
        m_iEnd = -1;
    }

    static bool Test() {
        std::cout << "SparseArraySet: ";
        SparseArraySet testSet(3);
        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);

        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: SparseArraySet failed ContainsTest" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: SparseArraySet failed RemoveFirst Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: SparseArraySet failed RemoveAdd Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: SparseArraySet failed RemoveLast Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Remove(1);
        if (testSet.Contains(1) || testSet.Size() != 2) {
            std::cout << "FAILED: SparseArraySet failed RemoveMiddle Test" << std::endl;
            return false;
        }

        testSet.Insert(1);
        testSet.Remove(1);
        testSet.Remove(2);
        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Contains(1) || testSet.Contains(2) || testSet.Size() != 0 || !testSet.Empty()) {
            std::cout << "FAILED: SparseArraySet failed RemoveAll Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: SparseArraySet failed InsertAll Test" << std::endl;
            return false;
        }

        testSet.SaveState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: SparseArraySet failed SaveState Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 2) {
            std::cout << "FAILED: SparseArraySet failed RemoveAfterSave Test" << std::endl;
            return false;
        }

        testSet.RestoreState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: SparseArraySet failed RestoreState Test" << std::endl;
            return false;
        }

        std::cout << "PASSED!" << std::endl << std::flush;
        return true;
    }

private:
    std::vector<int> m_Elements;
    int m_iBegin;
    int m_iEnd;
    std::vector<std::pair<int,int>> m_States;
    bool m_bInserted;
    bool m_bRemoved;
};

#endif // SPARSE_ARRAY_SET_H
