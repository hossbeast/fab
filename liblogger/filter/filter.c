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

#include "xapi.h"
#include "xlinux.h"
#include "narrate.h"

#include "internal.h"
#include "filter.internal.h"
#include "category.internal.h"

#include "list.h"
#include "macros.h"
#include "strutil.h"

#define restrict __restrict

typedef struct filter
{
	uint64_t	v;		// tag
	char			o;		// operation, + or -
} filter;

// active filters
static list * active;

//
// [[ static ]]
//

#if 0
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

static xapi logparse(char * expr, int exprl, int prepend, uint64_t bits)
{
  enter;

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

static xapi logconfig(uint64_t prefix, uint64_t bits)
{
  enter;

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
#endif

static xapi __attribute__((nonnull)) filter_parse(const char * const restrict expr, size_t exprl, filter * const restrict res)
{
  enter;

  exprl = exprl ?: strlen(expr);

  // skip leading whitespace
  int off = 0;
  while(exprl > off && (expr[off] == ' ' || expr[off] == '\t'))
    off++;

  if(exprl > off && (expr[off] == '+' || expr[off] == '-'))
  {
    char o = expr[off];
    off++;

    if(off < exprl)
    {
      int y;
      for(y = off; y < exprl; y++)
      {
        if(expr[y] == ' ' || expr[y] == '\t')
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

        if(estrcmp(&expr[off], k-off, "ALL", 3, 0) == 0)
        {
          tag |= L_ALL;
          off += 3;
        }
        else
        {
          logger_category * category = 0;
          fatal(category_byname, &expr[off], k - off, &category);

          if(category == 0)
            break; // nothing

          tag |= category->id;
          off += category->namel;
        }
      }

      if(tag && (off == y))
      {
        (*res) = (typeof(*res)){
            v : tag & L_ALL
          , o : o
        };

        return off;
      }
    }
  }

  finally : coda;
}

#if 0
static void filter_free(filter * filter)
{
  free(filter);
}

static void filter_xfree(filter ** filter)
{
  filter_free(*filter);
  *filter = 0;
}
#endif

static void filter_destructor(filter * const restrict filterp)
{
  free(filterp);
}

//
// [[ public ]]
//

xapi filter_setup()
{
  enter;

  fatal(list_create, &active, 0, filter_destructor, LIST_SECONDARY);

  finally : coda;
}

void filter_teardown()
{
  list_xfree(&active);
}

xapi filter_say(struct filter * filterp, struct narrator * _narrator)
{
  enter;

  sayc(filterp->o);

  uint64_t mask = filterp->v;
  uint64_t id = UINT64_C(1);
  while(mask)
  {
    if(mask & id)
    {
      logger_category * category = 0;
      fatal(category_byid, mask & id, &category);
      if(category)
      {
        sayf("%.*s", category->namel, category->name);
      }

      mask &= ~id;
    }
    id <<= 1;
  }

  finally : coda;
}

//
// api
//

API xapi logger_filter_push(const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp = 0;
  fatal(filter_parse, expr, exprl, &filterp);
  fatal(list_push, active, &filterp);
  filterp = 0;
  
finally:
  filter_free(filterp);
coda;
}

API xapi logger_filter_unshift(const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp;
  fatal(filter_parse, expr, exprl, &filterp);
  fatal(list_unshift, active, &filterp);
  filterp = 0;
  
finally:
  filter_free(filterp);
coda;
}

API xapi logger_filter_pop()
{
  enter;

  list_pop(active);
  
  finally : coda;
}

API xapi logger_filter_shift()
{
  enter;

  list_shift(active);
  
  finally : coda;
}

API xapi logger_filter_clear()
{
  enter;

  list_clear(active);
  
  finally : coda;
}
