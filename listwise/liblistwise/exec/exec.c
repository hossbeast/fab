/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#include <string.h>

#include "internal.h"

#if SANITY
#include "sanity.h"
#endif

#include "xlinux.h"

#include "macros.h"

#define restrict __restrict

///
/// static
///

static int exec_generator(
	  generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
	, void * udata
)
{
	pstring * ps = 0;
	pstring * ps1 = 0;

	// ovec workspace
	int* ovec = 0;
	int ovec_len = 0;

#if SANITY
	// sanity
	sanityblock * sb = 0;
#endif

	// list stack allocation
	if(!*lx)
		fatal(lwx_alloc, lx);

	// write init elements to the top of list stack
	int x;
	for(x = 0; x < initl; x++)
	{
		fatal(lstack_addw, *lx, init[x], initls ? initls[x] : strlen(init[x]));
	}

	// write initial generator args at top of list stack
	for(x = 0; x < g->argsl; x++)
	{
		fatal(lstack_addw, *lx, g->args[x]->s, g->args[x]->l);
	}

	// the initial state of selection/windows is all
	(*lx)->sel.active_era++;
	(*lx)->sel.staged_era++;
	(*lx)->win.active_era++;
	(*lx)->win.staged_era++;

	if(lw_would_exec())
	{
		fatal(generator_canon_pswrite, g, &ps);
		lw_log_exec(" >>      %.*s", (int)ps->l, ps->s);
	}

#if SANITY
	if(listwise_sanity)
	{
		fatal(sanityblock_create, &sb);
		fatal(sanity, *lx, sb, udata);
	}
#endif

	int sel_activate = 0;
	int sel_aggregate = 0;
	int win_activate = 0;
	int win_aggregate = 0;

	// execute operations
	for(x = 0; x < g->opsl; x++)
	{
		// possibly override activation settings (y-ish operators)
		if(g->ops[x]->op->optype & LWOPT_ACTIVATION_OVERRIDE)
		{
			sel_activate = !!(g->ops[x]->op->optype & LWOPT_SELECTION_ACTIVATE);
			win_activate = !!(g->ops[x]->op->optype & LWOPT_WINDOWS_ACTIVATE);
		}

		// possibly activate staged selection
		if(!(g->ops[x]->op->optype & LWOPT_SELECTION_AGGREGATE) && !sel_aggregate)
		{
			if(sel_activate)
				fatal(lstack_selection_activate, *lx);

			sel_activate = 0;
			(*lx)->sel.staged_era++;
		}

		// possibly activate staged windows
		if(!(g->ops[x]->op->optype & LWOPT_WINDOWS_AGGREGATE) && !win_aggregate)
		{
			if(win_activate)
				fatal(lstack_windows_activate, *lx);

			win_activate = 0;
			(*lx)->win.staged_era++;
		}

		// log the current lstack
		if(lw_would_exec())
		{
			fatal(lstack_description_pswrite, *lx, &ps);
			lw_log_exec("%.*s", (int)ps->l, ps->s);
		}

		// log this operation
		if(lw_would_exec())
		{
extern int operation_canon_pswrite(operation * const oper, uint32_t sm, pstring ** restrict ps);

			fatal(operation_canon_pswrite, g->ops[x], 0, &ps);
			fatal(listwise_lwop_pswrite, g->ops[x]->op->optype, 1, &ps1);

			lw_log_exec("");
			lw_log_exec(" >> [%2d] %.*s%*s%.*s", x, (int)ps->l, ps->s, MAX(35 - ps->l, 0), "", (int)ps1->l, ps1->s);
		}

		// execute the operation
		if((*lx)->l || (g->ops[x]->op->optype & LWOPT_EMPTYSTACK_YES))
			if(g->ops[x]->op->op_exec)
				fatal(g->ops[x]->op->op_exec, g->ops[x], *lx, &ovec, &ovec_len, udata);

#if SANITY
		if(listwise_sanity)
			fatal(sanity, *lx, sb, udata);
#endif

		// age active windows
		if(g->ops[x]->op->optype & LWOPT_WINDOWS_RESET)
			(*lx)->win.active_era++;

		// age active selections
		if(g->ops[x]->op->optype & LWOPT_SELECTION_RESET)
			(*lx)->sel.active_era++;

		// save activation settings, IF it specifies both STAGES and ACTIVATE
		//  y-ish operators specify only ACTIVATE
		if((g->ops[x]->op->optype & (LWOPT_SELECTION_ACTIVATE | LWOPT_SELECTION_STAGE)) == (LWOPT_SELECTION_ACTIVATE | LWOPT_SELECTION_STAGE))
			sel_activate = 1;
		if((g->ops[x]->op->optype & (LWOPT_WINDOWS_ACTIVATE | LWOPT_WINDOWS_STAGE)) == (LWOPT_WINDOWS_ACTIVATE | LWOPT_WINDOWS_STAGE))
			win_activate = 1;

		// save aggregation settings
		sel_aggregate = !!(g->ops[x]->op->optype & LWOPT_SELECTION_AGGREGATE);
		win_aggregate = !!(g->ops[x]->op->optype & LWOPT_WINDOWS_AGGREGATE);
	}

	// possibly activate staged selection
	if(sel_activate)
		fatal(lstack_selection_activate, *lx);

	// possibly activate staged windows
	if(win_activate)
		fatal(lstack_windows_activate, *lx);

	if(g->opsl && lw_would_exec())
	{
		fatal(lstack_description_pswrite, *lx, &ps);
		lw_log_exec("%.*s", (int)ps->l, ps->s);
	}

finally:
	free(ovec);
	psfree(ps);
	psfree(ps1);
#if SANITY
	sanityblock_free(sb);
#endif
coda;
}

///
/// API
///

int API listwise_exec_generator(
	  generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
)
{
	xproxy(exec_generator, g, init, initls, initl, lx, 0);
}

int API listwise_exec_generator2(
	  generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
	, void * udata
)
{
	xproxy(exec_generator, g, init, initls, initl, lx, udata);
}

int API listwise_exec(
    char * const restrict s
  , int l
  , char ** const restrict init
  , int * const restrict initls
  , const int initl
  , lwx ** restrict lx
)
{
	// generator
	generator* g = 0;

	fatal(generator_parse, 0, s, l, &g);
	fatal(exec_generator, g, init, initls, initl, lx, 0);

finally:
	generator_free(g);
coda;
}
