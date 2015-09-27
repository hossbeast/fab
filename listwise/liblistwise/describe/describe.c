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

#include <stdarg.h>
#include <inttypes.h>

#include "xlinux.h"

#include "internal.h"

#include "macros.h"

#define restrict __restrict

static uint32_t __attribute__((unused)) arg_scanmode(arg * const arg)
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

static xapi lstack_description(lwx * const lx, char * const dst, const size_t sz, size_t * const z, pstring ** restrict ps, fwriter writer)
{
  enter;

	SAY("sel active { lease(%3d) %s era(%3d) %s nil=%d } staged { lease(%3d) %s era(%3d) }\n"
		, lx->sel.active ? lx->sel.active->lease : 0
		, (lx->sel.active ? lx->sel.active->lease : 0) == lx->sel.active_era ? "= " : "!="
		, lx->sel.active_era
		, (lx->sel.active ? lx->sel.active->state == LWX_SELECTION_NONE : 0) ? "but" : "and"
		, lx->sel.active ? lx->sel.active->state == LWX_SELECTION_NONE : 0
		, lx->sel.staged ? lx->sel.staged->lease : 0
		, (lx->sel.staged ? lx->sel.staged->lease : 0) == lx->sel.staged_era ? "= " : "!="
		, lx->sel.staged_era
	);
	SAY("win active { %*sera(%3d)%*s } staged { %*sera(%3d) }\n"
		, 14, ""
		, lx->win.active_era
		, 10, ""
		, 14, ""
		, lx->win.staged_era
	);

	int x;
	int y;
	for(x = lx->l - 1; x >= 0; x--)
	{
		if(x != lx->l - 1)
			SAY("\n");

		if(lx->s[x].l == 0)
		{
			SAY("[%4d     ] -- empty", x);
		}

		for(y = 0; y < lx->s[x].l; y++)
		{
			int __attribute__((unused)) select = 0;
			int __attribute__((unused)) staged = 0;
			if(x == 0)
			{
				select = 1;
				if(lx->sel.active && lx->sel.active->lease == lx->sel.active_era)
				{
					select = 0;
					if(lx->sel.active->state != LWX_SELECTION_NONE && lx->sel.active->sl > (y / 8))
					{
						select = lx->sel.active->s[y / 8] & (0x01 << (y % 8));
					}
				}

				if(lx->sel.staged && lx->sel.staged->lease == lx->sel.staged_era)
				{
					if(lx->sel.staged->sl > (y / 8))
					{
						staged = lx->sel.staged->s[y / 8] & (0x01 << (y % 8));
					}
				}
			}
			
			if(x != lx->l - 1 || y)
				SAY("\n");

			// for each entry : indexes, whether selected, staged
			SAY("[%4d,%4d] %s%s "
				, x
				, y
				, select ? ">" : " "
				, staged ? "+" : " "
			);

			// display the string value of the row
			char * s = 0;
			int sl = 0;
			fatal(lstack_readrow, lx, x, y, &s, &sl, 0, 1, 0, 0, 0);

			SAY("'%.*s'", sl, s);

			// also display object properties if applicable
			if(lx->s[x].s[y].type)
			{
				SAY("[%hhu]%p/%p", lx->s[x].s[y].type, *(void**)lx->s[x].s[y].s, lx->s[x].s[y].s);
			}

			// indicate active windows other than ALL, which is the default state
			if(x == 0)
			{
				lwx_windows * win;
				int state = lstack_windows_state(lx, y, &win);

				if(state != LWX_WINDOWS_ALL)
				{
					SAY("\n");
					size_t oz = *z;
					SAY("%16s", " ");

					if(state != LWX_WINDOWS_NONE)
					{
						int escaping = 0;
						int w = -1;
						int i;

						for(i = 0; i < sl; i++)
						{
							if(w == -1 && lx->win.s[y].active->s[0].o == i)
							{
								w = 0;
							}

							if(escaping)
							{
								if(s[i] == 0x6d)
									escaping = 0;
							}
							else if(s[i] == 0x1b)
							{
								escaping = 1;
							}
							else
							{
								int marked = 0;
								if(w >= 0 && w < lx->win.s[y].active->l && i >= lx->win.s[y].active->s[w].o)
								{
									if((i - lx->win.s[y].active->s[w].o) < lx->win.s[y].active->s[w].l)
									{
										SAY("^");
										marked = 1;
									}
								}

								if(!marked)
								{
									// between internal windows
									SAY(" ");
									if(w >= 0 && w < lx->win.s[y].active->l && i >= lx->win.s[y].active->s[w].o)
										w++;
								}
							}
						}
					}

					if((*z - oz) < 45)
						SAY("%*s", 45 - (*z - oz), "");

					SAY(" active { lease(%3d) %s era(%3d)"
						, lx->win.s[y].active->lease
						, lx->win.s[y].active->lease == lx->win.active_era ? "= " : "!="
						, lx->win.active_era
					);

					if(lx->win.s[y].active->lease == lx->win.active_era)
					{
						SAY(" %s state=%s"
							, lx->win.s[y].active->state == LWX_WINDOWS_SOME ? "and" : "but"
							, LWX_WINDOWS_STR(lx->win.s[y].active->state)
						);
					}
					SAY(" }");
				}
			}

			// indicate staged windows
			if(x == 0)// && lx->win.s[y].staged && lx->win.s[y].staged->lease == lx->win.staged_era && !lx->win.s[y].staged->state == LWX_WINDOWS_NONE)
			{
				if(lx->win.s[y].staged)
				{
					SAY("\n");
					size_t oz = *z;
					SAY("%16s", " ");

					lwx_windows * win;
					int state = lstack_windows_staged_state(lx, y, &win);

					if(state != LWX_WINDOWS_NONE)
					{
						int escaping = 0;
						int w = -1;
						int i;
						for(i = 0; i < sl; i++)
						{
							if(state == LWX_WINDOWS_SOME && w == -1 && lx->win.s[y].staged->s[0].o == i)
							{
								w = 0;
							}

							if(escaping)
							{
								if(s[i] == 0x6d)
									escaping = 0;
							}
							else if(s[i] == 0x1b)
							{
								escaping = 1;
							}
							else if(state == LWX_WINDOWS_SOME)
							{
								int marked = 0;
								if(w >= 0 && w < lx->win.s[y].staged->l && i >= lx->win.s[y].staged->s[w].o)
								{
									if((i - lx->win.s[y].staged->s[w].o) < lx->win.s[y].staged->s[w].l)
									{
										SAY("+");
										marked = 1;
									}
								}

								if(!marked)
								{
									// between internal windows
									SAY(" ");
									if(w >= 0 && w < lx->win.s[y].staged->l && i >= lx->win.s[y].staged->s[w].o)
										w++;
								}
							}
						}
					}

					if((*z - oz) < 45)
						SAY("%*s", 45 - (*z - oz), "");

					SAY(" staged { lease(%3d) %s era(%3d)"
						, lx->win.s[y].staged->lease
						, lx->win.s[y].staged->lease == lx->win.staged_era ? "= " : "!="
						, lx->win.staged_era
					);

					if(lx->win.s[y].staged->lease == lx->win.staged_era)
					{
						SAY(" %s state=%s"
							, lx->win.s[y].staged->state == LWX_WINDOWS_SOME ? "and" : "but"
							, LWX_WINDOWS_STR(lx->win.s[y].staged->state)
						);
					}
					SAY(" }");
				}
			}
		}
	}

	finally : coda;
}

///
/// public
///

xapi operation_canon_pswrite(operation * const oper, uint32_t sm, pstring ** restrict ps)
{
  enter;

	fatal(psclear, ps);
	fatal(transform_operation_canon, oper, sm, 0, 0, (size_t[]) { }, ps, pswrite);

  finally : coda;
}

/// writes to a fixed size buffer
xapi zwrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
  enter;

	if(sz - (*z))
	{
		va_list va;
		va_start(va, fmt);
		(*z) += MIN(sz, vsnprintf(dst + (*z), sz - (*z), fmt, va));
		va_end(va);
	}

	finally : coda;
}

/// writes to a dynamically-resizing pstring
xapi pswrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
  enter;

	size_t ol = 0;
	if(*ps)
		ol = (*ps)->l;

	va_list va;
	va_start(va, fmt);
	fatal(psvcatf, ps, fmt, va);
	va_end(va);

	(*z) += (*ps)->l - ol;

	finally : coda;
}

///
/// API
///

API xapi lstack_description_write(lwx * const restrict lx, char * const restrict dst, const size_t sz, size_t * restrict z)
{
  enter;

	size_t lz = 0;
	if(!z)
		z = &lz;

	fatal(lstack_description, lx, dst, sz, z, 0, zwrite);

  finally : coda;
}

API xapi lstack_description_pswrite(lwx * const restrict lx, pstring ** restrict ps)
{
  enter;

	size_t z = 0;
	fatal(psclear, ps);
	fatal(lstack_description, lx, 0, 0, &z, ps, pswrite);

  finally : coda;
}

API xapi lstack_description_dump(lwx * const restrict lx, pstring ** restrict ps)
{
  enter;

	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	size_t lz = 0;
	fatal(lstack_description, lx, 0, 0, &lz, ps, pswrite);
	printf("%.*s\n", (int)(*ps)->l, (*ps)->s);

finally:
	psfree(lps);
coda;
}

API xapi lstack_description_log(lwx * const restrict lx, pstring ** restrict ps, void * restrict udata)
{
  enter;

	pstring * lps = 0;

	if(lw_would_lstack())
	{
		if(!ps)
			ps = &lps;
		fatal(psclear, ps);

		size_t lz = 0;
		fatal(lstack_description, lx, 0, 0, &lz, ps, pswrite);
		lw_log_lstack("%.*s", (int)(*ps)->l, (*ps)->s);
	}

finally:
	psfree(lps);
coda;
}
