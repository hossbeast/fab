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

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "macros.h"
#include "args.h"
#include "cksum.h"
#include "xstring.h"
#include "identity.h"
#include "dirutil.h"

//
// [[ static ]]
//

static void flatten(ff_node* n)
{
	if(n)
	{
		n->listl = 0;
		ff_node* t = n->chain[0];
		while(t)
		{
			flatten(t);

			t = t->next;
			n->listl++;
		}

		if(n->listl)
			n->list = calloc(n->listl, sizeof(n->list[0]));

		n->listl = 0;
		t = n->chain[0];
		while(t)
		{
			n->list[n->listl++] = t;
			t = t->next;
		}

		int x;
		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
			flatten(n->nodes_owned[x]);
	}
}

static void strmeasure(ff_node* n)
{
	if(n)
	{
		n->l = n->e - n->s;

		int x;
		for(x = 0; x < n->listl; x++)
			strmeasure(n->list[x]);

		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
			strmeasure(n->nodes_owned[x]);
	}
}

static int parse_generators(ff_node* n, generator_parser * gp)
{
	if(n)
	{
		if(n->type == FFN_GENERATOR)
		{
			if(generator_parse(gp, n->text, 0, &n->generator) == 0)
			{
				fail("failed to parse '%s' @ [%3d,%3d - %3d,%3d]"
					, n->text
					, n->loc.f_lin + 1
					, n->loc.f_col + 1
					, n->loc.l_lin + 1
					, n->loc.l_col + 1
				);
			}
		}

		int x;
		for(x = 0; x < n->listl; x++)
		{
			if(parse_generators(n->list[x], gp) == 0)
				qfail();
		}

		for(x = 0; x < sizeof(n->nodes_owned) / sizeof(n->nodes_owned[0]); x++)
		{
			if(parse_generators(n->nodes_owned[x], gp) == 0)
				qfail();
		}
	}

	finally : coda;
}

/// [[ api/public ]]

ff_node* ffn_mknode(const void * const loc, size_t locz, struct ff_file * const ff, uint32_t type, ...)
{
	ff_node* n = calloc(1, sizeof(*n));
	n->type = type;
	memcpy(&n->loc, loc, locz);
	n->loc.ff = ff;

	va_list va;
	va_start(va, type);

	n->s = va_arg(va, char*);
	n->e = va_arg(va, char*);

	if(type == FFN_STMTLIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// statements
	}
	else if(type == FFN_DEPENDENCY)
	{
		n->needs					= va_arg(va, ff_node*);
		n->feeds					= va_arg(va, ff_node*);
		n->flags					= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_INVOCATION)
	{
		n->module					= va_arg(va, ff_node*);
		n->chain[0]				= va_arg(va, ff_node*);	// designations
		n->scope					= va_arg(va, ff_node*);
		n->flags					= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_WORD)
	{
		n->text 					= va_arg(va, char*);
	}
	else if(type == FFN_NOFILE)
	{
		n->text						= va_arg(va, char*);
		n->parts					= va_arg(va, char**);
		n->partsl					= va_arg(va, int);
	}
	else if(type == FFN_FORMULA)
	{
		n->targets_0			= va_arg(va, ff_node*);
		n->targets_1			= va_arg(va, ff_node*);
		n->command				= va_arg(va, ff_node*);
		n->flags					= (uint32_t)va_arg(va, int);
	}
	else if(type == FFN_VARASSIGN)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARLOCK)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARLINK)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_LIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// elements
		n->generator_node	= va_arg(va, ff_node*);
		n->flags 					= (uint32_t)va_arg(va, int); 
	}
	else if(type == FFN_LF)
	{
		// no extra params
	}
	else
	{
		fprintf(stderr, "unknown type : %s\n", FFN_STRING(type));
		exit(0);
	}

	va_end(va);
	return n;
}

ff_node* ffn_addchain(ff_node * a, ff_node * const b)
{
	ff_node* i = a;
	while(a->next)
		a = a->next;

	a->next = b;
	return i;
}

int ffn_postprocess(ff_node * const ffn, generator_parser * const gp)
{
	// convert chains to lists
	flatten(ffn);

	// calculate string lengths
	strmeasure(ffn);

	// parse generator strings
	if(parse_generators(ffn, gp) == 0)
		qfail();

	finally : coda;
}

void ffn_free(ff_node * const ffn)
{
	if(ffn)
	{
		int x;
		for(x = 0; x < sizeof(ffn->stringlists) / sizeof(ffn->stringlists[0]); x++)
		{
			int i;
			for(i = 0; i < ffn->stringlistsl[x]; i++)
				free(ffn->stringlists[x][i]);

			free(ffn->stringlists[x]);
		}

		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
			ffn_free(ffn->nodes_owned[x]);

		for(x = 0; x < ffn->listl; x++)
			ffn_free(ffn->list[x]);

		free(ffn->list);

		if(ffn->type == FFN_GENERATOR)
			generator_free(ffn->generator);
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
	void dump(ff_node * const ffn, int lvl)
	{
		int x;
		if(ffn)
		{
			log(L_FF | L_FFTREE, "%*s%-25s%*s @ [%3d,%3d - %3d,%3d]"
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
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "statements", ffn->statementsl
				);
				for(x = 0; x < ffn->statementsl; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			else if(ffn->type == FFN_DEPENDENCY)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_WEAK ? "weak" : "strong"
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"
					, ffn->flags & FFN_MULTI ? "multi" : "single"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "needs"
				);
				dump(ffn->needs, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "feeds"
				);
				dump(ffn->feeds, lvl + 1);
			}
			else if(ffn->type == FFN_FORMULA)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "type"
					, ffn->flags & FFN_DISCOVERY ? "discovery" : "fabrication"
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "cardinality"	, ffn->flags & FFN_SINGLE ? "single " : ffn->flags & FFN_MULTI ? "multi" : "UNKNOWN"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "targets"
				);
				dump(ffn->targets_0, lvl + 1);
				dump(ffn->targets_1, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "command"
				);
				dump(ffn->command, lvl + 1);
			}
			else if(ffn->type == FFN_INVOCATION)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "subcontext", ffn->flags & FFN_SUBCONTEXT ? "yes" : "no"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "module"
				);
				dump(ffn->module, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "scope"	
				);
				dump(ffn->scope, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "varsettings"
					, ffn->varsettingsl
				);
				for(x = 0; x < ffn->varsettingsl; x++)
					dump(ffn->varsettings[x], lvl + 1);
			}
			else if(    ffn->type == FFN_VARASSIGN
			         || ffn->type == FFN_VARLOCK
			         || ffn->type == FFN_VARLINK)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "vars", ffn->varsl
				);
				for(x = 0; x < ffn->varsl; x++)
					dump(ffn->vars[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "definition"
				);
				dump(ffn->definition, lvl + 1);
			}
			else if(ffn->type == FFN_LIST)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s' (%u)"
					, lvl * 2, ""
					, "interpolation"
					, ffn->flags & FFN_WSSEP ? "ws"
					: ffn->flags & FFN_COMMASEP ? "comma"
					: "UNKNWN"
					, ffn->flags
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "elements", ffn->elementsl
				);
				for(x = 0; x < ffn->elementsl; x++)
					dump(ffn->elements[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "generator"
				);
				dump(ffn->generator_node, lvl + 1);
			}
			else if(ffn->type == FFN_WORD)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "text", ffn->text
				);
			}
			else if(ffn->type == FFN_NOFILE)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "text", ffn->text
				);
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "parts", ffn->partsl
				);
				for(x = 0; x < ffn->partsl; x++)
				{
					log(L_FF | L_FFTREE, "%*s  %12s : %s"
						, lvl * 2, ""
						, "part", ffn->parts[x]
					);
				}
			}
			else if(ffn->type == FFN_GENERATOR)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "generator-string", ffn->text
				);
			}
			else if(ffn->type == FFN_VARREF)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : '%s'"
					, lvl * 2, ""
					, "name", ffn->text
				);
			}
		}
	};

	if(log_would(L_FF | L_FFTREE))
		dump(root, 0);
}
