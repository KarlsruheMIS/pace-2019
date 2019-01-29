/******************************************************************************
 * union_find.h 
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>

// A simple Union-Find datastructure implementation.
// This is sometimes also caled "disjoint sets datastructure.
class union_find 
{
        public:
                union_find(unsigned n) : m_parent(n), m_rank(n), m_n(n) {
                        for( unsigned i = 0; i < m_parent.size(); i++) {
                                m_parent[i] = i;
                                m_rank[i]   = 0;
                        }
                };
                inline void Union(unsigned lhs, unsigned rhs)
                {
                        int set_lhs = Find(lhs);
                        int set_rhs = Find(rhs);
                        if( set_lhs != set_rhs ) {
                                if( m_rank[set_lhs] < m_rank[set_rhs]) {
                                        m_parent[set_lhs] = set_rhs;
                                } else {
                                        m_parent[set_rhs] = set_lhs;
                                        if( m_rank[set_lhs] == m_rank[set_rhs] ) m_rank[set_lhs]++;
                                }
                                --m_n;
                        }
                };

                inline unsigned Find(unsigned element)
                {
                        if( m_parent[element] != element ) {
                                unsigned retValue = Find( m_parent[element] );  
                                m_parent[element] = retValue; // path compression
                                return retValue;
                        }
                        return element;
                };

                // Returns:
                //   The total number of sets.
                inline unsigned n() const
                { return m_n; };

        private:
                std::vector< unsigned > m_parent;
                std::vector< unsigned > m_rank;

                // Number of elements in UF data structure.
                unsigned m_n;
};



#endif // ifndef UNION_FIND_H

