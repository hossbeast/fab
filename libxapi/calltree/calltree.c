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
#include "calltree.internal.h"
#include "mm.internal.h"
#include "stack.h"

#if XAPI_RUNTIME_CHECKS
# include "frame.internal.h"
#endif

#include "macros.h"
#include "memblk.def.h"

// per-thread calltree storage
__thread calltree * g_calltree;

#define restrict __restrict

//
// public
//

#if 0
void calltree_free()
{
	int x;
	for(x = 0; x < calltree_mb.blocksl; x++)
	{
		free(calltree_mb.blocks[x].s);
		memset(&calltree_mb.blocks[x], 0, sizeof(calltree_mb.blocks[0]));
	}
	free(calltree_mb.blocks);
	memset(&calltree_mb, 0, sizeof(calltree_mb));

	calltree = 0;
}
#endif

//
// api
//
API void xapi_calltree_unwind()
{
  g_calltree = 0;

#if XAPI_RUNTIME_CHECKS
  g_calltree_frames.l = 0;
#endif
}

API memblk * xapi_calltree_freeze()
{
	memblk * const mb = &mm_mb;
  calltree * const cs = g_calltree;

  // freze the root stack ; the exe stack is in its hierarchy
  stack_freeze(mb, cs->root);

  memblk_freeze(mb, &cs->exe);
  memblk_freeze(mb, &cs->root);

  return mb;
}

API void xapi_calltree_unfreeze()
{
	memblk * const mb = &mm_mb;
	calltree * const cs = g_calltree;

  memblk_unfreeze(mb, &cs->exe);
  memblk_unfreeze(mb, &cs->root);

  stack_unfreeze(mb, cs->root);
}

API calltree * xapi_calltree_thaw(char * const restrict mb)
{
	calltree * cs = (void*)mb;
  memblk_thaw(mb, &cs->exe);
  memblk_thaw(mb, &cs->root);

  stack_thaw(mb, cs->root);

  return cs;
}
