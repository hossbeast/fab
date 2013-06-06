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
static map * o_invalidationsmap;
static map * o_inspectionsmap;
static map * o_queriesmap;

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

	l += snprintf(dst + l, z - l, " : %s }", s->s);
	return dst;
}


int selector_process(selector * const s, int id, const ff_parser * const ffp, map * const tmap, lstack *** stax, int * staxa, int staxp)
{
	int select(selector * s, gn * g)
	{
		if(s->mode == '+')
		{
			if(s->lists & SELECTOR_FABRICATE)
				fatal(map_set, o_fabricationsmap, MM(g), 0, 0);
			if(s->lists & SELECTOR_FABRICATEX)
				fatal(map_set, o_fabricationxsmap, MM(g), 0, 0);
			if(s->lists & SELECTOR_INVALIDATE)
				fatal(map_set, o_invalidationsmap, MM(g), 0, 0);
			if(s->lists & SELECTOR_INSPECT)
				fatal(map_set, o_inspectionsmap, MM(g), 0, 0);
		}
		else if(s->mode == '-')
		{
			if(s->lists & SELECTOR_FABRICATE)
				fatal(map_delete, o_fabricationsmap, MM(g));
			if(s->lists & SELECTOR_FABRICATEX)
				fatal(map_delete, o_fabricationxsmap, MM(g));
			if(s->lists & SELECTOR_INVALIDATE)
				fatal(map_delete, o_invalidationsmap, MM(g));
			if(s->lists & SELECTOR_INSPECT)
				fatal(map_delete, o_inspectionsmap, MM(g));
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
			fatal(list_resolveflat, vff->ffn->statements[0], tmap, ffp->gp, stax, staxa, &staxp, 1);

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
					fatal(gn_match, (*stax)[pn]->s[0].s[y].s, &g);
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
	}
	else
	{
		gn * g = 0;
		fatal(gn_match, s->s, &g);

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
	fatal(map_create, &o_invalidationsmap, 0);
	fatal(map_create, &o_inspectionsmap, 0);
	fatal(map_create, &o_queriesmap, 0);

	finally : coda;
}

int selector_finalize(
	  gn **** fabrications, int * fabricationsl
	, gn **** fabricationxs, int * fabricationxsl
	, gn **** invalidations, int * invalidationsl
	, gn **** inspections, int * inspectionsl
	, gn **** queries, int * queriesl
)
{
	fatal(map_keys, o_fabricationsmap, fabrications, fabricationsl);
	fatal(map_keys, o_fabricationxsmap, fabricationxs, fabricationxsl);
	fatal(map_keys, o_invalidationsmap, invalidations, invalidationsl);
	fatal(map_keys, o_inspectionsmap, inspections, inspectionsl);
	fatal(map_keys, o_queriesmap, queries, queriesl);

	selector_teardown();

	finally : coda;
}

void selector_teardown()
{
	map_xfree(&o_fabricationsmap);
	map_xfree(&o_fabricationxsmap);
	map_xfree(&o_invalidationsmap);
	map_xfree(&o_inspectionsmap);
	map_xfree(&o_queriesmap);
}
