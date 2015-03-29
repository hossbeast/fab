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

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <inttypes.h>

#include "ffn.h"

#include "ff.parse.h"
#include "ff/ff.tokens.h"

#include "logs.h"
#include "global.h"
#include "xlinux.h"
#include "macros.h"
#include "args.h"
#include "cksum.h"
#include "dirutil.h"

#define restrict __restrict

//
// [[ static ]]
//

static int parse_transform(ff_node * n, transform_parser * gp)
{
	// render transform-string
	int x;
	for(x = 0; x < n->listl; x++)
	{
		// n->list[x]->type == FFN_WORD
		fatal(pscatw, &n->text, n->list[x]->text->s, n->list[x]->text->l);
	}	

	fatal(transform_parse, &gp, n->text->s, n->text->l, &n->transform);

finally :
	XAPI_INFOF("location", "[%d,%d - %d,%d]", n->loc.f_lin + 1, n->loc.f_col + 1, n->loc.l_lin + 1, n->loc.l_col + 1);
coda;
}

/// [[ api/public ]]

int ffn_mknode(ff_node ** const restrict n, const yyu_location * const restrict loc, uint32_t type, ...)
{
	// allocate new node
	fatal(xmalloc, n, sizeof(**n));
	(*n)->type = type;

	// copy location
	memcpy(&(*n)->loc, loc, sizeof((*n)->loc));

	// pull nodetype-specific params off the stack
	va_list va;
	va_start(va, type);
	if(type == FFN_STMTLIST)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// statements
	}
	else if(type == FFN_ONCEBLOCK)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);
	}
	else if(type == FFN_DEPENDENCY)
	{
		(*n)->needs									= va_arg(va, ff_node*);
		(*n)->feeds									= va_arg(va, ff_node*);
		(*n)->flags									= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_INVOCATION)
	{
		(*n)->module								= va_arg(va, ff_node*);
		(*n)->chain[0]							= va_arg(va, ff_node*);	// varsettings
		(*n)->scope									= va_arg(va, ff_node*);
		(*n)->flags									= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_VARREF)
	{
		(*n)->name									= va_arg(va, ff_node*);
		(*n)->flags									= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_NOFILE)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// parts
	}
	else if(type == FFN_FORMULA)
	{
		(*n)->targets_0							= va_arg(va, ff_node*);
		(*n)->targets_1							= va_arg(va, ff_node*);
		(*n)->command								= va_arg(va, ff_node*);
		(*n)->flags									= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_VARASSIGN)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->definition						= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARXFM_ADD)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->definition						= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARXFM_SUB)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->definition						= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARXFM_LW)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->transform_node				= va_arg(va, ff_node*);
		(*n)->transform_list_node		= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARLOCK)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->definition						= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARLINK)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// vars
		(*n)->definition						= va_arg(va, ff_node*);
	}
	else if(type == FFN_TRANSFORM)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// 
	}
	else if(type == FFN_LIST)
	{
		(*n)->chain[0]							= va_arg(va, ff_node*);	// elements
		(*n)->transform_node				= va_arg(va, ff_node*);
		(*n)->transform_list_node		= va_arg(va, ff_node*);
		(*n)->flags 								= (uint32_t)va_arg(va, int); 
	}
	else if(type == FFN_WORD)
	{
		fatal(psloadw, &(*n)->text, (*n)->loc.s, (*n)->loc.e - (*n)->loc.s);
	}
	else if(type == FFN_LF)
	{
		fatal(psloadw, &(*n)->text, (*n)->loc.s, (*n)->loc.e - (*n)->loc.s);
	}
	else
	{
		fprintf(stderr, "unknown type : %s\n", FFN_STRING(type));
	}
	va_end(va);

	finally : coda;
}

ff_node* ffn_addchain(ff_node * a, ff_node * const b)
{
	ff_node* i = a;
	while(a->next)
		a = a->next;

	a->next = b;
	return i;
}

int ffn_postprocess(ff_node * const n, struct ff_file * const ff, transform_parser * const gp)
{
	if(n)
	{
		// convert chains to lists
		int len = 0;
		ff_node* t = n->chain[0];
		while(t)
		{
			fatal(ffn_postprocess, t, ff, gp);

			t = t->next;
			len++;
		}

		if(len)
			fatal(xmalloc, &n->list, sizeof(*n->list) * len);

		n->listl = 0;
		t = n->chain[0];
		while(t)
		{
			n->list[n->listl++] = t;
			t = t->next;
		}

		// populate location
		n->loc.ff = ff;

		// calculate length
		n->loc.l = n->loc.e - n->loc.s;

		// parse transform strings
		if(n->type == FFN_TRANSFORM)
			fatal(parse_transform, n, gp);

		// create text
		if(n->type == FFN_NOFILE)
		{
			fatal(psloadw, &n->text, "/..", 3);

			int x;
			for(x = 0; x < n->listl; x++)
				fatal(pscatf, &n->text, "/%s", n->list[x]->text->s);
		}

		int x;
		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
			fatal(ffn_postprocess, n->nodes_owned[x], ff, gp);
	}

	finally : coda;
}

void ffn_free(ff_node * const ffn)
{
	if(ffn)
	{
		int x;
		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
			ffn_free(ffn->nodes_owned[x]);
/*
		for(x = 0; x < ffn->listl; x++)
			ffn_free(ffn->list[x]);
*/
		free(ffn->list);

		if(ffn->type == FFN_TRANSFORM)
			transform_free(ffn->transform);
	}

	free(ffn);
}

void ffn_xfree(ff_node ** const ffn)
{
	ffn_free(*ffn);
	*ffn = 0;
}

void ffn_dump(ff_node * const root)
{
#if DEVEL
	void dump(ff_node * const ffn, int lvl)
	{
		int x;
		if(ffn)
		{
			logf(L_FFTREE, "%*s%-25s%*s @ [%3d,%3d - %3d,%3d]"
				, lvl * 2, ""
				, FFN_STRING(ffn->type)
				, 70 - MIN(((lvl * 2) + 20), 70)
				, ""
				, ffn->loc.f_lin + 1
				, ffn->loc.f_col + 1
				, ffn->loc.l_lin + 1
				, ffn->loc.l_col + 1
			);

			if(ffn->type == FFN_STMTLIST)
			{
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "statements", ffn->statementsl
				);
				for(x = 0; x < ffn->statementsl; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			else if(ffn->type == FFN_ONCEBLOCK)
			{
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "statements", ffn->statementsl
				);
				for(x = 0; x < ffn->statementsl; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			else if(ffn->type == FFN_DEPENDENCY)
			{
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_WEAK ? "weak" : ffn->flags & FFN_BRIDGE ? "bridge" : "strong"
				);
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"
					, ffn->flags & FFN_MULTI ? "multi" : "single"
				);
				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "needs"
				);
				dump(ffn->needs, lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "feeds"
				);
				dump(ffn->feeds, lvl + 1);
			}
			else if(ffn->type == FFN_FORMULA)
			{
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_DISCOVERY ? "discovery" : "fabrication"
				);
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"	, ffn->flags & FFN_SINGLE ? "single " : ffn->flags & FFN_MULTI ? "multi" : "UNKNOWN"
				);
				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "targets"
				);
				dump(ffn->targets_0, lvl + 1);
				dump(ffn->targets_1, lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "command"
				);
				dump(ffn->command, lvl + 1);
			}
			else if(ffn->type == FFN_INVOCATION)
			{
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "subcontext", ffn->flags & FFN_SUBCONTEXT ? "yes" : "no"
				);
				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "module"
				);
				dump(ffn->module, lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "scope"	
				);
				dump(ffn->scope, lvl + 1);

				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "varsettings"
					, ffn->varsettingsl
				);
				for(x = 0; x < ffn->varsettingsl; x++)
					dump(ffn->varsettings[x], lvl + 1);
			}
			else if(    ffn->type == FFN_VARASSIGN
			         || ffn->type == FFN_VARLOCK
			         || ffn->type == FFN_VARLINK
			         || ffn->type == FFN_VARXFM_ADD
			         || ffn->type == FFN_VARXFM_SUB)
			{
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "vars", ffn->varsl
				);
				for(x = 0; x < ffn->varsl; x++)
					dump(ffn->vars[x], lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "definition"
				);
				dump(ffn->definition, lvl + 1);
			}
			else if(ffn->type == FFN_VARXFM_LW)
			{
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "vars", ffn->varsl
				);
				for(x = 0; x < ffn->varsl; x++)
					dump(ffn->vars[x], lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "transform"
				);
				dump(ffn->transform_node, lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "transform-list"
				);
			}
			else if(ffn->type == FFN_LIST)
			{
				logf(L_FFTREE, "%*s  %12s : %u"
					, lvl * 2, ""
					, "flags", ffn->flags
				);
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "elements", ffn->elementsl
				);
				for(x = 0; x < ffn->elementsl; x++)
					dump(ffn->elements[x], lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "transform"
				);
				dump(ffn->transform_node, lvl + 1);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "transform-list"
				);
				dump(ffn->transform_list_node, lvl + 1);
			}
			else if(ffn->type == FFN_WORD)
			{
				logf(L_FFTREE, "%*s  %12s : '%.*s'"
					, lvl * 2, ""
					, "text", ffn->text->l, ffn->text->s
				);
			}
			else if(ffn->type == FFN_NOFILE)
			{
				logf(L_FFTREE, "%*s  %12s : '%.*s'"
					, lvl * 2, ""
					, "text", ffn->text->l, ffn->text->s
				);
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "parts", ffn->partsl
				);
				for(x = 0; x < ffn->partsl; x++)
				{
					logf(L_FFTREE, "%*s  %12s : %s"
						, lvl * 2, ""
						, "part", ffn->parts[x]->text->s
					);
				}
			}
			else if(ffn->type == FFN_TRANSFORM)
			{
				logf(L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "transform-string", ffn->text->s
				);
				logf(L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "transform-list", ffn->listl
				);
				for(x = 0; x < ffn->listl; x++)
					dump(ffn->list[x], lvl + 1);
			}
			else if(ffn->type == FFN_VARREF)
			{
				logf(L_FFTREE, "%*s  %12s : %u"
					, lvl * 2, ""
					, "flags", ffn->flags
				);

				logf(L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "name"
				);
				dump(ffn->name, lvl + 1);
			}
		}
	};

	if(log_would(L_FFTREE))
		dump(root, 0);
#endif
}
