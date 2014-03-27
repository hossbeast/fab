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

#include <stdlib.h>

#include "gnlw.h"
#include "traverse.h"

#include "control.h"

#include "args.h"
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

static int recurse_needs(gn * root, gn ** r, int * x, int useweak)
{
	int logic(gn * gn, int d)
	{
		if(r)
		{
			r[(*x)++] = gn; // capture mode
		}
		else
		{
			(*x)++;  // count mode
		}

		return 0;
	};

	if(useweak)
		return traverse_depth_bynodes_needsward_useweak_nobridge_usenofile(root, logic);
	else
		return traverse_depth_bynodes_needsward_noweak_nobridge_usenofile (root, logic);
}

static int recurse_feeds(gn * root, gn ** r, int * x, int useweak)
{
	int logic(gn * gn, int d)
	{
		if(r)
		{
			r[(*x)++] = gn; // capture mode
		}
		else
		{
			(*x)++;  // count mode
		}

		return 0;
	};

	if(useweak)
		return traverse_depth_bynodes_feedsward_useweak_nobridge_usenofile(root, logic);
	else
		return traverse_depth_bynodes_feedsward_noweak_nobridge_usenofile (root, logic);
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
		if(g_args.mode_paths == MODE_ABSOLUTE)
		{
			*s = ((gn*)o)->path->abs;
			*l = ((gn*)o)->path->absl;
		}
		else if(g_args.mode_paths == MODE_RELATIVE_FABFILE_DIR)
		{
			*s = ((gn*)o)->path->rel_fab;
			*l = ((gn*)o)->path->rel_fabl;
		}
	}
	else if(strcmp(prop, "name") == 0)
	{
		*s = ((gn*)o)->path->name;
		*l = ((gn*)o)->path->namel;
	}
	else if(strcmp(prop, "dir") == 0)
	{
		if(g_args.mode_paths == MODE_ABSOLUTE)
		{
			*s = ((gn*)o)->path->abs_dir;
			*l = ((gn*)o)->path->abs_dirl;
		}
		else if(g_args.mode_paths == MODE_RELATIVE_FABFILE_DIR)
		{
			*s = ((gn*)o)->path->rel_fab_dir;
			*l = ((gn*)o)->path->rel_fab_dirl;
		}
	}
	else if(strcmp(prop, "ext") == 0)
	{
		*s = ((gn*)o)->path->ext;
		*l = ((gn*)o)->path->extl;
	}

	return 0;
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
	else if(strcmp(prop, "ineedw") == 0)
	{
		(*rl) = 0;
		for(x = 0; x < ((gn*)o)->needs.l; x++)
		{
			if(((gn*)o)->needs.e[x]->bridge == 0)
			{
				(*rl)++;
			}
		}

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		int i = 0;
		for(x = 0; x < ((gn*)o)->needs.l; x++)
		{
			if(((gn*)o)->needs.e[x]->bridge == 0)
			{
				(*rtypes)[i] = LISTWISE_TYPE_GNLW;
				(*r)[i] = ((gn*)o)->needs.e[x]->B;
				i++;
			}
		}
	}
	else if(strcmp(prop, "ineed") == 0)
	{
		(*rl) = 0;
		for(x = 0; x < ((gn*)o)->needs.l; x++)
		{
			if((((gn*)o)->needs.e[x]->weak == 0) && (((gn*)o)->needs.e[x]->bridge == 0))
			{
				(*rl)++;
			}
		}

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		int i = 0;
		for(x = 0; x < ((gn*)o)->needs.l; x++)
		{
			if((((gn*)o)->needs.e[x]->weak == 0) && (((gn*)o)->needs.e[x]->bridge == 0))
			{
				(*rtypes)[i] = LISTWISE_TYPE_GNLW;
				(*r)[i] = ((gn*)o)->needs.e[x]->B;
				i++;
			}
		}
	}
	else if(strcmp(prop, "ifeedw") == 0)
	{
		(*rl) = 0;
		for(x = 0; x < ((gn*)o)->feeds.l; x++)
		{
			if(((gn*)o)->feeds.e[x]->bridge == 0)
			{
				(*rl)++;
			}
		}

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		int i = 0;
		for(x = 0; x < ((gn*)o)->feeds.l; x++)
		{
			if(((gn*)o)->feeds.e[x]->bridge == 0)
			{
				(*rtypes)[i] = LISTWISE_TYPE_GNLW;
				(*r)[i] = ((gn*)o)->feeds.e[x]->A;
				i++;
			}
		}
	}else if(strcmp(prop, "ifeed") == 0)
	{
		(*rl) = 0;
		for(x = 0; x < ((gn*)o)->feeds.l; x++)
		{
			if((((gn*)o)->feeds.e[x]->weak == 0) && (((gn*)o)->feeds.e[x]->bridge == 0))
			{
				(*rl)++;
			}
		}

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		int i = 0;
		for(x = 0; x < ((gn*)o)->feeds.l; x++)
		{
			if((((gn*)o)->feeds.e[x]->weak == 0) && (((gn*)o)->feeds.e[x]->bridge == 0))
			{
				(*rtypes)[i] = LISTWISE_TYPE_GNLW;
				(*r)[i] = ((gn*)o)->feeds.e[x]->A;
				i++;
			}
		}
	}
	else if(strcmp(prop, "aneedw") == 0)
	{
		*rl = 0;
		fatal(recurse_needs, o, 0, rl, 1);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		fatal(recurse_needs, o, (gn**)(*r), &x, 1);
	}
	else if(strcmp(prop, "aneed") == 0)
	{
		*rl = 0;
		fatal(recurse_needs, o, 0, rl, 0);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		fatal(recurse_needs, o, (gn**)(*r), &x, 0);
	}
	else if(strcmp(prop, "afeedw") == 0)
	{
		*rl = 0;
		fatal(recurse_feeds, o, 0, rl, 1);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		fatal(recurse_feeds, o, (gn**)(*r), &x, 1);
	}
	else if(strcmp(prop, "afeed") == 0)
	{
		*rl = 0;
		fatal(recurse_feeds, o, 0, rl, 0);

		fatal(xmalloc, r, sizeof(gn*) * (*rl));
		fatal(xmalloc, rtypes, sizeof(*rtypes[0]) * (*rl));

		for(x = 0; x < (*rl); x++)
			(*rtypes)[x] = LISTWISE_TYPE_GNLW;

		x = 0;
		fatal(recurse_feeds, o, (gn**)(*r), &x, 0);
	}

	finally : coda;
}

void lw_destroy(void * o)
{
	/*
	** do nothing ; listwise gets copies of pointers to gn objects
  ** and I free them all at shutdown
	*/
}
