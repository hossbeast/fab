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

#include "log.h"

#include "xstring.h"
#include "args.h"
#include "cksum.h"
#include "macros.h"

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

// logtag definitions
struct g_logs_t o_logs[] = {
	  { .v = L_ERROR		, .s = "ERROR"		, .d = "errors leading to shutdown" }
	, { .v = L_WARN			, .s = "WARN"			, .d = "nonfatal warnings" }
	, { .v = L_INFO			, .s = "INFO"			, .d = "program flow" }
	, { .v = L_ARGS			, .s = "ARGS"			, .d = "program arguments" }
	, { .v = L_PARAMS		, .s = "PARAMS"		, .d = "program execution parameters" }
#if DEVEL
	, { .v = L_FFTOKN		, .s = "FFTOKN"		, .d = "fabfile parsing - token stream" }
	, { .v = L_FFSTAT		, .s = "FFSTAT"		, .d = "fabfile parsing - lexer start conditions" }
	, { .v = L_FFTREE		, .s = "FFTREE"		, .d = "fabfile parsing - parsed tree" }
	, { .v = L_FFFILE		, .s = "FFFILE"		, .d = "fabfile parsing - parsed file" }
	, { .v = L_FF				, .s = "FF"				, .d = "fabfile parsing" }
#endif
	, { .v = L_BPINFO		, .s = "BPINFO"		, .d = "buildplan - flow" }
	, { .v = L_BPEXEC		, .s = "BPEXEC"		, .d = "buildplan - execution" }
	, { .v = L_BPEVAL		, .s = "BPEVAL"		, .d = "buildplan - pruning/evaluating" }
	, { .v = L_BPDUMP		, .s = "BPDUMP"		, .d = "buildplan - dump the final buildplan" }
	, { .v = L_BP				, .s = "BP"				, .d = "buildplan" }
	, { .v = L_FMLEXEC	, .s = "FMLEXEC"	, .d = "formulas - execution results/details" }
#if DEVEL
	, { .v = L_FMLTARG	, .s = "FMLTARG"	, .d = "formulas - target resolution/assignment" }
#endif
	, { .v = L_FML			, .s = "FML"			, .d = "formulas" }
	, { .v = L_FAB			, .s = "FAB"			, .d = "fabrication formulas" }
	, { .v = L_DSCINFO	, .s = "DSCINFO"	, .d = "dependency discovery - flow" }
	, { .v = L_DSCEXEC	, .s = "DSCEXEC"	, .d = "dependency discovery - execution" }
	, { .v = L_DSCNEW		, .s = "DSCNEW"		, .d = "dependency discovery - new nodes/edges" }
	, { .v = L_DSC			, .s = "DSC"			, .d = "dependency discovery" }
#if DEVEL
	, { .v = L_DGDEPS		, .s = "DGDEPS"		, .d = "dependency graph - dependencies" }
#endif
	, { .v = L_DGRAPH		, .s = "DGRAPH"		, .d = "dependency graph - dump/details" }
	, { .v = L_DG				, .s = "DG"				, .d = "dependency graph" }
	, { .v = L_VARAUTO	, .s = "VARAUTO"	, .d = "variable definitions - automatic" }
	, { .v = L_VARUSER	, .s = "VARUSER"	, .d = "variable definitions - user-defined" }
	, { .v = L_VAR			, .s = "VAR"			, .d = "variable definitions" }
	, { .v = L_INVOKE		, .s = "INVOKE"		, .d = "fabfile invocations" }
	, { .v = L_SELECT		, .s = "SELECT"		, .d = "node selectors" }
	, { .v = L_LISTS		, .s = "LISTS"		, .d = "node lists" }
	, { .v = L_INVALID	, .s = "INVALID"	, .d = "node invalidation" }
#if DEBUG
	, { .v = L_LWVOCAL	, .s = "LWVOCAL"	, .d = " ** VERBOSE ** liblistwise invocations" }
#endif
};

struct g_logs_t * g_logs = o_logs;

int g_logs_l = sizeof(o_logs) / sizeof(o_logs[0]);

//
// [[ static ]]
//

static char * 	o_space;			// storage between vadd/flush calls
static int			o_space_l;
static int			o_space_a;

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
	// determine logtag len
	int x;
	for(x = 0; x < g_logs_l; x++)
	{
		g_logs[x].l = strlen(g_logs[x].s);
		o_name_len = MAX(o_name_len, g_logs[x].l);
	}
}

static int logwrite(const char * s, size_t l)
{
	if((o_space_l + l) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + l + 1);
		o_space_a = o_space_l + l + 1;
	}

	memcpy(o_space + o_space_l, s, l);
	o_space_l += l;

	return l;
}

static int logvprintf(const char * fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);
	
	int size = vsnprintf(0, 0, fmt, va2);
	va_end(va2);

	if((o_space_l + size) >= o_space_a)
	{
		o_space = realloc(o_space, o_space_l + size + 1);
		o_space_a = o_space_l + size + 1;
	}

	vsprintf(o_space + o_space_l, fmt, va);

	o_space_l += size;

	return size;
}

static int logprintf(const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	int R = logvprintf(fmt, va);
	va_end(va);

	return R;
}

static int start(const uint64_t e)
{
	int R = 0;

	// colorization
	if((e & L_COLOR_VALUE) && COLORHEX(e))
	{
		logwrite(COLOR(e));
	}

	// prefix
	int x;
	for(x = 0; x < g_logs_l; x++)
	{
		if((e & g_logs[x].v) == g_logs[x].v)
		{
			R = logprintf("%*s : ", o_name_len, g_logs[x].s);
			break;
		}
	}

	return R;
}

static int vstart(const uint64_t e)
{
	if(log_would(e))
	{
		o_space_l = 0;
		o_e = e;

		return start(e);
	}

	return 0;
}

static int vadd(const char* fmt, va_list va)
{
	return logvprintf(fmt, va);
}

static int vfinish(const char* fmt, void * va)
{
	int R = 0;
	if(fmt)
		R = vadd(fmt, *(va_list*)va);

	if((o_e & L_COLOR_VALUE) && COLORHEX(o_e))
	{
		logwrite(NOCOLOR);
	}

	logwrite("\n", 1);

	// flush to stderr
	int __attribute__((unused)) r = write(2, o_space, o_space_l);

	o_e = 0;

	return R;
}

static void describe(struct filter * f)
{
	char buf[256];

	char * s = buf;
	size_t z = sizeof(buf);

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

	log(L_INFO, "%.*s", l, s);
}

//
// [[ public ]]
//

void log_parse(char * args, int args_len)
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

						if(xstrcmp(&args[x+off], k-off, "TAG", 3, 0) == 0)
						{
							// special case for TAG to exclude LWVOCAL
							tag |= L_TAG & ~L_LWVOCAL;
							off += 3;
						}
						else
						{
							int i;
							for(i = 0; i < g_logs_l; i++)
							{
								if(xstrcmp(&args[x+off], k-off, g_logs[i].s, g_logs[i].l, 0) == 0)
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

							o_filter = realloc(o_filter, sizeof(*o_filter) * ns);
							o_filter_a = ns;
						}

						struct filter * f = &o_filter[o_filter_l++];

						f->v = tag & L_TAG;
						f->o = args[x];
						f->m = lhs == ' ' ? '(' : lhs;

						describe(f);
					}
				}
			}
		}
	}
}

int log_init(char * str)
{
	// apply initial args string
	log_parse(str, 0);

	// parse cmdline
	int fd = open("/proc/self/cmdline", O_RDONLY);

	char *	args = 0;
	int			args_len = 0;
	int			nlen = 0;

	do
	{
		size_t newlen = args_len ? (args_len + (args_len / 2 ) + (args_len * 2)) : 10;

		args = realloc(args, newlen);
		nlen = newlen;

	} while((args_len += (read(fd, &args[args_len], nlen - args_len))) == nlen);

	close(fd);

	args_len--;
	int x;
	for(x = 0; x < args_len; x++)
	{
		if(args[x] == 0)
			args[x] = ' ';
	}

	// parse cmdline args
	log_parse(args, args_len);

	free(args);

	return 0;
}

int log_would(const uint64_t bits)
{
	if(o_filter_l == 0)
		return 1;

	int r = 0;
	int x;
	for(x = 0; x < o_filter_l; x++)
	{
		uint64_t rr = 0;
		if(o_filter[x].m == '(')
			rr = bits & o_filter[x].v;
		if(o_filter[x].m == '{')
			rr = ((bits & o_filter[x].v) == bits);
		if(o_filter[x].m == '[')
			rr = ((bits & o_filter[x].v) == o_filter[x].v);
		if(o_filter[x].m == '<')
			rr = (bits == o_filter[x].v);

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

int vlog_start(const uint64_t e, const char* fmt, va_list va) 
{
	int R = 0;
	if((R = vstart(e)))
	{
		return R + vadd(fmt, va);
	}

	return 0;
}

int log_start(const uint64_t e, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vlog_start(e, fmt, va);
	va_end(va);

	return r;
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
	int r = vlog_add(fmt, va);
	va_end(va);

	return r;
}

int vlog_finish(const char * fmt, va_list va)
{
	if(log_would(o_e))
	{
		return vfinish(fmt, &va);
	}

	return 0;
}

int log_finish(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vlog_finish(fmt, va);
	va_end(va);

	return r;
}

int vlog_trace(const uint64_t e, const char * const fmt, const char * const function, const char * const file, int line, va_list va)
{
	if(vstart(e))
	{
		vadd(fmt, va);
		log_add(" in %s", function);

		if(UNWIND_ERRORS)
			log_add(" at %s:%d", file, line);

		return vfinish(0, 0);
	}

	return 0;
}

int log_trace(const uint64_t e, const char * const fmt, const char * const function, const char * const file, int line, ...)
{
	va_list va;
	va_start(va, line);
	int r = vlog_trace(e, fmt, function, file, line, va);
	va_end(va);

	return r;
}

int vlog(const uint64_t e, const char* fmt, va_list va)
{
	if(vstart(e))
	{
		return vfinish(fmt, &va);
	}

	return 0;
}

int log(const uint64_t e, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int r = vlog(e, fmt, va);
	va_end(va);

	return r;
}

int log_written()
{
	return o_space_l;
}

void log_teardown()
{
	free(o_space);
	free(o_filter);
}
