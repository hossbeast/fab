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

#include "xmem.h"

#include "liblistwise_control.h"

#define restrict __restrict

struct generator_parser_t
{
	void * p;		// scanner
};

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

	dprintf(listwise_info_fd, "\n");
}

static int generator_inputname(struct yyu_extra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
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

	if(token == generator_I64 || token == generator_BREF || token == generator_HREF || token == generator_OP)
	{
		if(token == generator_I64)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%ld", ((YYSTYPE*)lval)->i64);
		else if(token == generator_BREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%d", ((YYSTYPE*)lval)->ref);
		else if(token == generator_HREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\x{%02x}", ((YYSTYPE*)lval)->ref);
		else if(token == generator_OP)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%s", ((YYSTYPE*)lval)->op->s);

		*buf = pp->temp;
	}

	return 0;
}

static int parse(generator_parser* p, char* s, int l, char * name, int namel, generator** g)
{
	// create state specific to this parse
	void * state = 0;

	// results struct for this parse
	parse_param pp = {
		  .r = 0
		, .output_line	= 1
		, .info				= write_info
		, .error			= write_error
		, .tokname		= generator_tokenname
		, .statename	= generator_statename
		, .inputname	= generator_inputname
		, .lvalstr		= generator_lvalstr
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
	{
		if((pp.g->ops[x]->op->optype & LWOP_ARGS_CANHAVE) == 0)
		{
			if(pp.g->ops[x]->argsl)
			{
				fail("%s - arguments not expected\n", pp.g->ops[x]->op->s);
			}
		}

		if(pp.g->ops[x]->op->op_validate)
		{
			fatal(pp.g->ops[x]->op->op_validate, pp.g->ops[x]);
		}
	}

	(*g) = pp.g;
	pp.g = 0;

finally:
	// cleanup state for this parse
	generator_yy_delete_buffer(state, p->p);
	generator_free(pp.g);
	yyu_extra_destroy(&pp);
coda;
}

///
/// public
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

size_t operation_write(char * s, size_t sz, const operation * const op)
{
	size_t z = 0;
	z += snprintf(s + z, sz - z, "%s", op->op->s);

	int x;
	for(x = 0; x < op->argsl; x++)
		z += snprintf(s + z, sz - z, "/%s", op->args[x]->s);

	if(x && strlen(op->args[x - 1]->s) == 0)
		z += snprintf(s + z, sz - z, "/");

	return z;
}

void API generator_dump(generator* g)
{
	char buf[2048] = {};
	char * s = buf;
	size_t sz = sizeof(buf);
	size_t z = 0;

	int x;
	int y;

	printf("generator @ %p {\n", g);
	printf("  initial list\n");
	for(x = 0; x < g->argsl; x++)
	{
		if(x)
			z += snprintf(s + z, sz - z, "/");
		z += snprintf(s + z, sz - z, "%.*s", g->args[x]->l, g->args[x]->s);

		printf("    '%.*s'\n", g->args[x]->l, g->args[x]->s);
	}

	printf("  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		if(x || z)
			z += snprintf(s + z, sz - z, "/");

		z += operation_write(s + z, sz - z, g->ops[x]);

		printf("    OP - %s\n", g->ops[x]->op->s);
		printf("      args\n");
		for(y = 0; y < g->ops[x]->argsl; y++)
		{
			printf("        '%s'\n", g->ops[x]->args[y]->s);
		}

		if(y == 0)
			printf("        none\n");
	}

	if(x == 0)
		printf("    none\n");

	printf("\n");
	printf(" --> %s\n", s);
	printf("}\n");
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
