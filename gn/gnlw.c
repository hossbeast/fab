#include <stdlib.h>

#include "control.h"

#include "gnlw.h"
#include "xmem.h"

static int lw_string(void * o, char* prop, char ** s, int * l);
static int lw_reflect(void * o, char* property, void *** r, uint8_t ** rtypes, int ** rls, int * rl);
static void lw_destroy(void * o);

listwise_object gnlw = {
	  .string		= lw_string
	, .reflect	= lw_reflect
	, .destroy	= lw_destroy
};

///
/// static
///

static void recurse_needs(gn * root, gn ** r, int * x)
{
	int logic(gn * gn, int d)
	{
		if(r)
		{
			// capture mode
			int i;
			for(i = 0; i < gn->needs.l; i++)
				r[(*x)++] = gn->needs.e[i]->B;
		}
		else
		{
			// count mode
			(*x) += gn->needs.l;
		}

		return 1;
	};

	gn_depth_traversal_nodes_needsward(root, logic);
}

static void recurse_feeds(gn * root, gn ** r, int * x)
{
	int logic(gn * gn, int d)
	{
		if(r)
		{
			// capture mode
			int i;
			for(i = 0; i < gn->feeds.l; i++)
				r[(*x)++] = gn->feeds.e[i]->B;
		}
		else
		{
			// count mode
			(*x) += gn->feeds.l;
		}

		return 1;
	};

	gn_depth_traversal_nodes_feedsward(root, logic);
}

///
/// public
///

int lw_string(void * o, char* prop, char ** s, int * l)
{
	*s = 0;
	*l = 0;

	if(!prop || strcmp(prop, "path") == 0)
	{
		*s = ((gn*)o)->path->rel;
		*l = ((gn*)o)->path->rell;
	}
	else if(strcmp(prop, "name") == 0)
	{
		*s = ((gn*)o)->path->name;
		*l = ((gn*)o)->path->namel;
	}
	else if(strcmp(prop, "dir") == 0)
	{
		*s = ((gn*)o)->path->rel_dir;
		*l = ((gn*)o)->path->rel_dirl;
	}
	else if(strcmp(prop, "ext") == 0)
	{
		*s = ((gn*)o)->path->ext;
		*l = ((gn*)o)->path->extl;
	}

	return 1;
}

int lw_reflect(void * o, char* prop, void *** r, uint8_t ** rtypes, int ** rls, int * rl)
{
	int x;

/* string-based property reflection */

	if(!prop || strcmp(prop, "path") == 0
					 || strcmp(prop, "name") == 0
					 || strcmp(prop, "dir") == 0
					 || strcmp(prop, "ext") == 0)
	{
		*rl = 1;
		fatal(xmalloc, r, sizeof(char*) * (*rl));
		fatal(xmalloc, rls, sizeof(**rls) * (*rl));

		lw_string(o, prop, (char**)&(*r)[0], &(*rls)[0]);
	}

/* collection-based property reflection */

	else if(strcmp(prop, "ineed") == 0)
	{
//gn_dump((gn*)o);
		*rl = ((gn*)o)->needs.l;
		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
		{
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;
			(*r)[x] = ((gn*)o)->needs.e[x]->B;
		}
	}
	else if(strcmp(prop, "ifeed") == 0)
	{
		*rl = ((gn*)o)->feeds.l;
		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
		{
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;
			(*r)[x] = ((gn*)o)->feeds.e[x]->A;
		}
	}
	else if(strcmp(prop, "aneed") == 0)
	{
		*rl = 0;
		recurse_needs(o, 0, rl);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		recurse_needs(o, (gn**)(*r), &x);
	}
	else if(strcmp(prop, "afeed") == 0)
	{
		*rl = 0;
		recurse_feeds(o, 0, rl);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		recurse_feeds(o, (gn**)(*r), &x);
	}

	finally : coda;
}

void lw_destroy(void * o)
{
	/*
	** do nothing ; listwise gets copies of pointers to gn objects
  ** and I free them all at the end
	*/
}
