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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "xlinux.h"
#include "xapi.h"

#include "logger.h"
#include "macros.h"
#include "strutil.h"

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 60])
#define COLOR(x)		(char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

#if DEVEL
#define TRACEARGS const char * const func, const char * const file, int line,
#define TRACEPASS func, file, line,
#define NOTRACE   0, 0, 0,
#else
#define TRACEARGS
#define TRACEPASS
#define NOTRACE
#endif

static struct filter
{
	uint64_t	v;		// tag
	char			m;		// mode, (, [, {, or <
	char			o;		// operation, + or -
} * 							o_filter;
static int				o_filter_a;
static int				o_filter_l;

struct g_logs_t * g_logs;
int g_logs_l;

char ** g_argv;
int g_argc;
char * g_argvs;
int g_argvsl;

//
// [[ static ]]
//

static char **	o_logexprs;
static int			o_logexprs_a;
static int			o_logexprs_l;

static uint64_t o_space_bits;	// when a log is being constructed, effective bits
static char * 	o_space;			// storage between logvprintf/flush calls
static int			o_space_a;		// allocation
static int			o_space_l;		// bytes
static int			o_space_w;		// visible characters

static uint64_t	o_prefix_bits = ~0ULL;	// prefix decision bits

#if DEVEL
static uint64_t	o_trace_bits  = 0ULL;		// trace decision bits

static char *		o_trace_func;		// trace storage
static int			o_trace_func_a;
static int			o_trace_func_l;
static char * 	o_trace_file;
static int			o_trace_file_a;
static int			o_trace_file_l;
static int			o_trace_line;
#endif

static int o_name_len;	// max prefix name length

static unsigned char o_colors[] = {
	/* NO COLOR */   0x00
	/* L_RED 		*/ , 0x31
	/* L_GREEN	*/ , 0x32
	/* L_YELLOW */ , 0x33
	/* L_CYAN 	*/ , 0x36
	/* L_BLUE 	*/ , 0x34
};

static void __attribute__((constructor)) init()
{
	// determine logtag len
	int x;
	for(x = 0; x < g_logs_l; x++)
	{
		g_logs[x].l = strlen(g_logs[x].s);
		o_name_len = MAX(o_name_len, g_logs[x].l);
	}
}

static int logwrite(const char * s, size_t l, int w)
{
	/*
	** w specifies whether this segment comprises "visible chars", or not
	*/

	if((o_space_l + l) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + l + 1);
		o_space_a = o_space_l + l + 1;
	}

	memcpy(o_space + o_space_l, s, l);
	o_space_l += l;

	if(w)
		o_space_w += l;

	return l;
}

static int logvprintf(const char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);
	
	int w = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if((o_space_l + w) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + w + 1);
		o_space_a = o_space_l + w + 1;
	}

	vsprintf(o_space + o_space_l, fmt, va);

	o_space_l += w;
	o_space_w += w;

	return w;
}

static int logprintf(const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int w = logvprintf(fmt, va);
	va_end(va);

	return w;
}

static int start(const uint64_t e)
{
	if(log_would(e))
	{
#if DEVEL
		o_space_l = 0;
		o_space_w = 0;
		o_trace_func_l = 0;
		o_trace_file_l = 0;
#endif
		o_space_bits = e;

		// colorization
		if((e & L_COLOR_VALUE) && COLORHEX(e))
		{
			logwrite(COLOR(e), 0);
		}

		if(o_space_bits & o_prefix_bits)
		{
			// prefix
			int x;
			for(x = 0; x < g_logs_l; x++)
			{
				if((e & g_logs[x].v) == g_logs[x].v)
				{
					logprintf("%*s : ", o_name_len, g_logs[x].s);
					break;
				}
			}
		}

		return 1;
	}

	return 0;
}

static int finish()
{
	int w = 0;

#if DEVEL
	// location trace for errors
	if(o_space_bits & o_trace_bits)
		w += logprintf(" in %s at %s:%d", o_trace_func, o_trace_file, o_trace_line);
#endif

	if((o_space_bits & L_COLOR_VALUE) && COLORHEX(o_space_bits))
	{
		logwrite(NOCOLOR, 0);
	}

	logwrite("\n", 1, 0);

	// flush to stderr
	int __attribute__((unused)) r = write(1, o_space, o_space_l);

	o_space_bits = 0;

	return w;
}

static int describe(struct filter * f, char * dst, size_t sz)
{
	char * s = dst;
	size_t z = sz;

	size_t l = 0;
	l += snprintf(s + l, z - l, "%c%c", f->o, f->m);

	int y;
	int i = 0;
	for(y = 0; y < g_logs_l; y++)
	{
		if(g_logs[y].v & f->v)
		{
			if(i++)
				l += snprintf(s + l, z - l, "|");
			l += snprintf(s + l, z - l, "%s", g_logs[y].s);
		}
	}

	l += snprintf(s + l, z - l, "%c"
		, f->m == '(' ? ')'
		: f->m == '[' ? ']'
		: f->m == '{' ? '}'
		: f->m == '<' ? '>' : ' '
	);

	return l;
}

#if DEVEL
static void prep(const char * const func, const char * file, int line)
{
	int funcl = strlen(func);

	char * slash = 0;
	if((slash = strrchr(file, '/')))
		file = slash + 1;

	int filel = strlen(file);

	if(o_trace_func_a <= funcl)
	{
		o_trace_func = realloc(o_trace_func, funcl + 1);
		o_trace_func_a = funcl + 1;
	}
	memcpy(o_trace_func, func, funcl);
	o_trace_func[funcl] = 0;

	if(o_trace_file_a <= filel)
	{
		o_trace_file = realloc(o_trace_file, filel + 1);
		o_trace_file_a = filel + 1;
	}
	memcpy(o_trace_file, file, filel);
	o_trace_file[filel] = 0;

	o_trace_line = line;
}
#endif

/// parsetest
//
// SUMMARY
//  returns a boolean value indicating whether or not the specified argument is a valid logexpr
//
static int parsetest(char * expr, int exprl, struct filter * f)
{
	exprl = exprl ?: strlen(expr);

	if(exprl && (expr[0] == '+' || expr[0] == '-'))
	{
		char lhs = ' ';
		int off = 1;

		if(off < exprl)
		{
			if(expr[off] == '(')
				lhs = '(';
			else if(expr[off] == '{')
				lhs = '{';
			else if(expr[off] == '[')
				lhs = '[';
			else if(expr[off] == '<')
				lhs = '<';

			if(lhs != ' ')
				off++;

			if(off < exprl)
			{
				int y;
				for(y = off; y < exprl; y++)
				{
					if(lhs == ' ' && expr[y] == ' ')
						break;
					else if(lhs == '(' && expr[y] == ')')
						break;
					else if(lhs == '{' && expr[y] == '}')
						break;
					else if(lhs == '[' && expr[y] == ']')
						break;
					else if(lhs == '<' && expr[y] == '>')
						break;
				}

				uint64_t tag = 0;
				int k = 0;
				while(k != y)
				{
					if(expr[off] == ',' || expr[off] == '|')
						off++;

					for(k = off; k < y; k++)
					{
						if(expr[k] == ',' || expr[k] == '|')
							break;
					}

					if(estrcmp(&expr[off], k-off, "TAG", 3, 0) == 0)
					{
						tag |= L_TAG;
						off += 3;
					}
					else
					{
						int i;
						for(i = 0; i < g_logs_l; i++)
						{
							if(estrcmp(&expr[off], k-off, g_logs[i].s, g_logs[i].l, 0) == 0)
								break;
						}

						if(i == g_logs_l)
						{
							break; // nothing
						}
						else
						{
							tag |= g_logs[i].v;
							off += g_logs[i].l;
						}
					}
				}

				if(tag && (off == y))
				{
					if(f)
					{
						(*f) = (struct filter){
							  .v = tag & L_TAG
							, .o = expr[0]
							, .m = lhs == ' ' ? '(' : lhs
						};
					}
					return 1;
				}
			}
		}
	}

	return 0;
}

static int logparse(char * expr, int expr_len, int prepend, char * dst, size_t sz, size_t * z)
{
	struct filter f;

	if(parsetest(expr, expr_len, &f))
	{
		if(o_filter_a == o_filter_l)
		{
			int ns = o_filter_a ?: 3;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &o_filter, sizeof(*o_filter), ns, o_filter_a);
			o_filter_a = ns;
		}

		if(prepend)
		{
			memmove(
				  &o_filter[1]
				, &o_filter[0]
				, o_filter_l * sizeof(o_filter[0])
			);
			memcpy(&o_filter[0], &f, sizeof(f));
		}
		else
		{
			memcpy(&o_filter[o_filter_l], &f, sizeof(f));
		}
		o_filter_l++;

		if(dst && z)
			(*z) = describe(&f, dst, sz);
		else if(dst)
			describe(&f, dst, sz);
	}

	finally : coda;
}

static int logconfig(TRACEARGS uint64_t prefix
#if DEVEL
	, uint64_t trace
#endif
	, uint64_t bits
)
{
#if DEVEL
	o_trace_bits = trace;
#endif
	o_prefix_bits = prefix;

	// parse logexprs
	int x;
	for(x = 0; x < o_logexprs_l; x++)
	{
		if(bits)
		{
			char space[512];
			size_t sz = 0;
			fatal(logparse, o_logexprs[x], 0, 0, space, sizeof(space), &sz);

			if(sz)
			{
				log_logf(TRACEPASS bits, "%.*s", sz, space);
			}
		}
		else
		{
			fatal(logparse, o_logexprs[x], 0, 0, 0, 0, 0);
		}
	}

	finally : coda;
}

//
// [[ public ]]
//

int log_parse(char * expr, int expr_len, int prepend)
{
	xproxy(logparse, expr, expr_len, prepend, 0, 0, 0);
}

int log_log_parse_and_describe(TRACEARGS char * expr, int expr_len, int prepend, uint64_t bits)
{
	char space[512];
	size_t sz = 0;

	fatal(logparse, expr, expr_len, prepend, space, sizeof(space), &sz);

	if(sz)
	{
		log_logf(TRACEPASS bits, "%.*s", sz, space);
	}

	finally : coda;
}

int log_parse_pop()
{
	o_filter_l--;

	finally : coda;
}

int log_init()
{
	int			fd = -1;
	int			argsa = 0;	// g_argvs allocated size
	int			argva = 0;

	// snarf the cmdline
	fatal(xopen, "/proc/self/cmdline", O_RDONLY, &fd);

	// read into g_argvs - single string containing entire cmdline
	do
	{
		int newa = argsa ?: 10;
		newa += newa * 2 + newa / 2;
		fatal(xrealloc, &g_argvs, sizeof(*g_argvs), newa, argsa);
		argsa = newa;
	} while((g_argvsl += (read(fd, &g_argvs[g_argvsl], argsa - g_argvsl))) == argsa);

	g_argvsl--;

	// 1. replace nulls with spaces in g_argvs
	// 2. construct g_argv, array of arguments
	int x;
	int i = 0;
	for(x = 0; x <= g_argvsl; x++)
	{
		if(g_argvs[x] == 0)
		{
			int len = strlen(&g_argvs[i]);
			if(len)
			{
				if(g_argc == argva)
				{
					int newa = argva ?: 10;
					newa += newa * 2 + newa / 2;
					fatal(xrealloc, &g_argv, sizeof(*g_argv), newa, argva);
					argva = newa;
				}

				fatal(xmalloc, &g_argv[g_argc], len + 1);
				memcpy(g_argv[g_argc], &g_argvs[i], len);
				g_argc++;
				i = x + 1;
			}

			if(x < g_argvsl)
				g_argvs[x] = ' ';
		}
	}

	// process g_argv, splicing out recognized logger-options
	for(x = 0; x < g_argc; x++)
	{
		if(parsetest(g_argv[x], 0, 0))
		{
			if(o_logexprs_l == o_logexprs_a)
			{
				int ns = o_logexprs_a ?: 10;
				ns = ns * 2 + ns / 2;
				fatal(xrealloc, &o_logexprs, sizeof(*o_logexprs), ns, o_logexprs_a);
				o_logexprs_a = ns;
			}

			o_logexprs[o_logexprs_l++] = g_argv[x];
			g_argv[x] = 0;
		}
	}

	// shrink g_argv to size
	for(x = g_argc - 1; x >= 0; x--)
	{
		if(g_argv[x] == 0)
		{
			memmove(
			    &g_argv[x]
				, &g_argv[x+1]
				, (g_argc - x - 1) * sizeof(g_argv[0])
			);
			g_argc--;
		}
	}

finally:
	fatal(ixclose, &fd);
coda;
}

#if DEVEL
int log_config(uint64_t prefix, uint64_t trace)
{
	xproxy(logconfig, NOTRACE prefix, trace, 0);
}

int log_log_config_and_describe(TRACEARGS uint64_t prefix, uint64_t trace, uint64_t bits)
{
	xproxy(logconfig, TRACEPASS prefix, trace, bits);
}
#else
int log_config(uint64_t prefix)
{
	xproxy(logconfig, NOTRACE prefix, 0);
}

int log_log_config_and_describe(TRACEARGS uint64_t prefix, uint64_t bits)
{
	xproxy(logconfig, TRACEPASS prefix, bits);
}
#endif

int log_would(const uint64_t e)
{
	int r = 0;
	if((e & L_TAG) == 0)
	{
		r = 1;
	}
	else
	{
		int x;
		for(x = 0; x < o_filter_l; x++)
		{
			uint64_t rr = 0;
			if(o_filter[x].m == '(')
				rr = e & o_filter[x].v;
			if(o_filter[x].m == '{')
				rr = ((e & o_filter[x].v) == e);
			if(o_filter[x].m == '[')
				rr = ((e & o_filter[x].v) == o_filter[x].v);
			if(o_filter[x].m == '<')
				rr = (e == o_filter[x].v);

			if(rr)
			{
				if(o_filter[x].o == '+')
					r = 1;
				if(o_filter[x].o == '-')
					r = 0;
			}
		}
	}

	return r;
}

int log_log_start(TRACEARGS const uint64_t e)
{
	if(start(e))
	{
#if DEVEL
		// store trace
		prep(func, file, line);
#endif

		return 1;
	}

	return 0;
}

void log_finish()
{
	if(o_space_bits)
		finish();
}

int log_vlogf(TRACEARGS const uint64_t e, const char * const fmt, va_list va)
{
	if(o_space_bits)
	{
		if(log_would(o_space_bits))
			logvprintf(fmt, va);
	}
	else if(e && start(e))
	{
		// the message
		logvprintf(fmt, va);

#if DEVEL
		// store trace info
		prep(func, file, line);
#endif

		finish();
	}
	else
	{
		return 0;
	}

	return 1;
}

int log_logf(TRACEARGS const uint64_t e, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = log_vlogf(TRACEPASS e, fmt, va);
	va_end(va);

	return r;
}

int log_logs(TRACEARGS const uint64_t e, const char * const s)
{
	return log_logw(TRACEPASS e, s, strlen(s));
}

int log_logw(TRACEARGS const uint64_t e, const char * const src, size_t len)
{
	if(o_space_bits)
	{
		if(log_would(o_space_bits))
			logwrite(src, len, 1);
	}
	else if(e && start(e))
	{
		// the message
		logwrite(src, len, 1);

#if DEVEL
		// store trace info
		prep(func, file, line);
#endif

		finish();
	}
	else
	{
		return 0;
	}

	return 1;
}

int logged_bytes()
{
	return o_space_l;
}

int logged_characters()
{
	return o_space_w;
}

void log_teardown()
{
	free(o_space);
#if DEVEL
	free(o_trace_func);
	free(o_trace_file);
#endif
	free(o_filter);

	free(g_argvs);
	int x;
	for(x = 0; x < g_argc; x++)
		free(g_argv[x]);
	free(g_argv);

	for(x = 0; x < o_logexprs_l; x++)
		free(o_logexprs[x]);
	free(o_logexprs);
}
