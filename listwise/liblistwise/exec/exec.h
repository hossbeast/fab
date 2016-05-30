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

#ifndef _LISTWISE_EXEC_H
#define _LISTWISE_EXEC_H

#include "xapi.h"

struct lwx;
struct transform;

#define restrict __restrict

/// listwise_exec_transform
//
// SUMMARY
//  execute the listwise transform against the lw execution context
//
// PARAMETERS
//  g        - transform
//  [init]   - initial items to write at top of list stack
//  [initls] - lenghts of items in init, 0 for strlen
//  initl    - number of items in init
//  lx       - input/output lw context
//  dump     - [debug] whether to call lstack_dump at each stage of execution
//
// REMARKS
//  if *lx is null, a new lw context is created and returned
//  otherwise, an existing lw context is reused
//
xapi listwise_exec_transform(
    struct transform * const restrict g
  , char ** const restrict init
  , size_t * const restrict initls
  , size_t initl
  , struct lwx ** restrict lx
)
  __attribute__((nonnull(1, 5)));

#undef restrict
#endif
