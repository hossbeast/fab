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

#include <stdarg.h>
#include <inttypes.h>

#include "listwise/internal.h"

#include "generator/generator.def.h"
#include "generator/generator.tab.h"
#include "generator/generator.lex.h"
#include "generator/generator.tokens.h"
#include "generator/generator.states.h"
#include "genscan.h"

#include "xmem.h"


#define restrict __restrict

struct generator_parser_t
{
	void * p;		// scanner
};

///
/// static
///

static void write_info(char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vdprintf(listwise_info_fd, fmt, va);
	va_end(va);

	dprintf(listwise_info_fd, "\n");
}

static void write_error(char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vdprintf(listwise_error_fd, fmt, va);
	va_end(va);

	dprintf(listwise_error_fd, "\n");
}

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
	return generator_statenames[state];
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
			fail(LW_ARGSNUM, "expected: 0, actual: %d", op->argsl);
		}
	}

	if(op->op->op_validate)
	{
		fatal(op->op->op_validate, op);
	}

finally :
	XAPI_INFO(1, "operator", "%s", op->s);
coda;
}

static int parse(generator_parser* p, char* s, int l, char * name, int namel, generator** g)
{
	// create state specific to this parse
	void * state = 0;

	// results struct for this parse
	parse_param pp = {
		  .output_line	= 1
		, .log_state		= write_info
		, .log_token		= write_info
		, .log_error		= write_error
		, .tokname			= generator_tokenname
		, .statename		= generator_statename
		, .inputstr			= generator_inputstr
		, .lvalstr			= generator_lvalstr
	};

	// specific exception for "shebang" line exactly at the beginning

	char * b = s;
	if(strlen(s) > 1 && memcmp(s, "#!", 2) == 0 && strstr(s, "\n"))
		b = strstr(s, "\n") + 1;

	if((state = generator_yy_scan_string(b, p->p)) == 0)
		qfail();

	// results struct for this parse
	pp.scanner = p->p;

	// make it available to the lexer
	generator_yyset_extra(&pp, p->p);

	// parse
	generator_yyparse(p->p, &pp);

	if(pp.r)
		qfail();

	// postprocessing
	int x;
	for(x = 0; x < pp.g->opsl; x++)
		fatal(operation_validate, pp.g->ops[x]);

	(*g) = pp.g;
	pp.g = 0;

finally:
	// cleanup state for this parse
	generator_yy_delete_buffer(state, p->p);
	generator_free(pp.g);
	yyu_extra_destroy(&pp);
coda;
}

#undef restrict

///
/// API
///

int API generator_mkparser(generator_parser** p)
{
	if((*p = calloc(1, sizeof(*p[0]))) == 0)
		return 1;

	if(generator_yylex_init(&(*p)->p) != 0)
		return 1;

	return 0;
}

void API generator_parser_free(generator_parser* p)
{
	if(p)
	{
		generator_yylex_destroy(p->p);
	}

	free(p);
}

void API generator_parser_xfree(generator_parser** p)
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

int API generator_parse(generator_parser* p, char* s, int l, generator** g)
{
	return parse(p, s, l, 0, 0, g);
}

int API generator_parse_named(generator_parser* p, char* s, int l, char * name, int namel, generator** g)
{
	return parse(p, s, l, name, namel, g);
}

void API generator_dump(generator* g)
{
	char space[2048];

	size_t z = 0;

	// expanded generator description
	dprintf(listwise_info_fd, "generator @ %p {\n", g);
	dprintf(listwise_info_fd, "  initial list\n");

	int x;
	for(x = 0; x < g->argsl; x++)
	{
		z = generator_arg_snwrite(space, sizeof(space), g->args[x], 0);
		dprintf(listwise_info_fd, "    %.*s\n", (int)z, space);
	}

	dprintf(listwise_info_fd, "  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		dprintf(listwise_info_fd, "    OP - %s\n", g->ops[x]->op->s);
		dprintf(listwise_info_fd, "      args\n");

		int y;
		for(y = 0; y < g->ops[x]->argsl; y++)
		{
			z = generator_arg_snwrite(space, sizeof(space), g->ops[x]->args[y], 0);
			dprintf(listwise_info_fd, "        %.*s\n", (int)z, space);
		}

		if(y == 0)
			dprintf(listwise_info_fd, "        none\n");
	}

	if(x == 0)
		dprintf(listwise_info_fd, "    none\n");

	// normalized generator-string
	dprintf(listwise_info_fd, "\n");

	z = generator_snwrite(space, sizeof(space), g);
	dprintf(listwise_info_fd, " --> %.*s\n", (int)z, space);

	dprintf(listwise_info_fd, "}\n");
}

///
/// public
///

size_t generator_snwrite(char * const dst, const size_t sz, generator * const g)
{
	uint32_t sm = GENSCAN_SLASH_DOREFS;

	size_t z = 0;
	z += generator_args_snwrite(dst + z, sz - z, g->args, g->argsl, sm);

	if(z)
		z += snprintf(dst + z, sz - z, " ");

	return z + generator_operations_snwrite(dst + z, sz - z, g->ops, g->opsl, sm);
}

size_t generator_operations_snwrite(char * const dst, const size_t sz, operation ** const ops, int opsl, uint32_t sm)
{
	size_t z = 0;
	
	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			z += snprintf(dst + z, sz - z, " ");

		z += generator_operation_snwrite(dst + z, sz - z, ops[x], sm);
	}

	return z;
}

size_t generator_operation_snwrite(char * const dst, const size_t sz, operation * const oper, uint32_t sm)
{
	size_t z = 0;
	z += snprintf(dst + z, sz - z, "%s", oper->op->s);

	// emit the delimiter
	if(oper->argsl)
		z += snprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);

	return z + generator_args_snwrite(dst + z, sz - z, oper->args, oper->argsl, sm);
}

size_t generator_args_snwrite(char * const dst, const size_t sz, arg ** const args, const int argsl, uint32_t sm)
{
	size_t z = 0;

	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the opener
		if(GS_ENCLOSED(sm))
			z += snprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);
		else if(x)
			z += snprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);

		// emit the argument string
		z += generator_arg_snwrite(dst + z, sz - z, args[x], sm);

		// emit the closer
		if(GS_ENCLOSED(sm))
			z += snprintf(dst + z, sz - z, "%c", genscan_closing_char[sm]);
		else if(x == (argsl - 1) && args[x]->l == 0)
			z += snprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);
	}

	return z;
}

size_t generator_arg_snwrite(char * const dst, const size_t sz, arg * const arg, uint32_t sm)
{
	size_t z = 0;

	// track backreferences
	int k = 0;

	// emit characters according to the scanmode
	int i;
	for(i = 0; i < arg->l; i++)
	{
		if(arg->refs.l > k && arg->refs.v[k].s == &arg->s[i])
		{
			z += snprintf(dst + z, sz - z, "\\%d", arg->refs.v[k].ref);

			i += arg->refs.v[k].l - 1;
			k++;
		}
		else if((arg->s[i] == ' ' || arg->s[i] == '\t' || arg->s[i] == '\n') && GS_DELIMITED(sm))
		{
			z += snprintf(dst + z, sz - z, "\\x{%02hhx}", arg->s[i]);
		}
		else if((arg->s[i] != ' ' || arg->s[i] != '\t' || arg->s[i] != '\n') && (arg->s[i] <= 0x20 || arg->s[i] >= 0x7f))
		{
			z += snprintf(dst + z, sz - z, "\\x{%02hhx}", arg->s[i]);
		}
		else if(arg->s[i] == '\\' && GS_DOREFS(sm))
		{
			z += snprintf(dst + z, sz - z, "\\%c", arg->s[i]);
		}
		else if(GS_DELIMITED(sm) && arg->s[i] == genscan_opening_char[sm])
		{
			z += snprintf(dst + z, sz - z, "\\%c", arg->s[i]);
		}
		else if(GS_ENCLOSED(sm) && arg->s[i] == genscan_closing_char[sm])
		{
			z += snprintf(dst + z, sz - z, "\\%c", arg->s[i]);
		}
		else
		{
			z += snprintf(dst + z, sz - z, "%c", arg->s[i]);
		}
	}

	return z;
}

uint32_t generator_arg_scanmode(arg * const arg)
{
	int brefs = 0;
	int hrefs = 0;
	int ws = 0;
	int slash = 0;
	int comma = 0;
	int rbrace = 0;

	int i;
	for(i = 0; i < arg->l; i++)
	{
		if(arg->s[i] == ' ')
			ws++;
		else if(arg->s[i] == '/')
			slash++;
		else if(arg->s[i] == ',')
			comma++;
		else if(arg->s[i] == '}')
			rbrace++;
		else if(arg->s[i] <= 0x1f || arg->s[i] >= 0x7f)
			hrefs++;
	}

	for(i = 0; i < arg->refs.l; i++)
	{
		if(arg->refs.v[i].k == REFTYPE_BREF)
		{
			brefs++;
			break;
		}
	}

	// determine the appropriate scanmode for this argument
	uint32_t sm = 0;

	if(brefs || hrefs)
		sm |= GENSCAN_ESCAPE_MODE_DOREFS;

	if(ws)
		sm |= GENSCAN_TOKENS_BRACES;
	else if(!slash)
		sm |= GENSCAN_TOKENS_SLASH;
	else if(!comma)
		sm |= GENSCAN_TOKENS_COMMA;
	else
		sm |= GENSCAN_TOKENS_BRACES;

	return sm;
}
