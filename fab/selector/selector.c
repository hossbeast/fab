#include <stdio.h>

#include <listwise.h>
#include <listwise/lstack.h>

#include "selector.h"

#include "gn.h"
#include "args.h"
#include "list.h"

#include "log.h"
#include "control.h"

static map * o_fabricationsmap;
static map * o_fabricationxsmap;
static map * o_fabricationnsmap;
static map * o_invalidationsmap;
static map * o_discoveriesmap;
static map * o_inspectionsmap;

char * selector_string(const selector * const s, char * const dst, const size_t z)
{
	int l = 0;
	l += snprintf(dst + l, z - l, "{ %c", s->mode);

	uint32_t x = 1;
	while(x)
	{
		if(s->lists & x)
		{
			if(l > 3)
				l += snprintf(dst + l, z - l, "|");
			l += snprintf(dst + l, z - l, "%s", SELECTOR_STR(x));
		}
		x <<= 1;
	}

	l += snprintf(dst + l, z - l, ",%s : %s }", SELECTOR_BASE_STR(s->base), s->s);
	return dst;
}

int selector_process(selector * const s, int id, const ff_parser * const ffp, map * const tmap, lstack *** stax, int * staxa, int staxp)
{
	int select(selector * s, gn * g)
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
		}

		finally : coda;
	};

	char space[128];

	log(L_SELECT, "%s", selector_string(s, space, sizeof(space)));

	int l = 0;
	if(s->s[0] == '[')
	{
		// parse list expression
		ff_file * vff = 0;
		fatal(ff_list_parse, ffp, s->s, strlen(s->s), id, &vff);

		if(vff)
		{
			int pn = staxp;
			fatal(list_resolveflat, vff->ffn->statements[0], tmap, ffp->gp, stax, staxa, &staxp, 1, 0);

			int y;
			LSTACK_ITERATE((*stax)[pn], y, go);
			if(go)
			{
				gn * g = 0;
				if((*stax)[pn]->s[0].s[y].type)
				{
					g = *(void**)(*stax)[pn]->s[0].s[y].s;
				}
				else
				{
					if(s->base == SELECTOR_BASE_CWD)
						fatal(gn_lookup, (*stax)[pn]->s[0].s[y].s, g_args.cwd, &g);
					else if(s->base == SELECTOR_BASE_FABFILE_DIR)
						fatal(gn_lookup, (*stax)[pn]->s[0].s[y].s, g_args.init_fabfile_path->abs_dir, &g);
				}

				if(g)
				{
					log(L_SELECT, " > %s", g->idstring);
					fatal(select, s, g);
					l++;
				}
			}
			LSTACK_ITEREND;
		}
		else
		{
			qfail();
		}
	}
	else
	{
		gn * g = 0;
		if(s->base == SELECTOR_BASE_CWD)
			fatal(gn_lookup, s->s, g_args.cwd, &g);
		else if(s->base == SELECTOR_BASE_FABFILE_DIR)
			fatal(gn_lookup, s->s, g_args.init_fabfile_path->abs_dir, &g);

		if(g)
		{
			log(L_SELECT, " > %s", g->idstring);
			fatal(select, s, g);
			l++;
		}
	}

	if(l == 0)
	{
		log(L_WARN, "selector : %s matches nothing", s->s);
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

	finally : coda;
}

int selector_finalize(
	  gn **** fabrications, int * fabricationsl
	, gn **** fabricationxs, int * fabricationxsl
	, gn **** fabricationns, int * fabricationnsl
	, gn **** invalidations, int * invalidationsl
	, gn **** discoveries, int * discoveriesl
	, gn **** inspections, int * inspectionsl
)
{
	fatal(map_keys, o_fabricationsmap, fabrications, fabricationsl);
	fatal(map_keys, o_fabricationxsmap, fabricationxs, fabricationxsl);
	fatal(map_keys, o_fabricationnsmap, fabricationns, fabricationnsl);
	fatal(map_keys, o_invalidationsmap, invalidations, invalidationsl);
	fatal(map_keys, o_discoveriesmap, discoveries, discoveriesl);
	fatal(map_keys, o_inspectionsmap, inspections, inspectionsl);

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
}