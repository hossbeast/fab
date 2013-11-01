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

#include "liblistwise_control.h"

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
		if(token == generator_CREF)
		{
			if(((YYSTYPE*)lval)->ref == 0x07)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\a");
			else if(((YYSTYPE*)lval)->ref == 0x08)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\b");
			else if(((YYSTYPE*)lval)->ref == 0x09)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\t");
			else if(((YYSTYPE*)lval)->ref == 0x0a)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\n");
			else if(((YYSTYPE*)lval)->ref == 0x0b)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\v");
			else if(((YYSTYPE*)lval)->ref == 0x0c)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\f");
			else if(((YYSTYPE*)lval)->ref == 0x0d)
				*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\r");
		}
		else if(token == generator_I64)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "%"PRId64, ((YYSTYPE*)lval)->i64);
		else if(token == generator_BREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\%d", ((YYSTYPE*)lval)->ref);
		else if(token == generator_HREF)
			*bufl = snprintf(pp->temp, sizeof(pp->temp), "\\x{%02x}", ((YYSTYPE*)lval)->ref);
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

	int x;
	int y;

	printf("generator @ %p {\n", g);
	printf("  initial list\n");

	char * s = space;
	size_t sz = sizeof(space);
	uint32_t sm = 0;
	size_t z = 0;

	z += generator_args_write(g->args, g->argsl, s + z, sz - z, &sm);

	for(x = 0; x < g->argsl; x++)
	{
		printf("    '%.*s'\n", g->args[x]->l, g->args[x]->s);
	}

	printf("  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		if(x || z)
			z += snprintf(s + z, sz - z, " ");

		z += generator_operation_write(g->ops[x], s + z, sz - z, &sm);

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

///
/// public
///

size_t generator_operations_write(operation ** const ops, int opsl, char * const buf, const size_t sz, uint32_t * sm)
{
	uint32_t local_sm;
	if(!sm)
	{
		local_sm = GENSCAN_SLASH_WITHREFS;
		sm = &local_sm;
	}

	size_t z = 0;
	
	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			z += snprintf(buf + z, sz - z, " ");

		z += generator_operation_write(ops[x], buf + z, sz - z, sm);
	}

	return z;
}

size_t generator_operation_write(operation * const oper, char * const buf, const size_t sz, uint32_t * sm)
{
	uint32_t local_sm;
	if(!sm)
	{
		local_sm = GENSCAN_SLASH_WITHREFS;
		sm = &local_sm;
	}

	size_t z = 0;
	z += snprintf(buf + z, sz - z, "%s", oper->op->s);

	// emit the delimiter
	if(oper->argsl)
		z += snprintf(buf + z, sz - z, "%c", genscan_opener[*sm]);

	z += generator_args_write(oper->args, oper->argsl, buf, sz - z, sm);

	return z;
}

size_t generator_args_write(arg ** const args, const int argsl, char * const buf, const size_t sz, uint32_t * sm)
{
	size_t z = 0;
	uint32_t local_sm;
	if(!sm)
	{
		local_sm = GENSCAN_SLASH_WITHREFS;
		sm = &local_sm;
	}

	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the delimiter
		if(x)
			z += snprintf(buf + z, sz - z, "%c", genscan_opener[*sm]);

		z += generator_arg_write(args[x], buf, sz - z, sm);
	}

	return z;
}

size_t generator_arg_write(arg * const arg, char * const buf, const size_t sz, uint32_t * sm)
{
	size_t z = 0;
	uint32_t local_sm;
	if(!sm)
	{
		local_sm = GENSCAN_SLASH_WITHREFS;
		sm = &local_sm;
	}

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
	uint32_t newsm = 0;

	if(brefs || hrefs)
		newsm |= GENSCAN_MODE_WITHREFS;

	if(ws)
		newsm |= GENSCAN_CHAR_BRACES;
	else if(!slash)
		newsm |= GENSCAN_CHAR_SLASH;
	else if(!comma)
		newsm |= GENSCAN_CHAR_COMMA;
	else
		newsm |= GENSCAN_CHAR_BRACES;

	// if the scanmode is changing, emit a scanmode directive
	if(newsm != *sm)
	{
		z += snprintf(buf + z, sz - z, " !");

		if((newsm & GENSCAN_CHAR) == GENSCAN_CHAR_SLASH)
			z += snprintf(buf + z, sz - z, "/");
		else if((newsm & GENSCAN_CHAR) == GENSCAN_CHAR_COMMA)
			z += snprintf(buf + z, sz - z, ",");
		else if((newsm & GENSCAN_CHAR) == GENSCAN_CHAR_BRACES)
			z += snprintf(buf + z, sz - z, "{}");

		if((newsm & GENSCAN_MODE) == GENSCAN_MODE_WITHREFS)
			z += snprintf(buf + z, sz - z, "\\");

		z += snprintf(buf + z, sz - z, " ");
		*sm = newsm;
	}

	// track backreferences
	int k = 0;

	// emit characters according to the scanmode
	for(i = 0; i < arg->l; i++)
	{
		if(arg->refs.l > k && arg->refs.v[k].s == &arg->s[i])
		{
			z += snprintf(buf + z, sz - z, "\\%d", arg->refs.v[k].ref);

			k++;
			i += arg->refs.v[k].l - 1;
		}
		else if(arg->s[i] == ' ')
		{
			if(((*sm) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_OPEN)
				z += snprintf(buf + z, sz - z, "\\x{%02hhx}", arg->s[i]);
			else if(((*sm) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_CLOSED)
				z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
		else if(arg->s[i] == '/')
		{
			if(((*sm) & GENSCAN_CHAR) == GENSCAN_CHAR_SLASH)
				z += snprintf(buf + z, sz - z, "\\%c", arg->s[i]);
			else
				z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
		else if(arg->s[i] == ',')
		{
			if(((*sm) & GENSCAN_CHAR) == GENSCAN_CHAR_COMMA)
				z += snprintf(buf + z, sz - z, "\\%c", arg->s[i]);
			else
				z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
		else if(arg->s[i] == '}')
		{
			if(((*sm) & GENSCAN_CHAR) == GENSCAN_CHAR_BRACES)
				z += snprintf(buf + z, sz - z, "\\%c", arg->s[i]);
			else
				z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
		else if(arg->s[i] == '\\')
		{
			if(((*sm) & GENSCAN_MODE) == GENSCAN_MODE_WITHREFS)
				z += snprintf(buf + z, sz - z, "\\%c", arg->s[i]);
			else
				z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
		else if(arg->s[i] <= 0x1f || arg->s[i] >= 0x7f)
		{
			z += snprintf(buf + z, sz - z, "\\x{%02hhx}", arg->s[i]);
		}
		else
		{
			z += snprintf(buf + z, sz - z, "%c", arg->s[i]);
		}
	}

	// emit the closing token if any
	if((((*sm) & GENSCAN_CLOSURE) == GENSCAN_CLOSURE_CLOSED) || arg->l == 0)
		z += snprintf(buf + z, sz - z, "%c", genscan_closer[*sm]);

	return z;
}
