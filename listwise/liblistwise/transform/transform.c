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

#include "internal.h"

#include "transform/transform.def.h"
#include "transform/transform.tab.h"
#include "transform/transform.lex.h"
#include "transform/transform.tokens.h"
#include "transform/transform.states.h"
#include "genscan.h"

#include "xlinux.h"

#include "macros.h"

#define restrict __restrict

struct transform_parser
{
	void * p;		// scanner
};

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

static xapi parse(transform_parser ** p, char* s, int l, char * name, int namel, transform** g, void * udata)
{
  enter;

	// local parser
	transform_parser * lp = 0;

	// create state specific to this parse
	void * state = 0;

	// transform-string
	char * b = s;

	if(!p)
		p = &lp;

	if(!*p)
		fatal(transform_mkparser, p);

	// results struct for this parse
	parse_param pp = {
		  .tokname				= transform_tokenname
		, .statename			= transform_statename
		, .inputstr				= transform_inputstr
		, .lvalstr				= transform_lvalstr
#if DEBUG || DEVEL
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

		fatal(transform_description_log, *g, 0, udata);
	}

finally:
	// cleanup state for this parse
	transform_yy_delete_buffer(state, (*p)->p);
	transform_free(pp.g);
	yyu_extra_destroy(&pp);
	transform_parser_free(lp);

XAPI_INFOS("transform-string", b);
coda;
}

///
/// static : describe
///

static xapi transform_arg_canon(arg * const arg, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
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
			SAY("\\%d", arg->refs.v[k].ref);

			i += arg->refs.v[k].l - 1;
			k++;
		}
		else if(arg->s[i] == ' ' && GS_DELIMITED(sm))
		{
			SAY("\\s");
		}
		else if(arg->s[i] == '\t' && GS_DELIMITED(sm))
		{
			SAY("\\t");
		}
		else if(arg->s[i] == '\r' && GS_DELIMITED(sm))
		{
			SAY("\\r");
		}
		else if(arg->s[i] == '\n' && GS_DELIMITED(sm))
		{
			SAY("\\n");
		}
		else if((arg->s[i] != ' ' || arg->s[i] != '\t' || arg->s[i] != '\n' || arg->s[i] != '\r') && (arg->s[i] <= 0x20 || arg->s[i] >= 0x7f))
		{
			SAY("\\x{%02hhx}", arg->s[i]);
		}
		else if(arg->s[i] == '\\' && GS_DOREFS(sm))
		{
			SAY("\\%c", arg->s[i]);
		}
		else if(GS_DELIMITED(sm) && arg->s[i] == genscan_opening_char[sm])
		{
			SAY("\\%c", arg->s[i]);
		}
		else if(GS_ENCLOSED(sm) && arg->s[i] == genscan_closing_char[sm])
		{
			SAY("\\%c", arg->s[i]);
		}
		else
		{
			SAY("%c", arg->s[i]);
		}
	}

	finally : coda;
}

static xapi transform_args_canon(arg ** const args, const int argsl, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
  enter;

	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the opener
		if(GS_ENCLOSED(sm))
			SAY("%c", genscan_opening_char[sm]);
		else if(x)
			SAY("%c", genscan_opening_char[sm]);

		// emit the argument string
		fatal(transform_arg_canon, args[x], sm, dst + (*z), sz - (*z), z, ps, writer);

		// emit the closer
		if(GS_ENCLOSED(sm))
			SAY("%c", genscan_closing_char[sm]);
		else if(x == (argsl - 1) && args[x]->l == 0)
			SAY("%c", genscan_opening_char[sm]);
	}

	finally : coda;
}

static xapi transform_operations_canon(operation ** const ops, int opsl, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
  enter;

	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			SAY(" ");

		fatal(transform_operation_canon, ops[x], sm, dst + (*z), sz - (*z), z, ps, writer);
	}

	finally : coda;
}

static xapi transform_canon(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
  enter;

	uint32_t sm = GENSCAN_SLASH_DOREFS;

	fatal(transform_args_canon, g->args, g->argsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	if(z)
		SAY(" ");

	fatal(transform_operations_canon, g->ops, g->opsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	finally : coda;
}

static xapi transform_description(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * const z, pstring ** restrict ps, fwriter writer)
{
  enter;

	// expanded transform description
	SAY("transform @ %p {\n", g);
	SAY("  initial list\n");

	int x;
	for(x = 0; x < g->argsl; x++)
	{
		SAY("    ");
		fatal(transform_arg_canon, g->args[x], 0, dst + (*z), sz - (*z), z, ps, writer);
		SAY("\n");
	}

	SAY("  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		SAY("    OP - %s\n", g->ops[x]->op->s);
		SAY("      args\n");

		int y;
		for(y = 0; y < g->ops[x]->argsl; y++)
		{
			SAY("        ");
			fatal(transform_arg_canon, g->ops[x]->args[y], 0, dst + (*z), sz - (*z), z, ps, writer);
			SAY("\n");
		}

		if(y == 0)
			SAY("        none\n");
	}

	if(x == 0)
		SAY("    none");

	// canonicalized transform-string
	SAY("\n --> ");
	fatal(transform_canon, g, dst + (*z), sz - (*z), z, ps, writer);
	SAY("\n}");

	finally : coda;
}

///
/// public : describe
///

xapi transform_operation_canon(operation * const oper, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
  enter;

	SAY("%s", oper->op->s);

	// conditionally emit the delimiter
	if(oper->argsl)
		SAY("%c", genscan_opening_char[sm]);

	fatal(transform_args_canon, oper->args, oper->argsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	finally : coda;
}

///
/// api
///

API xapi transform_mkparser(transform_parser ** p)
{
	enter;

	fatal(xmalloc, p, sizeof(**p));

	if(transform_yylex_init(&(*p)->p) != 0)
		tfail(perrtab_SYS, ENOMEM);

	finally : coda;
}

API void transform_parser_free(transform_parser * p)
{
	if(p)
	{
		transform_yylex_destroy(p->p);
	}

	free(p);
}

API void transform_parser_xfree(transform_parser ** p)
{
	transform_parser_free(*p);
	*p = 0;
}

API void transform_free(transform* g)
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

API void transform_xfree(transform** g)
{
	transform_free(*g);
	*g = 0;
}

API xapi transform_parse(transform_parser ** p, char* s, int l, transform** g)
{
	enter;

	fatal(parse, p, s, l, 0, 0, g, 0);

	finally : coda;
}

API xapi transform_parse2(transform_parser ** p, char* s, int l, transform** g, void * udata)
{
	enter;

	fatal(parse, p, s, l, 0, 0, g, udata);

	finally : coda;
}

API xapi transform_parse_named(transform_parser ** p, char* s, int l, char * name, int namel, transform** g)
{
	enter;

	fatal(parse, p, s, l, name, namel, g, 0);

	finally : coda;
}

API xapi transform_parse_named2(transform_parser ** p, char* s, int l, char * name, int namel, transform** g, void * udata)
{
	enter;

	fatal(parse, p, s, l, name, namel, g, udata);

	finally : coda;
}

///
/// api : describe
///

API xapi transform_canon_write(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
{
  enter;

	size_t lz = 0;
	if(!z)
		z = &lz;

	fatal(transform_canon, g, dst, sz, z, 0, zwrite);

  finally : coda;
}

API xapi transform_canon_pswrite(transform * const restrict g, pstring ** restrict ps)
{
	enter;

	size_t lz = 0;
	fatal(psclear, ps);
	fatal(transform_canon, g, 0, 0, &lz, ps, pswrite);

	finally : coda;
}

API xapi transform_canon_dump(transform * const restrict g, pstring ** restrict ps)
{
	enter;

	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	size_t lz = 0;
	fatal(transform_canon, g, 0, 0, &lz, ps, pswrite);
	printf("%.*s\n", (int)(*ps)->l, (*ps)->s);

finally:
	psfree(lps);
coda;
}

API xapi transform_canon_log(transform * const restrict g, pstring ** restrict ps, void * restrict udata)
{
	enter;

	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	if(lw_would_transform())
	{
		size_t lz = 0;
		fatal(transform_canon, g, 0, 0, &lz, ps, pswrite);
		lw_log_transform("%.*s", (int)(*ps)->l, (*ps)->s);
	}

finally:
	psfree(lps);
coda;
}

API xapi transform_description_write(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
{
  enter;

	size_t lz = 0;
	if(!z)
		z = &lz;

	fatal(transform_description, g, dst, sz, z, 0, zwrite);

  finally : coda;
}

API xapi transform_description_pswrite(transform * const restrict g, pstring ** restrict ps)
{
	enter;

	size_t lz = 0;
	fatal(psclear, ps);
	fatal(transform_description, g, 0, 0, &lz, ps, pswrite);

	finally : coda;
}

API xapi transform_description_dump(transform * const restrict g, pstring ** restrict ps)
{
	enter;

	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	size_t lz = 0;
	fatal(transform_description, g, 0, 0, &lz, ps, pswrite);
	printf("%.*s\n", (int)(*ps)->l, (*ps)->s);

finally:
	psfree(lps);
coda;
}

API xapi transform_description_log(transform * const restrict g, pstring ** restrict ps, void * restrict udata)
{
	enter;

	pstring * lps = 0;

	if(lw_would_transform())
	{
		if(!ps)
			ps = &lps;
		fatal(psclear, ps);

		size_t lz = 0;
		fatal(transform_description, g, 0, 0, &lz, ps, pswrite);
		lw_log_transform("%.*s", (int)(*ps)->l, (*ps)->s);
	}

finally:
	psfree(lps);
coda;
}
