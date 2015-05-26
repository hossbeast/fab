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
#include <string.h>

#include "listwise.h"
#include "listwise/lstack.h"

#include "args.h"
#include "args.def.h"
#include "params.h"
#include "selector.h"
#include "selector.def.h"

#include "logs.h"
#include "macros.h"
#include "map.h"

#define restrict __restrict

static map * o_fabricationsmap;
static map * o_fabricationxsmap;
static map * o_fabricationnsmap;
static map * o_invalidationsmap;
static map * o_discoveriesmap;
static map * o_inspectionsmap;
static map * o_queriesmap;

//
// static
//

static int select_exec(selector * s, gn * g)
{
  if(s->mode == '+')
  {
    if(s->lists & SELECTOR_FABRICATE)
      fatal(map_set, o_fabricationsmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_FABRICATEX)
      fatal(map_set, o_fabricationxsmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_FABRICATEN)
      fatal(map_set, o_fabricationnsmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_INVALIDATE)
      fatal(map_set, o_invalidationsmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_DISCOVERY)
      fatal(map_set, o_discoveriesmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_INSPECT)
      fatal(map_set, o_inspectionsmap, MM(g), 0, 0, 0);
    if(s->lists & SELECTOR_QUERY)
      fatal(map_set, o_queriesmap, MM(g), 0, 0, 0);
  }
  else if(s->mode == '-')
  {
    if(s->lists & SELECTOR_FABRICATE)
      map_delete(o_fabricationsmap, MM(g));
    if(s->lists & SELECTOR_FABRICATEX)
      map_delete(o_fabricationxsmap, MM(g));
    if(s->lists & SELECTOR_FABRICATEN)
      map_delete(o_fabricationnsmap, MM(g));
    if(s->lists & SELECTOR_INVALIDATE)
      map_delete(o_invalidationsmap, MM(g));
    if(s->lists & SELECTOR_DISCOVERY)
      map_delete(o_discoveriesmap, MM(g));
    if(s->lists & SELECTOR_INSPECT)
      map_delete(o_inspectionsmap, MM(g));
    if(s->lists & SELECTOR_QUERY)
      map_delete(o_queriesmap, MM(g));
  }

  finally : coda;
};

//
// public
//

int selector_process(selector * const s, int id, const ff_parser * const ffp, map * const tmap, lwx *** stax, size_t * staxa, size_t staxp)
{
	char space[128];

	logf(L_SELECT, "%s", selector_string(s, space, sizeof(space)));

	int l = 0;
	if(s->s[0] == '[')
	{
		// parse list expression
		ff_file * vff = 0;
		fatal(ff_list_parse, ffp, s->s, strlen(s->s), id, &vff);

		int pn = staxp;
		fatal(list_resolveflat, vff->ffn->statements[0], tmap, ffp->gp, stax, staxa, &staxp, 1, 0);

		int y;
		LSTACK_ITERATE((*stax)[pn], y, go);
		if(go)
		{
			char * rv;
			int rl;
			uint8_t rt;
			fatal(lstack_readrow, (*stax)[pn], 0, y, &rv, &rl, &rt, 0, 0, 0, 0);

			gn * g = 0;
			if(rt)
			{
				g = *(void**)rv;
			}
			else
			{
				if(s->base == SELECTOR_BASE_CWD)
					fatal(gn_lookup, rv, rl, g_params.cwd, g_params.cwdl, &g);
				else if(s->base == SELECTOR_BASE_FABFILE_DIR)
					fatal(gn_lookup, rv, rl, g_args->init_fabfile_path->abs_dir, g_args->init_fabfile_path->abs_dirl, &g);
			}

			if(g)
			{
				logf(L_SELECT, " > %s", g->idstring);
				fatal(select_exec, s, g);
				l++;
			}
		}
		LSTACK_ITEREND;
	}
	else
	{
		gn * g = 0;
		if(s->base == SELECTOR_BASE_CWD)
			fatal(gn_lookup, s->s, s->sl, g_params.cwd, g_params.cwdl, &g);
		else if(s->base == SELECTOR_BASE_FABFILE_DIR)
			fatal(gn_lookup, s->s, s->sl, g_args->init_fabfile_path->abs_dir, g_args->init_fabfile_path->abs_dirl, &g);

		if(g)
		{
			logf(L_SELECT, " > %s", g->idstring);
			fatal(select_exec, s, g);
			l++;
		}
	}

	if(l == 0)
	{
		failf(FAB_NOSELECT, "selector : %s matches nothing", s->s);
	}

	finally : coda;
}

int selector_init()
{
	fatal(map_create, &o_fabricationsmap, 0);
	fatal(map_create, &o_fabricationxsmap, 0);
	fatal(map_create, &o_fabricationnsmap, 0);
	fatal(map_create, &o_invalidationsmap, 0);
	fatal(map_create, &o_discoveriesmap, 0);
	fatal(map_create, &o_inspectionsmap, 0);
	fatal(map_create, &o_queriesmap, 0);

	finally : coda;
}

int selector_finalize(handler_context * const restrict ctx)
{
	fatal(map_keysx, o_fabricationsmap , &ctx->fabrications , &ctx->fabricationsl , MAP_DEREF);
	fatal(map_keysx, o_fabricationxsmap, &ctx->fabricationxs, &ctx->fabricationxsl, MAP_DEREF);
	fatal(map_keysx, o_fabricationnsmap, &ctx->fabricationns, &ctx->fabricationnsl, MAP_DEREF);
	fatal(map_keysx, o_invalidationsmap, &ctx->invalidations, &ctx->invalidationsl, MAP_DEREF);
	fatal(map_keysx, o_discoveriesmap  , &ctx->discoveries  , &ctx->discoveriesl  , MAP_DEREF);
	fatal(map_keysx, o_inspectionsmap  , &ctx->inspections  , &ctx->inspectionsl  , MAP_DEREF);
	fatal(map_keysx, o_queriesmap      , &ctx->queries      , &ctx->queriesl      , MAP_DEREF);

	finally : coda;
}

void selector_teardown()
{
	map_xfree(&o_fabricationsmap);
	map_xfree(&o_fabricationxsmap);
	map_xfree(&o_fabricationnsmap);
	map_xfree(&o_invalidationsmap);
	map_xfree(&o_discoveriesmap);
	map_xfree(&o_inspectionsmap);
	map_xfree(&o_queriesmap);
}
