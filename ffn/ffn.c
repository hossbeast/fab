#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ftw.h>
#include <inttypes.h>

#include "ff.h"
#include "ff.tokens.h"
#include "ff.tab.h"
#include "ff.lex.h"

#include "gn.h"

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
		n->list_l = 0;
		ff_node* t = n->chain[0];
		while(t)
		{
			flatten(t);

			t = t->next;
			n->list_l++;
		}

		if(n->list_l)
			n->list = calloc(n->list_l, sizeof(n->list[0]));

		n->list_l = 0;
		t = n->chain[0];
		while(t)
		{
			n->list[n->list_l++] = t;
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
		for(x = 0; x < n->list_l; x++)
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
		for(x = 0; x < n->list_l; x++)
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

/// [[ public ]]

/// [[ api/public ]]

ff_node* ffn_mknode(const ff_loc * const loc, size_t locz, struct ff_file * const ff, uint32_t type, ...)
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
		n->flags					= (uint8_t)va_arg(va, int);
	}
	else if(type == FFN_INVOCATION)
	{
		n->modules				= va_arg(va, ff_node*);
		n->scope					= va_arg(va, ff_node*);
		n->chain[0]				= va_arg(va, ff_node*);	// designations
		n->flags					= (uint8_t)va_arg(va, int);
	}
	else if(type == FFN_WORD)
	{
		n->text 					= va_arg(va, char*);
	}
	else if(type == FFN_FORMULA)
	{
		n->targets_0			= va_arg(va, ff_node*);
		n->targets_1			= va_arg(va, ff_node*);
		n->chain[0]				= va_arg(va, ff_node*);	// commands
		n->flags					= (uint8_t)va_arg(va, int);
	}
	else if(type == FFN_VARASSIGN)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARPUSH)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARPOP)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_VARDESIGNATE)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// vars
		n->definition			= va_arg(va, ff_node*);
	}
	else if(type == FFN_LIST)
	{
		n->chain[0]				= va_arg(va, ff_node*);	// elements
		n->generator_node	= va_arg(va, ff_node*);
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

void ffn_freenode(ff_node * const ffn)
{
	if(ffn)
	{
		int x;
		for(x = 0; x < sizeof(ffn->strings) / sizeof(ffn->strings[0]); x++)
			free(ffn->strings[x]);

		for(x = 0; x < sizeof(ffn->nodes_owned) / sizeof(ffn->nodes_owned[0]); x++)
			ffn_freenode(ffn->nodes_owned[x]);

		for(x = 0; x < ffn->list_l; x++)
			ffn_freenode(ffn->list[x]);

		free(ffn->list);

		if(ffn->type == FFN_GENERATOR)
			generator_free(ffn->generator);
	}

	free(ffn);
}

void ffn_xfreenode(ff_node ** const ffn)
{
	ffn_freenode(*ffn);
	*ffn = 0;
}

void ffn_dump(ff_node * const root)
{
	void dump(ff_node * const ffn, int lvl)
	{
		int x;
		if(ffn)
		{
			log(L_FF | L_FFTREE, "%*s%-20s%*s @ [%3d,%3d - %3d,%3d]"
				, lvl * 2, ""
				, FFN_STRING(ffn->type)
				, 70 - MIN(((lvl * 2) + 20), 70)
				, ""
				, ffn->loc.f_lin, ffn->loc.f_col, ffn->loc.l_lin, ffn->loc.l_col
			);

			if(ffn->type == FFN_STMTLIST)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "statements", ffn->statements_l
				);
				for(x = 0; x < ffn->statements_l; x++)
					dump(ffn->statements[x], lvl + 1);
			}
			if(ffn->type == FFN_DEPENDENCY)
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

				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "command", ffn->commands_l
				);
				for(x = 0; x < ffn->commands_l; x++)
					dump(ffn->commands[x], lvl + 1);
			}
			else if(ffn->type == FFN_INVOCATION)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %s"
					, lvl * 2, ""
					, "gated"	, ffn->flags & FFN_GATED ? "yes" : "no"
				);
				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "modules"
				);
				dump(ffn->modules, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "scope"	
				);
				dump(ffn->scope, lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "designations"
					, ffn->designations_l
				);
				for(x = 0; x < ffn->designations_l; x++)
					dump(ffn->designations[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "command", ffn->commands_l
				);
			}
			else if(    ffn->type == FFN_VARASSIGN
			         || ffn->type == FFN_VARPUSH
			         || ffn->type == FFN_VARPOP
			         || ffn->type == FFN_VARDESIGNATE)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "vars", ffn->vars_l
				);
				for(x = 0; x < ffn->vars_l; x++)
					dump(ffn->vars[x], lvl + 1);

				log(L_FF | L_FFTREE, "%*s  %12s :"
					, lvl * 2, ""
					, "definition"
				);
				dump(ffn->definition, lvl + 1);
			}
			else if(ffn->type == FFN_LIST)
			{
				log(L_FF | L_FFTREE, "%*s  %12s : %d"
					, lvl * 2, ""
					, "elements", ffn->elements_l
				);
				for(x = 0; x < ffn->elements_l; x++)
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
