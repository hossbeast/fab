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

static int generator_arg_canon(arg * const arg, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
	// track backreferences
	int k = 0;

	// emit characters according to the scanmode
	int i;
	for(i = 0; i < arg->l; i++)
	{
		if(arg->refs.l > k && arg->refs.v[k].s == &arg->s[i])
		{
			SAY("\\%d", arg->refs.v[k].ref);

			i += arg->refs.v[k].l - 1;
			k++;
		}
		else if(arg->s[i] == ' ' && GS_DELIMITED(sm))
		{
			SAY("\\s");
		}
		else if(arg->s[i] == '\t' && GS_DELIMITED(sm))
		{
			SAY("\\t");
		}
		else if(arg->s[i] == '\r' && GS_DELIMITED(sm))
		{
			SAY("\\r");
		}
		else if(arg->s[i] == '\n' && GS_DELIMITED(sm))
		{
			SAY("\\n");
		}
		else if((arg->s[i] != ' ' || arg->s[i] != '\t' || arg->s[i] != '\n' || arg->s[i] != '\r') && (arg->s[i] <= 0x20 || arg->s[i] >= 0x7f))
		{
			SAY("\\x{%02hhx}", arg->s[i]);
		}
		else if(arg->s[i] == '\\' && GS_DOREFS(sm))
		{
			SAY("\\%c", arg->s[i]);
		}
		else if(GS_DELIMITED(sm) && arg->s[i] == genscan_opening_char[sm])
		{
			SAY("\\%c", arg->s[i]);
		}
		else if(GS_ENCLOSED(sm) && arg->s[i] == genscan_closing_char[sm])
		{
			SAY("\\%c", arg->s[i]);
		}
		else
		{
			SAY("%c", arg->s[i]);
		}
	}

	finally : coda;
}

static int generator_args_canon(arg ** const args, const int argsl, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the opener
		if(GS_ENCLOSED(sm))
			SAY("%c", genscan_opening_char[sm]);
		else if(x)
			SAY("%c", genscan_opening_char[sm]);

		// emit the argument string
		fatal(generator_arg_canon, args[x], sm, dst + (*z), sz - (*z), z, ps, writer);

		// emit the closer
		if(GS_ENCLOSED(sm))
			SAY("%c", genscan_closing_char[sm]);
		else if(x == (argsl - 1) && args[x]->l == 0)
			SAY("%c", genscan_opening_char[sm]);
	}

	finally : coda;
#if 0
	size_t z = 0;

	int x;
	for(x = 0; x < argsl; x++)
	{
		// emit the opener
		if(GS_ENCLOSED(sm))
			z += znprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);
		else if(x)
			z += znprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);

		// emit the argument string
		z += generator_arg_canon_write(dst + z, sz - z, args[x], sm);

		// emit the closer
		if(GS_ENCLOSED(sm))
			z += znprintf(dst + z, sz - z, "%c", genscan_closing_char[sm]);
		else if(x == (argsl - 1) && args[x]->l == 0)
			z += znprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);
	}

	return z;
#endif
}

static int generator_operation_canon(operation * const oper, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
	SAY("%s", oper->op->s);

	// conditionally emit the delimiter
	if(oper->argsl)
		SAY("%c", genscan_opening_char[sm]);

	fatal(generator_args_canon, oper->args, oper->argsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	finally : coda;
#if 0
static size_t generator_operation_canon_write(char * const dst, const size_t sz, operation * const oper, uint32_t sm)
{
	size_t z = 0;
	z += znprintf(dst + z, sz - z, "%s", oper->op->s);

	// emit the delimiter
	if(oper->argsl)
		z += znprintf(dst + z, sz - z, "%c", genscan_opening_char[sm]);

	return z + generator_args_canon_write(dst + z, sz - z, oper->args, oper->argsl, sm);
}
#endif
}

static int generator_operations_canon(operation ** const ops, int opsl, uint32_t sm, char * const dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			SAY(" ");

		fatal(generator_operation_canon, ops[x], sm, dst + (*z), sz - (*z), z, ps, writer);
	}

	finally : coda;
#if 0
	size_t z = 0;
	
	int x;
	for(x = 0; x < opsl; x++)
	{
		if(x)
			z += znprintf(dst + z, sz - z, " ");

		z += generator_operation_canon_write(dst + z, sz - z, ops[x], sm);
	}

	return z;
#endif
}

static int generator_canon(generator * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, fwriter writer)
{
	uint32_t sm = GENSCAN_SLASH_DOREFS;

	fatal(generator_args_canon, g->args, g->argsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	if(z)
		SAY(" ");

	fatal(generator_operations_canon, g->ops, g->opsl, sm, dst + (*z), sz - (*z), z, ps, writer);

	finally : coda;
#if 0
static size_t generator_canon_write(generator * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
{
	uint32_t sm = GENSCAN_SLASH_DOREFS;

	size_t z = 0;
	z += generator_args_canon_write(dst + z, sz - z, g->args, g->argsl, sm);

	if(z)
		z += znprintf(dst + z, sz - z, " ");

	return z + generator_operations_canon_write(dst + z, sz - z, g->ops, g->opsl, sm);
}
#endif
}

static int generator_description(generator * const restrict g, char * const restrict dst, const size_t sz, size_t * const z, pstring ** restrict ps, fwriter writer)
{
	// expanded generator description
	SAY("generator @ %p {\n", g);
	SAY("  initial list\n");

	int x;
	for(x = 0; x < g->argsl; x++)
	{
		SAY("    ");
		fatal(generator_arg_canon, g->args[x], 0, dst + (*z), sz - (*z), z, ps, writer);
		SAY("\n");
	}

	SAY("  operations\n");
	for(x = 0; x < g->opsl; x++)
	{
		SAY("    OP - %s\n", g->ops[x]->op->s);
		SAY("      args\n");

		int y;
		for(y = 0; y < g->ops[x]->argsl; y++)
		{
			SAY("        ");
			fatal(generator_arg_canon, g->ops[x]->args[y], 0, dst + (*z), sz - (*z), z, ps, writer);
			SAY("\n");
		}

		if(y == 0)
			SAY("        none\n");
	}

	if(x == 0)
		SAY("    none");

	// canonicalized generator-string
	SAY("\n --> ");
	fatal(generator_canon, g, dst + (*z), sz - (*z), z, ps, writer);
	SAY("\n}");

	finally : coda;
}

static int lstack_description(lwx * const lx, char * const dst, const size_t sz, size_t * const z, pstring ** restrict ps, fwriter writer)
{
	SAY("sel active { lease(%3d) %s era(%3d) %s nil=%d } staged { lease(%3d) %s era(%3d) }\n"
		, lx->sel.active ? lx->sel.active->lease : 0
		, (lx->sel.active ? lx->sel.active->lease : 0) == lx->sel.active_era ? "= " : "!="
		, lx->sel.active_era
		, (lx->sel.active ? lx->sel.active->nil : 0) ? "but" : "and"
		, lx->sel.active ? lx->sel.active->nil : 0
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
					if(lx->sel.active->nil == 0 && lx->sel.active->sl > (y / 8))
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

				if(state != LWX_WINDOWED_ALL)
				{
					SAY("\n");
					size_t oz = *z;
					SAY("%16s", " ");

					int escaping = 0;
					int w = -1;
					int i;
					if(state != LWX_WINDOWED_NONE)
					{
						for(i = 0; i < sl; i++)
						{
							if(w == -1 && lx->win.s[y].active->s[0].o == i)
							{
								w = 0;
							}

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
									SAY(" ");
								}

								if(w >= 0 && w < lx->win.s[y].active->l && i >= lx->win.s[y].active->s[w].o)
									w++;
							}
						}
					}

					if((*z - oz) < 45)
						SAY("%*s", 45 - (*z - oz), "");

					SAY(" active { lease(%3d) %s era(%3d) %s nil=%d }"
						, lx->win.s[y].active->lease
						, lx->win.s[y].active->lease == lx->win.active_era ? "= " : "!="
						, lx->win.active_era
						, (lx->win.s[y].active->lease == lx->win.active_era ? lx->win.s[y].active->nil : 0) ? "but" : "and"
						, lx->win.s[y].active->nil
					);
				}
			}

			// indicate staged windows
			if(x == 0 && lx->win.s[y].staged && lx->win.s[y].staged->lease == lx->win.staged_era && !lx->win.s[y].staged->nil)
			{
				SAY("\n");
				size_t oz = *z;
				SAY("%16s", " ");

				int escaping = 0;
				int w = -1;
				int i;
				for(i = 0; i < sl; i++)
				{
					if(w == -1 && lx->win.s[y].staged->s[0].o == i)
					{
						w = 0;
					}

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
							SAY(" ");
						}

						if(w >= 0 && w < lx->win.s[y].staged->l && i >= lx->win.s[y].staged->s[w].o)
							w++;
					}
				}

				if((*z - oz) < 45)
					SAY("%*s", 45 - (*z - oz), "");

				SAY(" staged { lease(%3d) %s era(%3d) }"
					, lx->win.s[y].staged->lease
					, lx->win.s[y].staged->lease == lx->win.staged_era ? "= " : "!="
					, lx->win.staged_era
				);
			}
		}
	}

	finally : coda;
}

///
/// public
///

int operation_canon_pswrite(operation * const oper, uint32_t sm, pstring ** restrict ps)
{
	fatal(psclear, ps);
	xproxy(generator_operation_canon, oper, sm, 0, 0, (size_t[]) { }, ps, pswrite);
}

/// writes to a fixed size buffer
int zwrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
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
int pswrite(char * const restrict dst, const size_t sz, size_t * restrict z, pstring ** restrict ps, const char * const restrict fmt, ...)
{
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

int API generator_canon_write(generator * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
{
	size_t lz = 0;
	if(!z)
		z = &lz;

	xproxy(generator_canon, g, dst, sz, z, 0, zwrite);
}

int API generator_canon_pswrite(generator * const restrict g, pstring ** restrict ps)
{
	size_t lz = 0;
	fatal(psclear, ps);
	xproxy(generator_canon, g, 0, 0, &lz, ps, pswrite);
}

int API generator_canon_dump(generator * const restrict g, pstring ** restrict ps)
{
	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	size_t lz = 0;
	fatal(generator_canon, g, 0, 0, &lz, ps, pswrite);
	printf("%.*s\n", (int)(*ps)->l, (*ps)->s);

finally:
	psfree(lps);
coda;
}

int API generator_canon_log(generator * const restrict g, pstring ** restrict ps, void * restrict udata)
{
	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	if(lw_would_generator())
	{
		size_t lz = 0;
		fatal(generator_canon, g, 0, 0, &lz, ps, pswrite);
		lw_log_generator("%.*s", (int)(*ps)->l, (*ps)->s);
	}

finally:
	psfree(lps);
coda;
}

int API generator_description_write(generator * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
{
	size_t lz = 0;
	if(!z)
		z = &lz;

	xproxy(generator_description, g, dst, sz, z, 0, zwrite);
}

int API generator_description_pswrite(generator * const restrict g, pstring ** restrict ps)
{
	size_t lz = 0;
	fatal(psclear, ps);
	xproxy(generator_description, g, 0, 0, &lz, ps, pswrite);
}

int API generator_description_dump(generator * const restrict g, pstring ** restrict ps)
{
	pstring * lps = 0;
	if(!ps)
		ps = &lps;
	fatal(psclear, ps);

	size_t lz = 0;
	fatal(generator_description, g, 0, 0, &lz, ps, pswrite);
	printf("%.*s\n", (int)(*ps)->l, (*ps)->s);

finally:
	psfree(lps);
coda;
}

int API generator_description_log(generator * const restrict g, pstring ** restrict ps, void * restrict udata)
{
	pstring * lps = 0;

	if(lw_would_generator())
	{
		if(!ps)
			ps = &lps;
		fatal(psclear, ps);

		size_t lz = 0;
		fatal(generator_description, g, 0, 0, &lz, ps, pswrite);
		lw_log_generator("%.*s", (int)(*ps)->l, (*ps)->s);
	}

finally:
	psfree(lps);
coda;
}

xapi API lstack_description_write(lwx * const restrict lx, char * const restrict dst, const size_t sz, size_t * restrict z)
{
	size_t lz = 0;
	if(!z)
		z = &lz;

	xproxy(lstack_description, lx, dst, sz, z, 0, zwrite);
}

xapi API lstack_description_pswrite(lwx * const restrict lx, pstring ** restrict ps)
{
	size_t z = 0;
	fatal(psclear, ps);
	xproxy(lstack_description, lx, 0, 0, &z, ps, pswrite);
}

xapi API lstack_description_dump(lwx * const restrict lx, pstring ** restrict ps)
{
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

xapi API lstack_description_log(lwx * const restrict lx, pstring ** restrict ps, void * restrict udata)
{
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
