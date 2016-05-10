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

#include <stdarg.h>
#include <inttypes.h>

#include "xapi.h"
#include "narrator.h"

#include "internal.h"
#include "transform.internal.h"
#include "transform/transform.def.h"
#include "transform/transform.tab.h"
#include "transform/transform.lex.h"
#include "transform/transform.tokens.h"
#include "transform/transform.states.h"
#include "genscan.internal.h"
#include "logging.internal.h"

#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

#include "macros.h"

#define restrict __restrict

typedef struct transform_parser
{
	void * p;		// scanner
} transform_parser;

///
/// static
///

static int transform_inputstr(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
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

static int transform_lvalstr(int token, void * restrict lval, struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
	parse_param * pp = (parse_param*)xtra;

	*buf = 0;
	*bufl = 0;

	if(token == transform_I64 || token == transform_BREF || token == transform_HREF || token == transform_CREF || token == transform_OP)
	{
		if(token == transform_CREF || token == transform_HREF)
		{
			if(((YYSTYPE*)lval)->ref < 0x20 || ((YYSTYPE*)lval)->ref > 0x7E)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "0x%02x", ((YYSTYPE*)lval)->ref);
			else
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "0x%1$02x=%1$1c", ((YYSTYPE*)lval)->ref);
		}
		else if(token == transform_I64)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%"PRId64, ((YYSTYPE*)lval)->i64);
		else if(token == transform_BREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%d", ((YYSTYPE*)lval)->ref);
		else if(token == transform_OP)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%s", ((YYSTYPE*)lval)->op->s);

		*buf = pp->temp;
	}

	return 0;
}

static const char * transform_statename(int state)
{
	return state >= 0 ? transform_statenames[state] : "";
}

static const char * transform_tokenname(int token)
{
	return transform_tokennames[token];
}

static xapi operation_validate(operation * op)
{
  enter;

	if((op->op->optype & LWOP_ARGS_CANHAVE) == 0)
	{
		if(op->argsl)
		{
      xapi_fail_intent();
      xapi_infof("expected", "%d", 0);
      xapi_infof("actual", "%d", op->argsl);
			fail(LISTWISE_ARGSNUM);
		}
	}

	if(op->op->op_validate)
	{
		fatal(op->op->op_validate, op);
	}

finally :
	xapi_infof("op", "%s", op->op->s);
coda;
}

static xapi reduce(parse_param * pp)
{
  enter;

	int r;
	if((r = transform_yyparse(pp->scanner, pp)) || pp->scanerr)
	{
		if(r == 2)
		{
			// memory exhaustion error from the parser
			tfail(perrtab_SYS, ENOMEM);
		}
		else if(XAPI_UNWINDING)
		{
      // fail from within a lexer or parser rule (such as ENOMEM or NXOP)
			fail(0);
		}
		else if(pp->scanerr)
		{
			// scanner invoked YYU_LFAILF
			fails(pp->scanerr, "message", pp->error_str);
		}
		else if(pp->gramerr)
		{
			// error from the parser
			fails(LISTWISE_SYNTAX, "message", pp->error_str);
		}
	}

finally :
	if(XAPI_FAILING)
	{
		if(pp->scanerr || pp->gramerr)
		{
			if(pp->namel)
				xapi_infof("input", "%.*s", pp->namel, pp->name);

			xapi_infof("loc", "[%d,%d - %d,%d]"
				, pp->error_loc.f_lin + 1
				, pp->error_loc.f_col + 1
				, pp->error_loc.l_lin + 1
				, pp->error_loc.l_col + 1
			);

			if(pp->gramerr)
			{
				xapi_infof("token", "%s", pp->tokenstring);
			}
		}
	}
coda;
}

static xapi parse(transform_parser ** restrict p, const char * const restrict s, size_t sl, char * const restrict name, transform ** const restrict g)
{
  enter;

  int token = 0;

	// local parser
	transform_parser * lp = 0;

	// create state specific to this parse
	void * state = 0;

	// transform-string
	const char * b = s;

	if(!p)
		p = &lp;

	if(!*p)
		fatal(transform_parser_create, p);

	// results struct for this parse
	parse_param pp = {
		  .tokname				= transform_tokenname
		, .statename			= transform_statename
		, .inputstr				= transform_inputstr
		, .lvalstr				= transform_lvalstr
#if DEBUG || DEVEL
    , .state_logs     = L_LISTWISE | L_STATES
    , .token_logs     = L_LISTWISE | L_TOKENS
#endif
	};

	// specific exception for "shebang" line exactly at the beginning
	if(strlen(s) > 1 && memcmp(s, "#!", 2) == 0 && strstr(s, "\n"))
		b = strstr(s, "\n") + 1;

	if((state = transform_yy_scan_string(b, (*p)->p)) == 0)
		tfail(perrtab_SYS, ENOMEM);

	// results struct for this parse
	pp.scanner = (*p)->p;

	// make it available to the lexer
	transform_yyset_extra(&pp, (*p)->p);

	// invoke the parser, raise errors as necessary
	fatal(reduce, &pp);

	if(((*g) = pp.g))
	{
		pp.g = 0;

		// postprocessing
		int x;
		for(x = 0; x < (*g)->opsl; x++)
			fatal(operation_validate, (*g)->ops[x]);

    if(log_would(L_LISTWISE | L_PARSE))
    {
      fatal(log_start, L_LISTWISE | L_PARSE, &token);
      fatal(transform_description_say, *g, log_narrator());
      fatal(log_finish, &token);
    }
	}

finally:
  fatal(log_finish, &token);

	// cleanup state for this parse
	transform_yy_delete_buffer(state, (*p)->p);
	transform_free(pp.g);
	yyu_extra_destroy(&pp);
	transform_parser_free(lp);

  xapi_infos("transform-string", b);
coda;
}

static xapi arg_canon_say(arg * const arg, uint32_t sm, narrator * const restrict N)
{
  enter;

	// track backreferences
	int k = 0;

	// emit characters according to the scanmode
	int i;
	for(i = 0; i < arg->l; i++)
	{
		if(arg->refs.l > k && arg->refs.v[k].s == &arg->s[i])
		{
			sayf("\\%d", arg->refs.v[k].ref);

			i += arg->refs.v[k].l - 1;
			k++;
		}
		else if(arg->s[i] == ' ' && GS_DELIMITED(sm))
		{
			sayf("\\s");
		}
		else if(arg->s[i] == '\t' && GS_DELIMITED(sm))
		{
			sayf("\\t");
		}
		else if(arg->s[i] == '\r' && GS_DELIMITED(sm))
		{
			sayf("\\r");
		}
		else if(arg->s[i] == '\n' && GS_DELIMITED(sm))
		{
			sayf("\\n");
		}
		else if((arg->s[i] != ' ' || arg->s[i] != '\t' || arg->s[i] != '\n' || arg->s[i] != '\r') && (arg->s[i] <= 0x20 || arg->s[i] >= 0x7f))
		{
			sayf("\\x{%02hhx}", arg->s[i]);
		}
		else if(arg->s[i] == '\\' && GS_DOREFS(sm))
		{
			sayf("\\%c", arg->s[i]);
		}
		else if(GS_DELIMITED(sm) && arg->s[i] == genscan_opening_char[sm])
		{
			sayf("\\%c", arg->s[i]);
		}
		else if(GS_ENCLOSED(sm) && arg->s[i] == genscan_closing_char[sm])
		{
			sayf("\\%c", arg->s[i]);
		}
		else
		{
			sayf("%c", arg->s[i]);
		}
	}

	finally : coda;
}

static xapi args_canon_say(arg ** const args, const int argsl, uint32_t sm, narrator * const restrict N)
{
  enter;

	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the opener
		if(GS_ENCLOSED(sm))
			sayf("%c", genscan_opening_char[sm]);
		else if(x)
			sayf("%c", genscan_opening_char[sm]);

		// emit the argument string
		fatal(arg_canon_say, args[x], sm, N);

		// emit the closer
		if(GS_ENCLOSED(sm))
			sayf("%c", genscan_closing_char[sm]);
		else if(x == (argsl - 1) && args[x]->l == 0)
			sayf("%c", genscan_opening_char[sm]);
	}

	finally : coda;
}

static xapi operations_canon_say(operation ** const ops, int opsl, uint32_t sm, narrator * const restrict N)
{
  enter;

	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			sayf(" ");

		fatal(operation_canon_say, ops[x], sm, N);
	}

	finally : coda;
}

///
/// public : describe
///

xapi operation_canon_say(operation * const oper, uint32_t sm, narrator * const restrict N)
{
  enter;

	sayf("%s", oper->op->s);

	// conditionally emit the delimiter
	if(oper->argsl)
		sayf("%c", genscan_opening_char[sm]);

	fatal(args_canon_say, oper->args, oper->argsl, sm, N);

	finally : coda;
}

///
/// api
///

API xapi transform_parser_create(transform_parser ** restrict p)
{
	enter;

	fatal(xmalloc, p, sizeof(**p));

	if(transform_yylex_init(&(*p)->p) != 0)
		tfail(perrtab_SYS, ENOMEM);

	finally : coda;
}

API void transform_parser_free(transform_parser * restrict p)
{
	if(p)
	{
		transform_yylex_destroy(p->p);
	}

	free(p);
}

API void transform_parser_ifree(transform_parser ** const restrict p)
{
	transform_parser_free(*p);
	*p = 0;
}

API xapi transform_parse(transform_parser ** restrict p, const char * const restrict s, size_t sl, transform ** const restrict g)
{
	enter;

	fatal(parse, p, s, sl, 0, g);

	finally : coda;
}

API xapi transform_parsex(transform_parser ** restrict p, const char * const restrict s, size_t sl, char * const restrict name, transform ** g)
{
	enter;

	fatal(parse, p, s, sl, name, g);

	finally : coda;
}

API void transform_free(transform * restrict g)
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

API void transform_ifree(transform ** const restrict g)
{
	transform_free(*g);
	*g = 0;
}

API xapi transform_canon_say(transform * const restrict g, narrator * const restrict N)
{
  enter;

	uint32_t sm = GENSCAN_SLASH_DOREFS;

  off_t pos;
  fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &pos);

	fatal(args_canon_say, g->args, g->argsl, sm, N);

  off_t T = pos;
  fatal(narrator_seek, N, 0, NARRATOR_SEEK_CUR, &pos);

	if(T != pos)
		says(" ");

	fatal(operations_canon_say, g->ops, g->opsl, sm, N);

	finally : coda;
}

API xapi transform_description_say(transform * const restrict g, narrator * const restrict N)
{
  enter;

	// expanded transform description
	sayf("transform @ %p {\n", g);
	sayf("  initial list\n");

	int x;
	for(x = 0; x < g->argsl; x++)
	{
		sayf("    ");
		fatal(arg_canon_say, g->args[x], 0, N);
		sayf("\n");
	}

	sayf("  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		sayf("    OP - %s\n", g->ops[x]->op->s);
		sayf("      args\n");

		int y;
		for(y = 0; y < g->ops[x]->argsl; y++)
		{
			sayf("        ");
			fatal(arg_canon_say, g->ops[x]->args[y], 0, N);
			sayf("\n");
		}

		if(y == 0)
			sayf("        none\n");
	}

	if(x == 0)
		sayf("    none");

	// canonicalized transform-string
	sayf("\n --> ");
	fatal(transform_canon_say, g, N);
	sayf("\n}");

	finally : coda;
}
