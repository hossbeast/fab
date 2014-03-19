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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "logger.h"
#include "xlinux.h"
#include "xapi.h"
#if 0
#include "global.h"
#include "args.h"
#endif
#include "cksum.h"
#include "macros.h"
#include "strutil.h"

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 60])
#define COLOR(x)		(char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

static struct filter
{
	uint64_t	v;		// tag
	char			m;		// mode, (, [, {, or <
	char			o;		// operation, + or -
} * 							o_filter;
static int				o_filter_a;
static int				o_filter_l;

static uint64_t o_e;						// when a log is being constructed, effective bits

struct g_logs_t * g_logs;
int g_logs_l;

//
// [[ static ]]
//

static char * 	o_space;			// storage between vadd/flush calls
static int			o_space_a;		// allocation
static int			o_space_l;		// bytes
static int			o_space_w;		// visible characters

#if DEBUG
static uint64_t	o_trace_bits;

static char *		o_func;				// trace storage
static int			o_func_a;
static int			o_func_l;
static char * 	o_file;
static int			o_file_a;
static int			o_file_l;
static int			o_line;
#endif

static int o_name_len;

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
	g_logs = o_logs;
	g_logs_l = o_logs_l;

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

static int start(const uint64_t e, int pfx)
{
	if(log_would(e))
	{
#if DEBUG
		o_space_l = 0;
		o_space_w = 0;
		o_func_l = 0;
		o_file_l = 0;
#endif
		o_e = e;

		int w = 0;

		// colorization
		if((e & L_COLOR_VALUE) && COLORHEX(e))
		{
			logwrite(COLOR(e), 0);
		}

		if(pfx)
		{
			// prefix
			int x;
			for(x = 0; x < g_logs_l; x++)
			{
				if((e & g_logs[x].v) == g_logs[x].v)
				{
					w += logprintf("%*s : ", o_name_len, g_logs[x].s);
					break;
				}
			}
		}

		return w;
	}

	return 0;
}

static int vadd(const char* fmt, va_list va)
{
	return logvprintf(fmt, va);
}

static int vfinish(const char* fmt, void * va, int trace)
{
	int w = 0;
	if(fmt)
		w += vadd(fmt, *(void**)va);

#if DEBUG
	if(trace)
	{
		// location trace for errors
		if(o_e & o_trace_bits)
			w += log_add(" in %s at %s:%d", o_func, o_file, o_line);
	}
#endif

	if((o_e & L_COLOR_VALUE) && COLORHEX(o_e))
	{
		logwrite(NOCOLOR, 0);
	}

	logwrite("\n", 1, 0);

	// flush to stderr
	int __attribute__((unused)) r = write(2, o_space, o_space_l);

	o_e = 0;

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

#if DEBUG
static void prep(const char * const func, const char * const file, int line)
{
	int funcl = strlen(func);
	int filel = strlen(file);

	if(o_func_a <= funcl)
	{
		o_func = realloc(o_func, funcl + 1);
		o_func_a = funcl + 1;
	}
	memcpy(o_func, func, funcl);
	o_func[funcl] = 0;

	if(o_file_a <= filel)
	{
		o_file = realloc(o_file, filel + 1);
		o_file_a = filel + 1;
	}
	memcpy(o_file, file, filel);
	o_file[filel] = 0;

	o_line = line;
}
#endif

//
// [[ public ]]
//

static int logparse(char * args, int args_len, char * dst, size_t sz, size_t * z)
{
	args_len = args_len ?: strlen(args);

	int x;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == '+' || args[x] == '-')
		{
			char lhs = ' ';
			int off = 1;

			if((x + off) < args_len)
			{
				if(args[x+off] == '(')
					lhs = '(';
				else if(args[x+off] == '{')
					lhs = '{';
				else if(args[x+off] == '[')
					lhs = '[';
				else if(args[x+off] == '<')
					lhs = '<';

				if(lhs != ' ')
					off++;

				if((x + off) < args_len)
				{
					int y;
					for(y = off; (x+y) < args_len; y++)
					{
						if(lhs == ' ' && args[x+y] == ' ')
							break;
						else if(lhs == '(' && args[x+y] == ')')
							break;
						else if(lhs == '{' && args[x+y] == '}')
							break;
						else if(lhs == '[' && args[x+y] == ']')
							break;
						else if(lhs == '<' && args[x+y] == '>')
							break;
					}

					uint64_t tag = 0;

					int k = 0;
					while(k != y)
					{
						if(args[x+off] == ',' || args[x+off] == '|')
							off++;

						for(k = off; k < y; k++)
						{
							if(args[x+k] == ',' || args[x+k] == '|')
								break;
						}

						if(estrcmp(&args[x+off], k-off, "TAG", 3, 0) == 0)
						{
							// special case : TAG excludes liblistwise-specific tags
//							tag |= L_TAG & ~(L_LWTOKEN | L_LWSTATE | L_LWPARSE | L_LWEXEC | L_LWSANITY);
							off += 3;
						}
						else
						{
							int i;
							for(i = 0; i < g_logs_l; i++)
							{
								if(estrcmp(&args[x+off], k-off, g_logs[i].s, g_logs[i].l, 0) == 0)
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
						if(o_filter_a == o_filter_l)
						{
							int ns = 3;
							if(o_filter_a)
								ns = o_filter_a * 2 + o_filter_a / 2;

							fatal(xrealloc, &o_filter, sizeof(*o_filter), ns, o_filter_a);
							o_filter_a = ns;
						}

						struct filter * f = &o_filter[o_filter_l++];

						f->v = tag & L_TAG;
						f->o = args[x];
						f->m = lhs == ' ' ? '(' : lhs;

						if(dst && z)
							(*z) = describe(f, dst, sz);
						else if(dst)
							describe(f, dst, sz);
					}
				}
			}
		}
	}

	finally : coda;
}

int log_parse(char * args, int args_len)
{
	xproxy(logparse, args, args_len, 0, 0, 0);
}

int log_parse_and_describe(char * args, int args_len, uint64_t bits)
{
	char space[256];
	size_t sz = 0;

	fatal(logparse, args, args_len, space, sizeof(space), &sz);

	log(bits, "%.*s", sz, space);

	finally : coda;
}

static int loginit(uint64_t trace, char * dst, size_t sz, size_t * z)
{
	int			fd = -1;
	char *	args = 0;
	int			args_len = 0;
	int			nlen = 0;

	// save trace settings
	o_trace_bits = trace;

	// parse cmdline
	fatal(xopen, "/proc/self/cmdline", O_RDONLY, &fd);

	do
	{
		size_t newlen = args_len ? (args_len + (args_len / 2 ) + (args_len * 2)) : 10;
		fatal(xrealloc, &args, 1, newlen, nlen);
		nlen = newlen;

	} while((args_len += (read(fd, &args[args_len], nlen - args_len))) == nlen);

	args_len--;
	int x;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == 0)
			args[x] = ' ';
	}

	// parse cmdline args
	fatal(logparse, args, args_len, dst, sz, z);

finally:
	free(args);
	fatal(ixclose, &fd);
coda;
}

int log_init(uint64_t trace)
{
	xproxy(loginit, trace, 0, 0, 0);
}

int log_init_and_describe(uint64_t trace, uint64_t bits)
{
	char space[256];
	size_t sz = 0;

	fatal(loginit, trace, space, sizeof(space), &sz);

	log(bits, "%.*s", sz, space);

	finally : coda;
}

int log_would(const uint64_t e)
{
	if(o_filter_l == 0)
		return 1;

	int r = 0;
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

	return r;
}

int vlog_starti(
#if DEBUG
const char * const func, const char * const file, int line,
#endif
const uint64_t e, const char* fmt, va_list va)
{
	int w = 0;
	if((w = start(e, 1)))
	{
#if DEBUG
		// store trace
		prep(func, file, line);
#endif

		return w + vadd(fmt, va);
	}

	return 0;
}

int log_starti(
#if DEBUG
const char * const func, const char * const file, int line,
#endif
const uint64_t e, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
#if DEBUG
	int w = vlog_starti(func, file, line, e, fmt, va);
#else
	int w = vlog_starti(e, fmt, va);
#endif
	va_end(va);

	return w;
}

int vlog_add(const char* fmt, va_list va) 
{
	if(log_would(o_e))
	{
		return vadd(fmt, va);
	}

	return 0;
}

int log_add(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int w = vlog_add(fmt, va);
	va_end(va);

	return w;
}

int vlog_finish(const char * fmt, va_list va)
{
	if(log_would(o_e))
	{
		return vfinish(fmt, &va, 1);
	}

	return 0;
}

int log_finish(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int w = vlog_finish(fmt, va);
	va_end(va);

	return w;
}

int vlogi(
#if DEBUG
const char * const func, const char * const file, int line,
#endif
const uint64_t e, const char * const fmt, va_list va)
{
	int w = 0;
	if((w = start(e, 1)))
	{
		// error message
		w += vadd(fmt, va);

#if DEBUG
		// store trace info
		prep(func, file, line);
#endif

		return w + vfinish(0, 0, 1);
	}

	return 0;
}

int logi(
#if DEBUG
const char * const func, const char * const file, int line,
#endif
const uint64_t e, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);
#if DEBUG
	int w = vlogi(func, file, line, e, fmt, va);
#else
	int w = vlogi(e, fmt, va);
#endif
	va_end(va);

	return w;
}

void log_writei(
#if DEBUG
const char * const func, const char * const file, int line,
#endif
const uint64_t e, const char * const src, size_t len)
{
	if(log_would(e))
	{
		start(e, 0);

#if DEBUG
		prep(func, file, line);
#endif

		// colorization
		if((e & L_COLOR_VALUE) && COLORHEX(e))
		{
			logwrite(COLOR(e), 0);
		}

		logwrite(src, len, 0);
		vfinish(0, 0, 0);
	}
}

int logged_bytes()
{
	return o_space_l;
}

int logged_chars()
{
	return o_space_w;
}

void log_teardown()
{
	free(o_space);
#if DEBUG
	free(o_func);
	free(o_file);
#endif
	free(o_filter);
}
