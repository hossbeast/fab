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

#ifndef _WALKER_H
#define _WALKER_H

#include <fcntl.h>
#include "xapi.h"

struct node;

#define restrict __restrict

/// walker_walk
//
// SUMMARY
//  
//
// PARAMETERS
//  [root]     - 
//  [ancestor] - 
//  abspath    - absolute path to a directory
//
xapi walker_walk(struct node ** restrict root, struct node * restrict ancestor, const char * restrict abspath)
  __attribute__((nonnull(1,3)));

#endif
