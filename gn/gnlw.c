#include <stdlib.h>

#include "control.h"

#include "gnlw.h"

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

static void recurse_needs(gn * n, gn ** r, int * x)
{
	int i;
	if(r)
	{
		for(i = 0; i < n->needs.l; i++)
			r[(*x)++] = n->needs.e[i];

		for(i = 0; i < n->needs.l; i++)
			recurse_needs(n->needs.e[i], r, x);
	}
	else
	{
		(*x) += n->needs.l;

		for(i = 0; i < n->needs.l; i++)
			recurse_needs(n->needs.e[i], 0, x);
	}
}

static void recurse_feeds(gn * n, gn ** r, int * x)
{
	int i;
	if(r)
	{
		for(i = 0; i < n->feeds.l; i++)
			r[(*x)++] = n->feeds.e[i];

		for(i = 0; i < n->feeds.l; i++)
			recurse_feeds(n->feeds.e[i], r, x);
	}
	else
	{
		(*x) += n->feeds.l;

		for(i = 0; i < n->feeds.l; i++)
			recurse_feeds(n->feeds.e[i], 0, x);
	}
}

///
/// public
///

int lw_string(void * o, char* prop, char ** s, int * l)
{
	*s = 0;
	*l = 0;

	if(!prop || strcmp(prop, "name") == 0)
	{
		*s = ((gn*)o)->name;
		*l = ((gn*)o)->namel;
	}
	else if(strcmp(prop, "name") == 0)
	{
		*s = ((gn*)o)->name;
		*l = ((gn*)o)->namel;
	}
	else if(strcmp(prop, "dir") == 0)
	{
		*s = ((gn*)o)->dir;
		*l = ((gn*)o)->dirl;
	}
	else if(strcmp(prop, "ext") == 0)
	{
		*s = ((gn*)o)->ext;
		*l = ((gn*)o)->extl;
	}

	return 1;
}

int lw_reflect(void * o, char* prop, void *** r, uint8_t ** rtypes, int ** rls, int * rl)
{
	int x;

printf("REFLECT : %s @ %p\n", prop, o);

/* string-based property reflection */
	if(strcmp(prop, "name") == 0)
	{
		*rl = 1;
		fatal(xmalloc, r, sizeof(char*) * (*rl));
		fatal(xmalloc, rls, sizeof(*rls[0]) * (*rl));

		fatal(xmalloc, &(((char**)(*r))[0]), ((gn*)o)->namel + 1);
		memcpy((((char**)(*r))[0]), ((gn*)o)->name, ((gn*)o)->namel);
		(*rls)[0] = ((gn*)o)->namel;
	}
	else if(strcmp(prop, "name") == 0)
	{
		*rl = 1;
		fatal(xmalloc, r, sizeof(char*) * 1);
		fatal(xmalloc, rls, sizeof(*rls[0]) * 1);

		fatal(xmalloc, &(((char**)(*r))[0]), ((gn*)o)->namel + 1);
		memcpy((((char**)(*r))[0]), ((gn*)o)->name, ((gn*)o)->namel);
		(*rls)[0] = ((gn*)o)->namel;
	}
	else if(strcmp(prop, "dir") == 0)
	{
		*rl = 1;
		fatal(xmalloc, r, sizeof(char*) * 1);
		fatal(xmalloc, rls, sizeof(*rls) * 1);

		fatal(xmalloc, &(((char**)(*r))[0]), ((gn*)o)->dirl + 1);
		memcpy((((char**)(*r))[0]), ((gn*)o)->dir, ((gn*)o)->dirl);
		(*rls)[0] = ((gn*)o)->dirl;
	}
	else if(strcmp(prop, "ext") == 0)
	{
		*rl = 1;
		fatal(xmalloc, r, sizeof(char*) * 1);
		fatal(xmalloc, rls, sizeof(*rls) * 1);

		fatal(xmalloc, &(((char**)(*r))[0]), ((gn*)o)->extl + 1);
		memcpy((((char**)(*r))[0]), ((gn*)o)->ext, ((gn*)o)->extl);
		(*rls)[0] = ((gn*)o)->extl;
	}

/* collection-based property reflection */

	else if(strcmp(prop, "ineed") == 0)
	{
		*rl = ((gn*)o)->needs.l;
		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
		{
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;
			fatal(xmalloc, &(((gn**)(*r))[x]), sizeof(gn*));
			((gn**)(*r))[x] = ((gn*)o)->needs.e[x];
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
			fatal(xmalloc, &(((gn**)(*r))[x]), sizeof(gn*));
			((gn**)(*r))[x] = ((gn*)o)->feeds.e[x];
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

	return 1;
}

void lw_destroy(void * o)
{
	/* do nothing ; listwise gets copies of pointers to gn objects
  ** and I free them all at the end
  */
}