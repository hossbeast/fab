/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include "listwise/internal.h"
#include "liblistwise_control.h"

#if SANITY
#include "sanity.h"
#endif

#include "xmem.h"

#define restrict __restrict

int API listwise_exec_generator(
	  const generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
	, int dump
)
{
	// ovec workspace
	int* ovec = 0;
	int ovec_len = 0;

#if SANITY
	// sanity
	sanityblock * sb = 0;
#endif

	// system implemented operators
	operator * oop = op_lookup("o", 1);
	operator * yop = op_lookup("y", 1);
	operator * syop = op_lookup("sy", 2);
	operator * wyop = op_lookup("wy", 2);

	// list stack allocation
	if(!*lx)
		fatal(xmalloc, lx, sizeof(*lx[0]));

	// write init elements to top of list stack
	int x;
	for(x = 0; x < initl; x++)
	{
		fatal(lstack_write, *lx, 0, x, init[x], initls[x]);
	}

	// write initial generator args at top of list stack
	for(x = 0; x < g->argsl; x++)
	{
		fatal(lstack_write, *lx, 0, x + initl, g->args[x]->s, g->args[x]->l);
	}

	// the initial state of the selection is all
	(*lx)->sel.active_era++;
	(*lx)->sel.staged_era++;
	(*lx)->win.active_era++;
	(*lx)->win.staged_era++;

	if(dump)
		lstack_dump(*lx);

#if SANITY
	if(listwise_sanity)
	{
		fatal(sanityblock_create, &sb);
		fatal(sanity, *lx, sb);
	}
#endif

	// execute operations
	for(x = 0; x < g->opsl; x++)
	{
		int isor = g->ops[x]->op == oop;
		while(g->ops[x]->op == oop)
		{
			if(++x == g->opsl)
				break;
		}

		if(!isor)
		{
			// possibly activate windows staged by the previous operator
			if(x && ((g->ops[x-1]->op->optype & LWOP_WINDOWS_ACTIVATE) == LWOP_WINDOWS_ACTIVATE))
			{
				fatal(lstack_windows_activate, *lx);
			}

			// possibly activate selections staged by the previous operator
			if(x && ((g->ops[x-1]->op->optype & LWOP_SELECTION_ACTIVATE) == LWOP_SELECTION_ACTIVATE))
			{
				fatal(lstack_sel_activate, *lx);
			}
		}

		if(x && dump)
		{
			lstack_dump(*lx);
		}

		if(g->ops[x]->op != yop && g->ops[x]->op != wyop && !isor)
			(*lx)->win.staged_era++;

		if(g->ops[x]->op != yop && g->ops[x]->op != syop && !isor)
			(*lx)->sel.staged_era++;

		if(dump)
		{
			dprintf(listwise_info_fd, "\n");

			char buf[128];
			size_t z = generator_operation_snwrite(buf, sizeof(buf), g->ops[x], 0);
			dprintf(listwise_info_fd, " >> %.*s\n", (int)z, buf);
		}

		if(g->ops[x]->op == yop || g->ops[x]->op == wyop)
			fatal(lstack_windows_activate, *lx);

		if(g->ops[x]->op == yop || g->ops[x]->op == syop)
			fatal(lstack_sel_activate, *lx);

		// execute the operator
		if(((*lx)->l || (g->ops[x]->op->optype & LWOP_EMPTYSTACK_YES) == LWOP_EMPTYSTACK_YES) && g->ops[x]->op->op_exec)
			fatal(g->ops[x]->op->op_exec, g->ops[x], *lx, &ovec, &ovec_len);

#if SANITY
		if(listwise_sanity)
			fatal(sanity, *lx, sb);
#endif

		if((g->ops[x]->op->optype & LWOP_WINDOWS_RESET) == LWOP_WINDOWS_RESET)
			(*lx)->win.active_era++;

		if((g->ops[x]->op->optype & LWOP_SELECTION_RESET) == LWOP_SELECTION_RESET)
			(*lx)->sel.active_era++;
	}

	if(dump)
	{
		// possibly activate windows staged by the previous operator
		if(x && (g->ops[x-1]->op->optype & LWOP_WINDOWS_ACTIVATE) == LWOP_WINDOWS_ACTIVATE)
		{
			fatal(lstack_windows_activate, *lx);
		}
	}

	// possibly activate selections staged by the previous operator
	if(x && (g->ops[x-1]->op->optype & LWOP_SELECTION_ACTIVATE) == LWOP_SELECTION_ACTIVATE)
	{
		fatal(lstack_sel_activate, *lx);
	}

	if(dump && g->opsl)
		lstack_dump(*lx);

	(*lx)->win.active_era++;	// age active windows

	// clear string props set with the fx operator
	for(x = 0; x < object_registry.l; x++)
		xfree(&object_registry.e[x]->string_property);

finally:
	free(ovec);
#if SANITY
	sanityblock_free(sb);
#endif
coda;
}

int listwise_exec(
    char * const restrict s
  , int l
  , char ** const restrict init
  , int * const restrict initls
  , const int initl
  , lwx ** restrict lx
)
{
	// generator parser
	generator_parser* p = 0;

	// generator
	generator* g = 0;

	if(generator_mkparser(&p) != 0)
		fail("mkparser failed\n");

	if(generator_parse(p, s, l, &g) != 0)
		fail("parse failed\n");

	if(listwise_exec_generator(g, init, initls, initl, lx, 0) != 0)
		fail("listwise_exec_generator failed");

	generator_free(g);
	generator_parser_free(p);

	finally : coda;
}
