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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "sanity.h"
#include "listwise/internal.h"

#include "xlinux.h"
#include "macros.h"

#define restrict __restrict

#if SANITY
int sanityblock_create(sanityblock ** const sb)
{
	fatal(xmalloc, sb, sizeof(**sb));

	finally : coda;
}

void sanityblock_reset(sanityblock * const sb)
{
	int x;
	for(x = 0; x < sb->s_stringsl; x++)
		sb->s_strings[x].ol = 0;

	sb->s_stringsl = 0;
}

void sanityblock_free(sanityblock * const sb)
{
	if(sb)
	{
		int x;
		for(x = 0; x < sb->s_stringsa; x++)
		{
			free(sb->s_strings[x].s);
			free(sb->s_strings[x].o);
		}

		free(sb->s_strings);
	}
	free(sb);
}

int sanity(struct lwx_t * const restrict ls, sanityblock * const restrict sb)
{
	int R = 0;

#define err(fmt, ...) dprintf(listwise_sanity_fd, fmt "\n", ##__VA_ARGS__); R++

uint64_t totlists = 0;
uint64_t totstrings = 0;
uint64_t totcompares = 0;

	int ax;
	int ay;
	int bx;
	int by;
	for(ax = 0; ax < ls->a; ax++)
	{
		if(ls->s[ax].s)
		{
totlists++;
			for(ay = 0; ay < ls->s[ax].a; ay++)
			{
				if(ls->s[ax].s[ay].s)
				{
totstrings++;
					for(bx = 0; bx < ls->a; bx++)
					{
						if(ls->s[bx].s)
						{
							if(ax != bx)
							{
								if(ls->s[ax].s == ls->s[bx].s)
								{
									err("duplicate lists %p a=[%d] b=[%d]", ls->s[ax].s, ax, bx);
								}

								for(by = 0; by < ls->s[bx].a; by++)
								{
									if(ls->s[bx].s)
									{
										if(ay != by)
										{
totcompares++;
											if(ls->s[bx].s[by].s)
											{
												if(ls->s[ax].s[ay].s == ls->s[bx].s[by].s)
												{
													err("duplicate strings %p @ a=[%d:%d] b=[%d:%d]", ls->s[ax].s[ay].s, ax, ay, bx, by);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	int x;
	int y;

	// compare previous list of strings/objects to new list, detect loss
	for(x = 0; x < sb->s_stringsl; x++)
	{
		// check its old location, first
		typeof(sb->s_strings[0]) * sbs = &sb->s_strings[x];

		if(ls->s[sbs->x].s[sbs->y].s == sbs->s)
		{
			ax = sbs->x;
			ay = sbs->y;
		}
		else
		{
			for(ax = 0; ax < ls->a; ax++)
			{
				for(ay = 0; ay < ls->s[ax].a; ay++)
				{
					if(ls->s[ax].s[ay].s == sbs->s)
						break;
				}
				if(ay < ls->s[ax].a)
					break;
			}
		}
		if(ax >= ls->a)
		{
			err("LOSS %p", sbs->s);
			err(" -> %6s : %d", "x", sbs->x);
			err(" -> %6s : %d", "y", sbs->y);
			err(" -> %6s : %d", "l", sbs->l);
			err(" -> %6s : %d", "a", sbs->a);
			err(" -> %6s : %hhu", "t", sbs->t);

			if(sbs->t)
			{
				listwise_object* o = idx_lookup_val(object_registry.by_type, &sbs->t, 0);
				if(o)
				{
					char * s;
					int sl;
					o->string(*(void**)sbs->o, o->string_property, &s, &sl);

					if(s)
					{
						err(" -> %6s : [%hhu]%p (%.*s)", "value", sbs->t, *(void**)sbs->o, sl, s);
					}
					else
					{
						err(" -> %6s : [%hhu]%p NOVALUE", "value", sbs->t, *(void**)sbs->o);
					}
				}
				else
				{
					err(" -> %6s : [NOTYPE]", "value");
				}
			}
			else
			{
				err(" -> %6s : '%.*s'", "value", sbs->l, sbs->o);
			}
		}
	}

	sanityblock_reset(sb);

	// flat list of all strings/objects in the lstack
	for(x = 0; x < ls->a; x++)
	{
		for(y = 0; y < ls->s[x].a; y++)
		{
			if(ls->s[x].s[y].s)
			{
				if(sb->s_stringsl == sb->s_stringsa)
				{
					int ns = sb->s_stringsa ?: 10;
					ns = ns * 2 + ns / 2;
					fatal(xrealloc, &sb->s_strings, sizeof(*sb->s_strings), ns, sb->s_stringsa);
					sb->s_stringsa = ns;
				}

				typeof(sb->s_strings[0]) * sbs = &sb->s_strings[sb->s_stringsl++];

				sbs->s = ls->s[x].s[y].s;
				sbs->l = ls->s[x].s[y].l;
				sbs->a = ls->s[x].s[y].a;
				sbs->t = ls->s[x].s[y].type;
				sbs->x = x;
				sbs->y = y;

				if(sbs->oa <= ls->s[x].s[y].l)
				{
					int ns = sbs->oa ?: 10;
					while(ns <= ls->s[x].s[y].l)
						ns = ns * 2 + ns / 2;
					
					fatal(xrealloc, &sbs->o, sizeof(*sbs->o), ns, sbs->oa);
					sbs->oa = ns;
				}

				memcpy(sbs->o, ls->s[x].s[y].s, ls->s[x].s[y].l);
				sbs->ol = ls->s[x].s[y].l;
			}
		}
	}

	dprintf(listwise_sanity_fd, "SANITY : %3lu lists @ %6.2f apiece, %5d strings, compares : %lu\n", totlists, ((double)totstrings) / ((double)ls->a), sb->s_stringsl, totcompares);

	if(R)
		exit(1);

	finally : coda;
}
#endif
