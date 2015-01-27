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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#if __linux__
#include <linux/auxvec.h>
#endif

/*
exposes getauxval, but requires glibc 2.16. ubuntu 12.10 has glibc 2.15.
#include <sys/auxv.h>
*/

#include "xlinux.h"
#include "xapi.h"

#include "logger.h"
#include "macros.h"
#include "strutil.h"

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 60])
#define COLOR(x)		(char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

#if DEBUG || DEVEL
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

char **			g_argv;
int					g_argc;
char *			g_argvs;
int					g_argvsl;
char *			g_binary;
char *			g_interpreting;
char **			g_logv;
static int	g_logva;
int					g_logc;
char *			g_logvs;
int					g_logvsl;

//
// [[ static ]]
//

static uint64_t o_space_bits;	// when a log is being constructed, effective bits
static char * 	o_space;			// storage between logvprintf/flush calls
static int			o_space_a;		// allocation
static int			o_space_l;		// bytes
static int			o_space_w;		// visible characters

static uint64_t	o_prefix_bits = ~0ULL;	// prefix decision bits

#if DEBUG || DEVEL
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
		o_space_bits = e;
		o_space_w = 0;
		o_space_l = 0;
#if DEBUG || DEVEL
		o_trace_func_l = 0;
		o_trace_file_l = 0;
#endif

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

#if DEBUG || DEVEL
	// location trace for errors
	if(o_space_bits & o_trace_bits)
		w += logprintf(" in %s at %s:%d", o_trace_func, o_trace_file, o_trace_line);
#endif

	if((o_space_bits & L_COLOR_VALUE) && COLORHEX(o_space_bits))
	{
		logwrite(NOCOLOR, 0);
	}

	logwrite("\n", 1, 0);

	// flush to stdout
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

#if DEBUG || DEVEL
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
// RETURNS
//  number of characters parsed
//
static int parsetest(char * expr, int exprl, struct filter * f)
{
	if(expr)
	{
		exprl = exprl ?: strlen(expr);

		// skip leading whitespace
		int off = 0;
		while(exprl > off && (expr[off] == ' ' || expr[off] == '\t'))
			off++;

		if(exprl > off && (expr[off] == '+' || expr[off] == '-'))
		{
			char o = expr[off];
			char lhs = ' ';
			off++;

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
								, .o = o
								, .m = lhs == ' ' ? '(' : lhs
							};
						}

						return off;
					}
				}
			}
		}
	}

	return 0;
}

static int logparse(TRACEARGS char * expr, int exprl, int prepend, uint64_t bits)
{
	char space[512];

	struct filter f;

	exprl = exprl ?: strlen(expr);

	int off;
	while((off = parsetest(expr, exprl, &f)))
	{
		if(o_filter_a == o_filter_l)
		{
			size_t ns = o_filter_a ?: 3;
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

		if(bits)
		{
			size_t sz = describe(&f, space, sizeof(space));
			if(sz)
			{
				log_logf(TRACEPASS bits, "%.*s", sz, space);
			}
		}

		expr += off;
		exprl -= off;
	}

	finally : coda;
}

static int logconfig(TRACEARGS uint64_t prefix
#if DEBUG || DEVEL
	, uint64_t trace
#endif
	, uint64_t bits
)
{
#if DEBUG || DEVEL
	o_trace_bits = trace;
#endif
	o_prefix_bits = prefix;

	// parse logexprs
	int x;
	for(x = 0; x < g_logc; x++)
	{
		fatal(logparse, TRACEPASS g_logv[x], 0, 0, bits);
	}

	finally : coda;
}

//
// [[ public ]]
//

int log_parse(char * expr, int expr_len, int prepend)
{
	xproxy(logparse, NOTRACE expr, expr_len, prepend, 0);
}

int log_log_parse_and_describe(TRACEARGS char * expr, int expr_len, int prepend, uint64_t bits)
{
	xproxy(logparse, TRACEPASS expr, expr_len, prepend, bits);
}

int log_parse_pop()
{
	o_filter_l--;

	finally : coda;
}

int log_parse_clear()
{
	o_filter_l = 0;

	finally : coda;
}

int log_init()
{
	int	fd = -1;
	int	argsa = 0;	// g_argvs allocated size
	int	argva = 0;
#if __linux__
	char * auxv = 0;
	size_t auxvl = 0;
	size_t auxva = 0;
#endif
	int x;
	int y;
	int i;

	// snarf the cmdline
	fatal(xopen, "/proc/self/cmdline", O_RDONLY, &fd);

	// read into g_argvs - single string containing entire cmdline
	int binaryx = -1;
	int interpx = -1;
	char * execfn = 0;
	do
	{
		size_t newa = argsa ?: 100;
		newa += newa * 2 + newa / 2;
		fatal(xrealloc, &g_argvs, sizeof(*g_argvs), newa, argsa);
		argsa = newa;
		g_argvsl += read(fd, &g_argvs[g_argvsl], argsa - g_argvsl);
	} while(g_argvsl == argsa);
	g_argvsl--;

	// locate binary and interpreter, if any
	for(x = -1; x < g_argvsl; x = y)
	{
		for(y = x + 1; y < g_argvsl; y++)
		{
			if(g_argvs[y] == 0)
				break;
		}

		// the binary is the first argument
		if(binaryx == -1)
		{
			binaryx = x + 1;
#if __linux__
			/*
			requires glibc 2.16. ubuntu 12.10 has glibc 2.15.
			execfn = (char*)(intptr_t)getauxval(AT_EXECFN);
			*/

			fatal(ixclose, &fd);
			fatal(xopen, "/proc/self/auxv", O_RDONLY, &fd);
			do
			{
				size_t newa = auxva ?: 100;
				newa += newa * 2 + newa / 2;
				fatal(xrealloc, &auxv, sizeof(*auxv), newa, auxva);
				auxva = newa;
				auxvl += read(fd, &auxv[auxvl], auxva - auxvl);
			} while(auxvl == auxva);
			auxvl--;

			unsigned long * auxvec = (void*)auxv;
			while(*auxvec)
			{
				unsigned long key = auxvec[0];
				unsigned long val = auxvec[1];
				if(key == AT_EXECFN)
				{
					execfn = (char*)(uintptr_t)val;
					break;
				}
				auxvec += 2;
			}
#endif
		}

		// when interpreting, AT_EXECFN is the interpreter script
		else if(interpx == -1 && execfn && strcmp(&g_argvs[x + 1], execfn) == 0)
		{
			interpx = x + 1;
		}
	}

#if __linux__
	/*
	* on linux, an interpreter script is executed with the optional-arg passed as a
	* single string. break it up on spaces, so argument parsing will see separate args
	*  see : http://man7.org/linux/man-pages/man2/execve.2.html
	*
	* getauxval is probably not available on all unices
	*/
	for(x = binaryx + strlen(&g_argvs[binaryx]) + 1; x < (interpx - 1); x++)
	{
		// process the optional-arg in a linux-specific way
		if(g_argvs[x] == 0x20)
			g_argvs[x] = 0;
	}
#endif

#if 0
for(x = 0; x < g_argvsl; x++)
{
	printf("[%02d] 0x%02hhx", x, g_argvs[x]);
	if(g_argvs[x] > 0x20 && g_argvs[x] < 0x7f)
		printf(" %c", g_argvs[x]);
	printf("\n");
}
#endif

	// 1. replace nulls with spaces in g_argvs
	// 2. construct g_argv, array of arguments
	i = 0;
	for(x = 0; x <= g_argvsl; x++)
	{
		if(g_argvs[x] == 0)
		{
			int len = strlen(&g_argvs[i]);
			if(len)
			{
				if(g_argc == argva)
				{
					size_t newa = argva ?: 10;
					newa += newa * 2 + newa / 2;
					fatal(xrealloc, &g_argv, sizeof(*g_argv), newa, argva);
					argva = newa;
				}

				fatal(xmalloc, &g_argv[g_argc], len + 1);
				memcpy(g_argv[g_argc], &g_argvs[i], len);

				if(binaryx == i)
					g_binary = g_argv[g_argc];
				else if(interpx == i)
					g_interpreting = g_argv[g_argc];

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
			if(g_logc == g_logva)
			{
				int ns = g_logva ?: 10;
				ns = ns * 2 + ns / 2;
				fatal(xrealloc, &g_logv, sizeof(*g_logv), ns, g_logva);
				g_logva = ns;
			}

			g_logv[g_logc++] = g_argv[x];
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

	g_logvsl = 0;
	for(x = 0; x < g_logc; x++)
	{
		if(x)
			g_logvsl++;
		g_logvsl += strlen(g_logv[x]);
	}

	fatal(xmalloc, &g_logvs, g_logvsl + 1);

	for(x = 0; x < g_logc; x++)
	{
		if(x)
			strcat(g_logvs, " ");
		strcat(g_logvs, g_logv[x]);
	}

#if 0
printf("args : %s\n", g_argvs);
for(x = 0; x < g_argc; x++)
{
	printf("[%2d] %s", x, g_argv[x]);
	if(g_binary == g_argv[x])
		printf(" * binary");
	else if(g_interpreting == g_argv[x])
		printf(" * interpreting");
	printf("\n");
}

printf("logs : %s\n", g_logvs);
for(x = 0; x < g_logc; x++)
	printf("[%2d] %s\n", x, g_logv[x]);
#endif

finally:
	fatal(ixclose, &fd);
coda;
}

#if DEBUG || DEVEL
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
				rr = e & o_filter[x].v & L_TAG;
			if(o_filter[x].m == '{')
				rr = ((e & o_filter[x].v & L_TAG) == (e & L_TAG));
			if(o_filter[x].m == '[')
				rr = ((e & o_filter[x].v & L_TAG) == (o_filter[x].v & L_TAG));
			if(o_filter[x].m == '<')
				rr = ((e & L_TAG) == (o_filter[x].v & L_TAG));

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
#if DEBUG || DEVEL
		// store trace
		prep(func, file, line);
#endif

		return 1;
	}

	return 0;
}

void log_finish()
{
	if(log_would(o_space_bits))
		finish();

	o_space_bits = 0;
}

void log_vlogf(TRACEARGS const uint64_t e, const char * const fmt, va_list va)
{
	if(o_space_bits)
	{
		if(log_would(o_space_bits))
			logvprintf(fmt, va);
	}
	else if(start(e))
	{
		// the message
		logvprintf(fmt, va);

#if DEBUG || DEVEL
		// store trace info
		prep(func, file, line);
#endif

		finish();
	}
}

void log_logf(TRACEARGS const uint64_t e, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	log_vlogf(TRACEPASS e, fmt, va);
	va_end(va);
}

void log_logs(TRACEARGS const uint64_t e, const char * const s)
{
	log_logw(TRACEPASS e, s, strlen(s));
}

void log_logw(TRACEARGS const uint64_t e, const char * const src, size_t len)
{
	if(o_space_bits)
	{
		if(log_would(o_space_bits))
			logwrite(src, len, 1);
	}
	else if(start(e))
	{
		// the message
		logwrite(src, len, 1);

#if DEBUG || DEVEL
		// store trace info
		prep(func, file, line);
#endif

		finish();
	}
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
#if DEBUG || DEVEL
	free(o_trace_func);
	free(o_trace_file);
#endif
	free(o_filter);

	free(g_argvs);
	int x;
	for(x = 0; x < g_argc; x++)
		free(g_argv[x]);
	free(g_argv);

	for(x = 0; x < g_logc; x++)
		free(g_logv[x]);
	free(g_logv);
}
