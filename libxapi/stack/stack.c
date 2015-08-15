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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"
#include "stack.h"
#include "error.h"
#include "frame.internal.h"
#include "mm.internal.h"
#include "errtab.h"

#include "macros.h"
#include "memblk.def.h"

// per-thread stack storage
__thread stack * APIDATA g_stack;
__thread struct stacks stacks;

#define restrict __restrict

//
// public
//

void stack_push()
{
  // save the currently unwinding stack
  stack * current = g_stack;

  // allocate a new stack
  assure(&stacks.v, &stacks.l, &stacks.a, sizeof(*stacks.v), stacks.l + 1);

  // make the new stack a child of the current stack
  g_stack = &stacks.v[stacks.l++];

#if DEVEL
	S = g_stack;
#endif

  memset(g_stack, 0, sizeof(*g_stack));
  g_stack->parent = current;
  if(current)
    current->frames.v[current->frames.l - 1].child = g_stack;
}

void stack_freeze(memblk * const restrict mb, stack * s)
{
	/*
	** etab is statically allocated and must be handled specially
	*/
	s->etab = (void*)(intptr_t)s->etab->id;
	memblk_freeze(mb, &s->msg);
	memblk_freeze(mb, &s->parent);

	int x;
	for(x = 0; x < s->frames.l; x++)
		frame_freeze(mb, &s->frames.v[x]);
  memblk_freeze(mb, &s->frames.v);
}

void stack_unfreeze(memblk * const restrict mb, stack * s)
{
	s->etab = xapi_errtab_byid((intptr_t)s->etab);
	memblk_unfreeze(mb, &s->msg);
	memblk_unfreeze(mb, &s->parent);

	memblk_unfreeze(mb, &s->frames.v);
	int x;
	for(x = 0; x < s->frames.l; x++)
		frame_unfreeze(mb, &s->frames.v[x]);
}

void stack_thaw(char * const restrict mb, stack * s)
{
	s->etab = xapi_errtab_byid((intptr_t)s->etab);
	memblk_thaw(mb, &s->msg);
	memblk_thaw(mb, &s->parent);

	memblk_thaw(mb, &s->frames.v);
	int x;
	for(x = 0; x < s->frames.l; x++)
		frame_thaw(mb, &s->frames.v[x]);
}
