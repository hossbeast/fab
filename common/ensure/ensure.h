/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _ENSURE_H
#define _ENSURE_H

#include "xapi.h"

#define restrict __restrict

/// ensure
//
// SUMMARY
//
// PARAMETERS
//  target - 
//  es     - element size
//  len    - required number of elements
//  ac     - pointer to allocated size in elements
//
xapi ensure(void * target, size_t es, size_t len, size_t * const restrict ac);

/// ensure2
//
// SUMMARY
//  ensure with the ability to specify the seed
//
xapi ensure2(void * target, size_t es, size_t len, size_t * const restrict ac, size_t seed);

#undef restrict
#endif
