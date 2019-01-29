/******************************************************************************
 * helpers.h
 *
 * Source of the Parallel Partitioning Program
 ******************************************************************************
 * Copyright (C) 2014 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
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

#ifndef HELPERS_ZUTE7MAJ
#define HELPERS_ZUTE7MAJ

#include <algorithm>

class helpers {
public:
        helpers() {};
        virtual ~helpers() {};

        template<typename vectortype, class Compare, class Equal> 
        void filter_duplicates( std::vector< vectortype > & input, Compare comparator_function, Equal equal_function);
};

template<typename vectortype, class Compare, class Equal> 
void helpers::filter_duplicates( std::vector< vectortype > & input, Compare comparator_function, Equal equal_function) {
        std::sort(input.begin(), input.end(), comparator_function);

        // filter duplicates / parallel edges
        typename std::vector< vectortype >::iterator it;
        it = std::unique(input.begin(), input.end(), equal_function);

        input.resize(std::distance(input.begin(),it));
        input.shrink_to_fit();
}

#endif /* end of include guard: HELPERS_ZUTE7MAJ */
