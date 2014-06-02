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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "internal.h"

#include "xlinux.h"
#include "macros.h"

#define restrict __restrict

///
/// static
///

/// allocate
//
// SUMMARY
//  make an assertion on allocated dimensions of an lstack
//
// PARAMETERS
//  lx - lstack
//  x  - stack index
//  y  - string index
//  z  - character index
//
static int allocate(lwx * const restrict lx, int x, int y, int z)
{
	if(x >= 0)
	{
		if(lx->a <= x)
		{
			int ns = lx->a ?: listwise_allocation_seed;
			while(ns <= x)
				ns = ns * 2 + ns / 2;

			fatal(xrealloc, &lx->s, sizeof(*lx->s), ns, lx->a);
			lx->a = ns;
		}

		if(y >= 0)
		{
			if(lx->s[x].a <= y)
			{
				int ns = lx->s[x].a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;

				// list of rows
				fatal(xrealloc, &lx->s[x].s, sizeof(*lx->s[0].s), ns, lx->s[x].a);

				// list of temp space
				fatal(xrealloc, &lx->s[x].t, sizeof(*lx->s[0].t), ns, lx->s[x].a);

				lx->s[x].a = ns;
			}

			// ensure window list
			if(x == 0 && lx->win.a <= y)
			{
				int ns = lx->win.a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;

				void * old = lx->win.s;
				fatal(xrealloc, &lx->win.s, sizeof(*lx->win.s), ns, lx->win.a);
				lx->win.a = ns;

				if(old != lx->win.s)
				{
					// if this reallocation moves lx->win.s, any lx->win.s[-].{active,staged} that was
					// nonzero will then be invalid
					int k;
					for(k = 0; k < lx->win.a; k++)
					{
						if(lx->win.s[k].active)
							lx->win.s[k].active = &lx->win.s[k].storage[lx->win.s[k].active_storage_index];

						if(lx->win.s[k].staged)
							lx->win.s[k].staged = &lx->win.s[k].storage[lx->win.s[k].staged_storage_index];
					}
				}
			}

			if(z >= 0)
			{
				if(lx->s[x].s[y].a <= z)
				{
					int ns = lx->s[x].s[y].a ?: listwise_allocation_seed;
					while(ns <= z)
						ns = ns * 2 + ns / 2;

					fatal(xrealloc
						, &lx->s[x].s[y].s
						, sizeof(*lx->s[0].s[0].s)
						, ns
						, lx->s[x].s[y].a
					);

					lx->s[x].s[y].a = ns;
				}
			}
		}
	}

	finally : coda;
}

static int ensure(lwx * const restrict lx, int x, int y, int z)
{
	if(x >= 0)
	{
		// ensure stack has enough lists
		if(lx->a <= x)
		{
			int ns = lx->a ?: listwise_allocation_seed;
			while(ns <= x)
				ns = ns * 2 + ns / 2;

			fatal(xrealloc, &lx->s, sizeof(lx->s[0]), ns, lx->a);
			lx->a = ns;
		}

		if(lx->l <= x)
			lx->l = x + 1;

		if(y >= 0)
		{
			// ensure list has enough strings
			if(lx->s[x].a <= y)
			{
				int ns = lx->s[x].a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;
				
				// list of strings
				fatal(xrealloc, &lx->s[x].s, sizeof(lx->s[x].s[0]), ns, lx->s[x].a);

				// list of tmp space
				fatal(xrealloc, &lx->s[x].t, sizeof(lx->s[x].t[0]), ns, lx->s[x].a);

				lx->s[x].a = ns;
			}

			// ensure window list
			if(x == 0 && lx->win.a <= y)
			{
				int ns = lx->win.a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;

				void * old = lx->win.s;
				fatal(xrealloc, &lx->win.s, sizeof(*lx->win.s), ns, lx->win.a);
				lx->win.a = ns;

				if(old != lx->win.s)
				{
					// if this reallocation moves lx->win.s, any lx->win.s[-].{active,staged} that was
					// nonzero will then be invalid
					int k;
					for(k = 0; k < lx->win.a; k++)
					{
						if(lx->win.s[k].active)
							lx->win.s[k].active = &lx->win.s[k].storage[lx->win.s[k].active_storage_index];

						if(lx->win.s[k].staged)
							lx->win.s[k].staged = &lx->win.s[k].storage[lx->win.s[k].staged_storage_index];
					}
				}
			}

			if(lx->s[x].l <= y)
				lx->s[x].l = y + 1;

			if(z >= 0)
			{
				// ensure string has enough space
				if(lx->s[x].s[y].a <= z)
				{
					int ns = lx->s[x].s[y].a ?: listwise_allocation_seed;
					while(ns <= z)
						ns = ns * 2 + ns / 2;

					fatal(xrealloc, &lx->s[x].s[y].s, sizeof(lx->s[x].s[y].s[0]), ns, lx->s[x].s[y].a);
					lx->s[x].s[y].a = ns;
				}
			}
		}
	}

	finally : coda;
}

static int writestack_alt(lwx * const restrict lx, int x, int y, const void* const restrict s, int l, uint8_t type)
{
	if(type)
	{
		// ensure stack has enough lists, list has enough strings, string has enough bytes
		fatal(allocate, lx, x, y, sizeof(s) - 1);

		// copy the pointer, set the type
		memcpy(lx->s[x].s[y].s, (void*)&s, sizeof(s));
		lx->s[x].s[y].type = type;
		lx->s[x].s[y].l = 0;
	}
	else
	{
		fatal(allocate, lx, x, y, l);

		// write and cap the string
		memcpy(lx->s[x].s[y].s, s, l);
		lx->s[x].s[y].s[l] = 0;
		lx->s[x].s[y].l = l;
		lx->s[x].s[y].type = 0;
	}

	// dirty the temp space for this row
	lx->s[x].t[y].y = LWTMP_UNSET;

	// reset the window for this row
	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

static int vwritestack_alt(lwx * const restrict lx, int x, int y, const void* const restrict fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int l = vsnprintf(0, 0, fmt, va);
	va_end(va);

	fatal(allocate, lx, x, y, l);

	vsprintf(lx->s[x].s[y].s, fmt, va2);
	va_end(va2);

	lx->s[x].s[y].l = l;
	lx->s[x].s[y].type = 0;

	// dirty the temp space for this row
	lx->s[x].t[y].y = LWTMP_UNSET;

	// reset the window for this row
	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

static int writestack(lwx * const restrict lx, int x, int y, const void* const restrict s, int l, uint8_t type)
{
	if(type)
	{
		// ensure stack has enough lists, list has enough strings, string has enough bytes
		fatal(ensure, lx, x, y, sizeof(s));

		// copy the pointer, set the type
		memcpy(lx->s[x].s[y].s, (void*)&s, sizeof(s));
		lx->s[x].s[y].type = type;
		lx->s[x].s[y].l = 0;
	}
	else
	{
		fatal(ensure, lx, x, y, l);

		// write and cap the string
		memcpy(lx->s[x].s[y].s, s, l);
		lx->s[x].s[y].s[l] = 0;
		lx->s[x].s[y].l = l;
		lx->s[x].s[y].type = 0;
	}

	// dirty the temp space for this entry
	lx->s[x].t[y].y = LWTMP_UNSET;

	// reset the window for this row
	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

static int vwritestack(lwx * const restrict lx, int x, int y, const char* const restrict fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int l = vsnprintf(0, 0, fmt, va);
	va_end(va);

	fatal(ensure, lx, x, y, l);

	vsprintf(lx->s[x].s[y].s, fmt, va2);
	va_end(va2);

	lx->s[x].s[y].l = l;
	lx->s[x].s[y].type = 0;

	// dirty the temp space for this entry
	lx->s[x].t[y].y = LWTMP_UNSET;

	// reset the window for this row
	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

//
// API
//

int API lstack_clear(lwx * const restrict lx, int x, int y)
{
	lx->s[x].s[y].l = 0;
	lx->s[x].s[y].type = 0;

	lx->s[x].t[y].y = LWTMP_UNSET;

	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

int API lstack_catw(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
{
	// ensure stack has enough lists, stack has enough strings, string has enough bytes
	fatal(ensure, lx, x, y, -1);
	fatal(ensure, lx, x, y, lx->s[x].s[y].l + l);

	// append and cap the string
	memcpy(lx->s[x].s[y].s + lx->s[x].s[y].l, s, l);
	lx->s[x].s[y].s[lx->s[x].s[y].l + l] = 0;
	lx->s[x].s[y].l += l;
	lx->s[x].s[y].type = 0;

	lx->s[x].t[y].y = LWTMP_UNSET;

	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

int API lstack_cats(lwx * const restrict lx, int x, int y, const char* const restrict s)
{
	xproxy(lstack_catw, lx, x, y, s, strlen(s));
}

int API lstack_catf(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	va_list va2;
	va_copy(va2, va);

	int l = vsnprintf(0, 0, fmt, va);
	va_end(va);

	// ensure stack has enough lists, stack has enough strings, string has enough bytes
	fatal(ensure, lx, x, y, -1);
	fatal(ensure, lx, x, y, lx->s[x].s[y].l + l + 1);

	// append and cap the string
	vsprintf(lx->s[x].s[y].s + lx->s[x].s[y].l, fmt, va2);
	lx->s[x].s[y].s[lx->s[x].s[y].l + l] = 0;
	lx->s[x].s[y].l += l;
	lx->s[x].s[y].type = 0;

	lx->s[x].t[y].y = LWTMP_UNSET;

	if(x == 0)
	{
		fatal(lstack_window_deactivate, lx, y);
	}

	finally : coda;
}

int API lstack_writes(lwx * const restrict lx, int x, int y, const char* const restrict s)
{
	xproxy(writestack, lx, x, y, s, strlen(s), 0);
}

int API lstack_writew(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
{
	xproxy(writestack, lx, x, y, s, l, 0);
}

int API lstack_writef(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	xproxy(vwritestack, lx, x, y, fmt, va);
}

int API lstack_adds(lwx * const restrict lx, const char* const restrict s)
{
	xproxy(writestack, lx, 0, lx->l ? lx->s[0].l : 0, s, strlen(s), 0);
}

int API lstack_addw(lwx * const restrict lx, const char* const restrict s, size_t l)
{
	xproxy(writestack, lx, 0, lx->l ? lx->s[0].l : 0, s, l, 0);
}

int API lstack_addf(lwx * const restrict lx, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	xproxy(vwritestack, lx, 0, lx->l ? lx->s[0].l : 0, fmt, va);
}

int API lstack_alt_writes(lwx * const restrict lx, int x, int y, const char* const restrict s)
{
	xproxy(writestack_alt, lx, x, y, s, strlen(s), 0);
}

int API lstack_alt_writew(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
{
	xproxy(writestack_alt, lx, x, y, s, l, 0);
}

int API lstack_alt_writef(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	xproxy(vwritestack_alt, lx, x, y, fmt, va);
}

int API lstack_obj_write(lwx * const restrict lx, int x, int y, const void* const restrict o, uint8_t type)
{
	xproxy(writestack, lx, x, y, o, 0, type);
}

int API lstack_obj_add(lwx * const restrict lx, const void* const restrict o, uint8_t type)
{
	xproxy(writestack, lx, 0, lx->l ? lx->s[0].l : 0, o, 0, type);
}

int API lstack_obj_alt_write(lwx * const restrict lx, int x, int y, const void* const restrict o, uint8_t type)
{
	xproxy(writestack_alt, lx, x, y, o, 0, type);
}

// shift removes the first list from the stack
int API lstack_shift(lwx * const restrict lx)
{
	if(lx->l)
	{
		typeof(lx->s[0]) T = lx->s[0];

		memmove(
			  &lx->s[0]
			, &lx->s[1]
			, (lx->l - 1) * sizeof(lx->s[0])
		);

		lx->s[--lx->l] = T;
	}

	finally : coda;
}

// pop removes the last list from the stack
int API lstack_pop(lwx * const restrict lx)
{
	if(lx->l)
		lx->l--;

	finally : coda;
}

// unshift allocates a new list at index 0
int API lstack_unshift(lwx * const restrict lx)
{
	// allocate new list, if necessary
	if(lx->l == lx->a)
	{
		fatal(xrealloc, &lx->s, sizeof(lx->s[0]), lx->l + 1, lx->a);
		lx->a++;
	}

	lx->l++;

	// swap an unused list into position zero
	typeof(lx->s[0]) T = lx->s[lx->l - 1];

	// copy list pointers down
	memmove(
			&lx->s[1]
		, &lx->s[0]
		, (lx->l - 1) * sizeof(lx->s[0])
	);

	lx->s[0] = T;

	// reset it
	lx->s[0].l = 0;

	finally : coda;
}

// push allocates a new list at the end
int API lstack_push(lwx * const restrict lx)
{
	// allocate new list, if necessary
	if(lx->l == lx->a)
	{
		fatal(xrealloc, &lx->s, sizeof(lx->s[0]), lx->l + 1, lx->a);
		lx->a++;
	}

	// reset
	lx->s[lx->l++].l = 0;

	finally : coda;
}

int API lstack_cycle(lwx * const restrict lx)
{
	return 1;
}

int API lstack_recycle(lwx * const restrict lx)
{
	return 1;
}

int API lstack_xchg(lwx * const restrict lx)
{
	return 1;
}

int API lstack_merge(lwx * const restrict lx, int a, int b)
{
	if(a < 0 || a >= lx->l)
		fail(LW_NOLIST);

	if(b < 0 || b >= lx->l)
		fail(LW_NOLIST);

	if(a == b)
		fail(LW_ILLOP);

	// number of entries to move
	int al = lx->s[a].l;
	int n = lx->s[b].l;

	// ensure allocated space in dest list
	fatal(allocate, lx, a, al + n - 1, -1);

	// copy unused rows from dst list to temp space
	typeof(lx->s[0].s[0]) * Ts = alloca(n * sizeof(*Ts));
	typeof(lx->s[0].t[0]) * Tt = alloca(n * sizeof(*Tt));
	memcpy(Ts, &lx->s[a].s[al], n * sizeof(*Ts));
	memcpy(Tt, &lx->s[a].t[al], n * sizeof(*Tt));

	// copy rows from src to dst, overwriting the rows just copied to temp
	memcpy(&lx->s[a].s[al], &lx->s[b].s[0], n * sizeof(lx->s[0].s[0]));
	memcpy(&lx->s[a].t[al], &lx->s[b].t[0], n * sizeof(lx->s[0].t[0]));

	// copy rows from temp to src list
	memcpy(&lx->s[b].s[0], Ts, n * sizeof(lx->s[0].s[0]));
	memcpy(&lx->s[b].t[0], Tt, n * sizeof(lx->s[0].t[0]));

	// shorten src list
	lx->s[b].l = 0;

	// extend dst list
	lx->s[a].l += n;

	// move remaining lists down and place the src list at the end of the stack
	typeof(lx->s[0]) T = lx->s[b];
	memmove(&lx->s[b], &lx->s[b + 1], (lx->l - b - 1) * sizeof(lx->s[0]));
	lx->s[lx->l - 1] = T;

	// shorten stack
	lx->l--;

finally:
	XAPI_INFOF("to", "%d", a);
	XAPI_INFOF("from", "%d", b);
coda;
}

int API lstack_allocate(lwx * const restrict lx, int x, int y, int z)
{
	xproxy(allocate, lx, x, y, z);
}

int API lstack_ensure(lwx * const restrict lx, int x, int y, int z)
{
	xproxy(ensure, lx, x, y, z);
}

int API lstack_move(lwx * const restrict lx, int ax, int ay, int bx, int by)
{
	// copy of ax:ay, which is about to be overwritten
	typeof(lx->s[0].s[0]) Ts = lx->s[ax].s[ay];
	typeof(lx->s[0].t[0]) Tt = lx->s[ax].t[ay];

	// copy
	lx->s[ax].s[ay] = lx->s[bx].s[by];
	lx->s[ax].t[ay] = lx->s[bx].t[by];

	// overwrite bx:by, creating a duplicate entry
	memmove(&lx->s[bx].s[by], &lx->s[bx].s[by+1], (lx->s[bx].l - by - 1) * sizeof(lx->s[0].s[0]));
	memmove(&lx->s[bx].t[by], &lx->s[bx].t[by+1], (lx->s[bx].l - by - 1) * sizeof(lx->s[0].t[0]));

	// overwrite the duplicate entry with the original dest
	lx->s[bx].s[lx->s[bx].l - 1] = Ts;
	lx->s[bx].t[lx->s[bx].l - 1] = Tt;

	// adjust the length
	lx->s[bx].l--;

#if 0
	if(ax == 0)
	{
		// if moving to an entry in the 0th list, unstage its window, if any
		lx->win.s[ay].staged = 0;
	}
#endif

	finally : coda;
}

int API lstack_displace(lwx * const restrict lx, int x, int y, int l)
{
	// notice that this is a no-op when l == 0
	// ensure there are sufficient unused entries beyond s[x].l
	fatal(allocate, lx, x, lx->s[x].l + l - 1, -1);

	// copy l allocated entries from beyond s[x].l
	typeof(lx->s[0].s[0]) * Ts = alloca(l * sizeof(lx->s[0].s[0]));
	typeof(lx->s[0].t[0]) * Tt = alloca(l * sizeof(lx->s[0].t[0]));
	memcpy(Ts, &lx->s[x].s[lx->s[x].l], l * sizeof(lx->s[0].s[0]));
	memcpy(Tt, &lx->s[x].t[lx->s[x].l], l * sizeof(lx->s[0].t[0]));

	// copy entries at y down, creating hole of size l
	// this also overwrites the entries we just copied
	memmove(&lx->s[x].s[y + l], &lx->s[x].s[y], (lx->s[x].l - y) * sizeof(lx->s[0].s[0]));
	memmove(&lx->s[x].t[y + l], &lx->s[x].t[y], (lx->s[x].l - y) * sizeof(lx->s[0].t[0]));

	// copy the l unused entries into the hole
	memcpy(&lx->s[x].s[y], Ts, l * sizeof(lx->s[0].s[0]));
	memcpy(&lx->s[x].t[y], Tt, l * sizeof(lx->s[0].t[0]));

	lx->s[x].l += l;

#if 0
	if(x == 0)
	{
		// if displacing entries in the 0th list, unstage their windows, if any
		int i;
		for(i = 0; i < l; i++)
			lx->win.s[y + i].staged = 0;
	}
#endif

	finally : coda;
}

int API lstack_delete(lwx * const restrict lx, int x, int y)
{
	// copy of this entry
	typeof(lx->s[0].s[0]) Ts = lx->s[x].s[y];
	typeof(lx->s[0].t[0]) Tt = lx->s[x].t[y];

	// overwrite this entry
	memmove(&lx->s[x].s[y], &lx->s[x].s[y+1], (lx->s[x].l - y - 1) * sizeof(lx->s[0].s[0]));
	memmove(&lx->s[x].t[y], &lx->s[x].t[y+1], (lx->s[x].l - y - 1) * sizeof(lx->s[0].t[0]));

	// overwrite the duplicated entry with the current entry
	lx->s[x].s[lx->s[x].l - 1] = Ts;
	lx->s[x].t[lx->s[x].l - 1] = Tt;

	lx->s[x].l--;

#if 0
	if(x == 0)
	{
		// if deleting an entry in the 0th list, 
	}
#endif

	finally : coda;
}

int API lstack_readrow(lwx * const lx, int x, int y, char ** const r, int * const rl, uint8_t * const rt, int obj, int win, int str, int * const _raw)
{
	char * zs		= lx->s[x].s[y].s;
	int zsl			= lx->s[x].s[y].l;
	uint8_t zst	= lx->s[x].s[y].type;
	int raw = 1;

	if(obj && lx->s[x].s[y].type)
	{
		listwise_object * o = 0;
		fatal(listwise_lookup_object, lx->s[x].s[y].type, &o);

		o->string(*(void**)lx->s[x].s[y].s, o->string_property, &zs, &zsl);
		raw = 0;
	}

	// if there is a window in effect for this entry
	if(win && x == 0 && lx->win.s[y].active && lx->win.s[y].active->lease == lx->win.active_era)
	{
		if(lx->s[x].t[y].y != LWTMP_WINDOW)
		{
			if(lx->s[x].t[y].a <= lx->win.s[y].active->zl)
			{
				fatal(xrealloc
					, &lx->s[x].t[y].s
					, sizeof(*lx->s[0].t[0].s)
					, lx->win.s[y].active->zl + 1
					, lx->s[x].t[y].a
				);

				lx->s[x].t[y].a = lx->win.s[y].active->zl + 1;
			}

			size_t z = 0;
			int i;
			for(i = 0; i < lx->win.s[y].active->l; i++)
			{
				memcpy(lx->s[x].t[y].s + z, zs + lx->win.s[y].active->s[i].o, lx->win.s[y].active->s[i].l);
				z += lx->win.s[y].active->s[i].l;
			}
			lx->s[x].t[y].s[lx->win.s[y].active->zl] = 0;
			lx->s[x].t[y].l = lx->win.s[y].active->zl;
			lx->s[x].t[y].y = LWTMP_WINDOW;
		}

		zs = lx->s[x].t[y].s;
		zsl = lx->s[x].t[y].l;
		raw = 0;
	}
	else if(str)
	{
		if(lx->s[x].t[y].y != LWTMP_STRING)
		{
			if(lx->s[x].t[y].a <= zsl)
			{
				fatal(xrealloc
					, &lx->s[x].t[y].s
					, sizeof(lx->s[x].t[y].s[0])
					, zsl + 1
					, lx->s[x].t[y].a
				);

				lx->s[x].t[y].a = zsl + 1;
			}

			memcpy(lx->s[x].t[y].s, zs, zsl);
			lx->s[x].t[y].s[zsl] = 0;
			lx->s[x].t[y].l = zsl;
			lx->s[x].t[y].y = LWTMP_STRING;
		}
		
		zs = lx->s[x].t[y].s;
		zsl = lx->s[x].t[y].l;
		raw = 0;
	}

	if(r)
		*r  = zs;
	if(rl)
		*rl = zsl;
	if(rt)
		*rt = zst;
	if(_raw)
		*_raw = raw;

	finally : coda;
}

int API lstack_getobject(lwx * const restrict lx, int x, int y, char ** const restrict r, uint8_t * const restrict rt)
{
	xproxy(lstack_readrow, lx, x, y, r, 0, rt, 0, 0, 0, 0);
}

int API lstack_getbytes(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
{
	xproxy(lstack_readrow, lx, x, y, r, rl, 0, 1, 1, 0, 0);
}

int API lstack_getstring(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
{
	xproxy(lstack_readrow, lx, x, y, r, rl, 0, 1, 1, 1, 0);
}

int API lstack_string(lwx * const restrict lx, int x, int y, char ** r)
{
	xproxy(lstack_readrow, lx, x, y, r, 0, 0, 1, 1, 1, 0);
}

int API lstack_swaptop(lwx * const restrict lx, int ay, int by)
{
	typeof(lx->s[0].s[0]) Ts = lx->s[0].s[ay];
	typeof(lx->s[0].t[0]) Tt = lx->s[0].t[ay];
	typeof(lx->win.s[0])  Tw = lx->win.s[ay];

	lx->s[0].s[ay] = lx->s[0].s[by];
	lx->s[0].t[ay] = lx->s[0].t[by];
	lx->win.s[ay]	= lx->win.s[by];
	lx->win.s[ay].active = &lx->win.s[ay].storage[lx->win.s[ay].active_storage_index];
	lx->win.s[ay].staged = &lx->win.s[ay].storage[lx->win.s[ay].staged_storage_index];

	lx->s[0].s[by] = Ts;
	lx->s[0].t[by] = Tt;
	lx->win.s[by]	= Tw;
	lx->win.s[by].active = &lx->win.s[by].storage[lx->win.s[by].active_storage_index];
	lx->win.s[by].staged = &lx->win.s[by].storage[lx->win.s[by].staged_storage_index];

	finally : coda;
}
