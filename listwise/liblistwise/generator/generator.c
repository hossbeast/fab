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

#include <stdarg.h>
#include <inttypes.h>

#include "internal.h"

#include "generator/generator.def.h"
#include "generator/generator.tab.h"
#include "generator/generator.lex.h"
#include "generator/generator.tokens.h"
#include "generator/generator.states.h"
#include "genscan.h"

#include "xlinux.h"

#include "macros.h"

#define restrict __restrict

struct generator_parser_t
{
	void * p;		// scanner
};

///
/// static
///

static int generator_inputstr(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
	parse_param * pp = (parse_param*)xtra;

	*buf = 0;
	*bufl = 0;

	if(pp->name)
	{
		snprintf(pp->temp, sizeof(pp->temp), "%.*s", pp->namel, pp->name);
		*buf = pp->temp;
		*bufl = pp->namel;
	}

	return 0;
}

static int generator_lvalstr(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
	parse_param * pp = (parse_param*)xtra;

	*buf = 0;
	*bufl = 0;

	if(token == generator_I64 || token == generator_BREF || token == generator_HREF || token == generator_CREF || token == generator_OP)
	{
		if(token == generator_CREF || token == generator_HREF)
		{
			if(((YYSTYPE*)lval)->ref < 0x20 || ((YYSTYPE*)lval)->ref > 0x7E)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "0x%02x", ((YYSTYPE*)lval)->ref);
			else
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "0x%1$02x=%1$1c", ((YYSTYPE*)lval)->ref);
		}
		else if(token == generator_I64)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%"PRId64, ((YYSTYPE*)lval)->i64);
		else if(token == generator_BREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%d", ((YYSTYPE*)lval)->ref);
		else if(token == generator_OP)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%s", ((YYSTYPE*)lval)->op->s);

		*buf = pp->temp;
	}

	return 0;
}

static const char * generator_statename(int state)
{
	return state >= 0 ? generator_statenames[state] : "";
}

static const char * generator_tokenname(int token)
{
	return generator_tokennames[token];
}

static int operation_validate(operation * op)
{
	if((op->op->optype & LWOP_ARGS_CANHAVE) == 0)
	{
		if(op->argsl)
		{
			failf(LW_ARGSNUM, "expected: 0, actual: %d", op->argsl);
		}
	}

	if(op->op->op_validate)
	{
		fatal(op->op->op_validate, op);
	}

finally :
	XAPI_INFOF("op", "%s", op->op->s);
coda;
}

static int reduce(parse_param * pp)
{
	int r;
	if((r = generator_yyparse(pp->scanner, pp)) || pp->scanerr)
	{
		if(r == 2)
		{
			// memory exhaustion error from the parser
			tfail(perrtab_SYS, ENOMEM);
		}
		else if(pp->scanerr)
		{
			// error from the scanner
			fails(pp->scanerr, pp->error_str);
		}
		else if(pp->gramerr)
		{
			// error from the parser
			fails(LW_SYNTAX, pp->error_str);
		}
		else
		{
			// error thrown from a grammar rule (such as ENOMEM)
			fail(0);
		}
	}

finally :
	if(XAPI_UNWINDING)
	{
		if(pp->scanerr || pp->gramerr)
		{
			if(pp->namel)
				XAPI_INFOF("input", "%.*s", pp->namel, pp->name);

			XAPI_INFOF("loc", "[%d,%d - %d,%d]"
				, pp->error_loc.f_lin + 1
				, pp->error_loc.f_col + 1
				, pp->error_loc.l_lin + 1
				, pp->error_loc.l_col + 1
			);

			if(pp->gramerr)
			{
				XAPI_INFOF("token", "%s", pp->tokenstring);
			}
		}
	}
coda;
}

static int parse(generator_parser ** p, char* s, int l, char * name, int namel, generator** g, void * udata)
{
	// local parser
	generator_parser * lp = 0;

	// create state specific to this parse
	void * state = 0;

	if(!p)
		p = &lp;

	if(!*p)
		fatal(generator_mkparser, p);

	// results struct for this parse
	parse_param pp = {
		  .tokname				= generator_tokenname
		, .statename			= generator_statename
		, .inputstr				= generator_inputstr
		, .lvalstr				= generator_lvalstr
#if DEVEL
		, .udata					= udata
		, .token_token		= listwise_logging_config ? listwise_logging_config->tokens_token : 0
		, .token_would		= listwise_logging_config ? listwise_logging_config->tokens_would : 0
		, .token_log			= listwise_logging_config ? listwise_logging_config->tokens_log : 0
		, .state_token		= listwise_logging_config ? listwise_logging_config->states_token : 0
		, .state_would		= listwise_logging_config ? listwise_logging_config->states_would : 0
		, .state_log			= listwise_logging_config ? listwise_logging_config->states_log : 0
#endif
	};

	// specific exception for "shebang" line exactly at the beginning
	char * b = s;
	if(strlen(s) > 1 && memcmp(s, "#!", 2) == 0 && strstr(s, "\n"))
		b = strstr(s, "\n") + 1;

	if((state = generator_yy_scan_string(b, (*p)->p)) == 0)
		tfail(perrtab_SYS, ENOMEM);

	// results struct for this parse
	pp.scanner = (*p)->p;

	// make it available to the lexer
	generator_yyset_extra(&pp, (*p)->p);

	// invoke the parser, raise errors as necessary
	fatal(reduce, &pp);

	if(((*g) = pp.g))
	{
		pp.g = 0;

		// postprocessing
		int x;
		for(x = 0; x < (*g)->opsl; x++)
			fatal(operation_validate, (*g)->ops[x]);

		fatal(generator_description_log, *g, 0, udata);
	}

finally:
	// cleanup state for this parse
	generator_yy_delete_buffer(state, (*p)->p);
	generator_free(pp.g);
	yyu_extra_destroy(&pp);
	generator_parser_free(lp);

XAPI_INFOW("transform-string", s, l);
coda;
}

///
/// API
///

int API generator_mkparser(generator_parser ** p)
{
	fatal(xmalloc, p, sizeof(**p));

	if(generator_yylex_init(&(*p)->p) != 0)
		tfail(perrtab_SYS, ENOMEM);

	finally : coda;
}

void API generator_parser_free(generator_parser * p)
{
	if(p)
	{
		generator_yylex_destroy(p->p);
	}

	free(p);
}

void API generator_parser_xfree(generator_parser ** p)
{
	generator_parser_free(*p);
	*p = 0;
}

void API generator_free(generator* g)
{
	if(g)
	{
		int x;
		for(x = 0; x < g->argsl; x++)
		{
			free(g->args[x]->s);
			free(g->args[x]->refs.v);

			if(g->args[x]->itype == ITYPE_RE)
			{
				free(g->args[x]->re.c_pcre);
				free(g->args[x]->re.c_pcre_extra);
			}

			free(g->args[x]);
		}

		free(g->args);

		for(x = 0; x < g->opsl; x++)
		{
			int y;
			for(y = 0; y < g->ops[x]->argsl; y++)
			{
				free(g->ops[x]->args[y]->s);
				free(g->ops[x]->args[y]->refs.v);

				if(g->ops[x]->args[y]->itype == ITYPE_RE)
				{
					free(g->ops[x]->args[y]->re.c_pcre);
					free(g->ops[x]->args[y]->re.c_pcre_extra);
				}

				free(g->ops[x]->args[y]);
			}

			free(g->ops[x]->args);
			free(g->ops[x]);
		}

		free(g->ops);
	}

	free(g);
}

void API generator_xfree(generator** g)
{
	generator_free(*g);
	*g = 0;
}

int API generator_parse(generator_parser ** p, char* s, int l, generator** g)
{
	fatal(parse, p, s, l, 0, 0, g, 0);

	finally : coda;
}

int API generator_parse2(generator_parser ** p, char* s, int l, generator** g, void * udata)
{
	fatal(parse, p, s, l, 0, 0, g, udata);

	finally : coda;
}

int API generator_parse_named(generator_parser ** p, char* s, int l, char * name, int namel, generator** g)
{
	fatal(parse, p, s, l, name, namel, g, 0);

	finally : coda;
}

int API generator_parse_named2(generator_parser ** p, char* s, int l, char * name, int namel, generator** g, void * udata)
{
	fatal(parse, p, s, l, name, namel, g, udata);

	finally : coda;
}
