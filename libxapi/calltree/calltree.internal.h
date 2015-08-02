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

#ifndef _XAPI_CALLTREE_INTERNAL_H
#define _XAPI_CALLTREE_INTERNAL_H

#include <stdint.h>

#include "calltree.h"

/*

calltree definition ; libxapi-visibility

The calltree tracks program execution from the first UNWIND-ing call

All allocations are tracked in the calltree, and take place before entering a call frame.
Heuristic measures are employed to allocate enough resources before the first error
takes place. In the event that insufficient frames were allocated to record the entire
calltree, frames closest to the site of the error are recorded first

*/

struct calltree;  // has a stack
struct stack;     // has a set of frames and an error
struct frame;     // has a set of stacks
struct info;

#if DEVEL
/*
** set to the calltree for the executing thread on every api call
** makes it easy to access the calltree from gdb
*/
struct calltree * CT;
#endif

/// calltree
//
// SUMMARY
//  its a call tree
//
typedef struct calltree
{
  // the current execution stack
  struct stack *  exe;

  // the root execution stack
  struct stack *  root;
} calltree;

// per-thread calltrees
extern __thread calltree * g_calltree;

/// xapi_calltree_free
//
// SUMMARY
//  free the calltree for this thread with free semantics
//
void calltree_free();

#undef restrict
#endif
