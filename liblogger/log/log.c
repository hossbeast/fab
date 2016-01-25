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

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "internal.h"
#include "log.h"

#include "macros.h"
#include "strutil.h"
#include "color.h"

#define restrict __restrict

#define COLORHEX(x)	(o_colors[(x & L_COLOR_VALUE) >> 60])
#define COLOR(x)		(char[7]){ 0x1b, 0x5b, 0x31, 0x3b, 0x33, COLORHEX(x), 0x6d }, 7
#define NOCOLOR			(char[6]){ 0x1b, 0x5b, 0x30, 0x3b, 0x30             , 0x6d }, 6

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
static uint64_t	  o_trace_bits  = 0ULL;		// trace decision bits
static pstring *  o_trace_func; // trace storage
static pstring *  o_trace_file;
static int			  o_trace_line;
#endif

static unsigned char o_colors[] = {
	/* NO COLOR */   0x00
	/* L_RED 		*/ , 0x31
	/* L_GREEN	*/ , 0x32
	/* L_YELLOW */ , 0x33
	/* L_CYAN 	*/ , 0x36
	/* L_BLUE 	*/ , 0x34
};

/*
** w specifies whether this segment comprises "visible chars", or not
*/

static int logwrite(const char * s, size_t l, int w)
{
	if((o_space_l + l) >= o_space_a)
	{
    fatal(xrealloc, &o_space, o_space_l + l + 1);
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

/// start
//
// SUMMARY
//  store the log prefix if log_would
//
// PARAMETERS
//  e - log bits
//  w - (returns) whether log_would
//
static xapi start(const uint64_t e, int * const restrict w)
{
  enter;

  (*w) = 0;
	if(log_would(e))
	{
		o_space_bits = e;
		o_space_w = 0;
		o_space_l = 0;
#if DEBUG || DEVEL
    fatal(psclear, &o_trace_func);
    fatal(psclear, &o_trace_file);
#endif

		// colorization
		if((e & L_COLOR_VALUE) && COLORHEX(e))
		{
      sayw(COLOR(e), 0);     
		}

		if(o_space_bits & o_prefix_bits)
		{
			// prefix
			int x;
			for(x = 0; x < g_logs_l; x++)
			{
				if((e & g_logs[x].v) == g_logs[x].v)
				{
          sayf("%*s : ", o_name_len, g_logs[x].s);
					break;
				}
			}
		}

    (*w) = 1;
	}

  finally : coda;
}

static xapi finish()
{
  enter;

#if DEBUG || DEVEL
	// location trace for errors
	if(o_space_bits & o_trace_bits)
		logprintf(" in %s at %s:%d", o_trace_func->s, o_trace_file->s, o_trace_line);
#endif

	if((o_space_bits & L_COLOR_VALUE) && COLORHEX(o_space_bits))
	{
    sayw(COLOR(e), 0);
	}

  says("\n");

	// flush to stdout
	int __attribute__((unused)) r = write(2, o_space, o_space_l);

	o_space_bits = 0;

  finally : coda;
}

#if DEBUG || DEVEL
static xapi trace_store(const char * const func, const char * file, int line)
{
  enter;

	char * slash = 0;
	if((slash = strrchr(file, '/')))
		file = slash + 1;
  fatal(psloads, &o_trace_func, func);
  fatal(psloads, &o_trace_file, file);

	o_trace_line = line;

  finally : coda;
}
#endif

//
// api
//

xapi logger_vlogf(const uint64_t e, const char * const fmt, va_list va)
{
  enter;

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
		trace_store(func, file, line);
#endif

		fatal(finish);
	}

  finally : coda;
}

xapi logger_logf(const uint64_t e, const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	log_vlogf(e, fmt, va);
	va_end(va);
}

xapi logger_logs(const uint64_t e, const char * const s)
{
	log_logw(e, s, strlen(s));
}

xapi logger_logw(const uint64_t e, const char * const src, size_t len)
{
  enter;

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
		fatal(trace_store, func, file, line);
#endif

		fatal(finish);
	}

  finally : coda;
}

xapi logger_log_start(const uint64_t e)
{
  enter;

  int w;
  fatal(start, e, &w);

#if DEBUG || DEVEL
  if(w)
	{
		// store trace
		fatal(trace_store, func, file, line);
	}
#endif

  finally : coda;
}

xapi logger_log_finish()
{
  enter;

	if(log_would(o_space_bits))
		fatal(finish);

	o_space_bits = 0;

  finally : coda;
}

int logger_log_would(const uint64_t e)
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

int logger_log_bytes()
{
	return o_space_l;
}

int logger_log_chars()
{
	return o_space_w;
}
