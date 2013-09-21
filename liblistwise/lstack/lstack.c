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

#include "listwise/internal.h"

#include "xmem.h"
#include "xstring.h"
#include "macros.h"
#include "liblistwise_control.h"

#if SANITY
#include "sanity.h"
#endif

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
//  ls - lstack
//  x  - stack index
//  y  - string index
//  z  - character index
//
static int allocate(lstack * const restrict ls, int x, int y, int z)
{
	if(x >= 0)
	{
		if(ls->a <= x)
		{
			int ns = ls->a ?: listwise_allocation_seed;
			while(ns <= x)
				ns = ns * 2 + ns / 2;

			fatal(xrealloc, &ls->s, sizeof(*ls->s), ns, ls->a);
			ls->a = ns;
		}

		if(y >= 0)
		{
			if(ls->s[x].a <= y)
			{
				int ns = ls->s[x].a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;

				fatal(xrealloc	
					, &ls->s[x].s
					, sizeof(*ls->s[0].s)
					, ns
					, ls->s[x].a
				);

				// list of windows
				fatal(xrealloc
					, &ls->s[x].w
					, sizeof(*ls->s[0].w)
					, ns
					, ls->s[x].a
				);

				// list of temp space
				fatal(xrealloc
					, &ls->s[x].t
					, sizeof(*ls->s[0].t)
					, ns
					, ls->s[x].a
				);

				ls->s[x].a = ns;
			}

			if(z >= 0)
			{
				if(ls->s[x].s[y].a <= z)
				{
					int ns = ls->s[x].s[y].a ?: listwise_allocation_seed;
					while(ns <= z)
						ns = ns * 2 + ns / 2;

					fatal(xrealloc
						, &ls->s[x].s[y].s
						, sizeof(*ls->s[0].s[0].s)
						, ns
						, ls->s[x].s[y].a
					);

					ls->s[x].s[y].a = ns;
				}
			}
		}
	}

	finally : coda;
}

static int ensure(lstack * const restrict ls, int x, int y, int z)
{
	if(x >= 0)
	{
		// ensure stack has enough lists
		if(ls->a <= x)
		{
			int ns = ls->a ?: listwise_allocation_seed;
			while(ns <= x)
				ns = ns * 2 + ns / 2;

			fatal(xrealloc, &ls->s, sizeof(ls->s[0]), ns, ls->a);
			ls->a = ns;
		}

		if(ls->l <= x)
			ls->l = x + 1;

		if(y >= 0)
		{
			// ensure list has enough strings
			if(ls->s[x].a <= y)
			{
				int ns = ls->s[x].a ?: listwise_allocation_seed;
				while(ns <= y)
					ns = ns * 2 + ns / 2;
				
				// list of strings
				fatal(xrealloc, &ls->s[x].s, sizeof(ls->s[x].s[0]), ns, ls->s[x].a);

				// list of windows
				fatal(xrealloc, &ls->s[x].w, sizeof(ls->s[x].w[0]), ns, ls->s[x].a);

				// list of tmp space
				fatal(xrealloc, &ls->s[x].t, sizeof(ls->s[x].t[0]), ns, ls->s[x].a);

				ls->s[x].a = ns;
			}

			if(ls->s[x].l <= y)
				ls->s[x].l = y + 1;

			if(z > 0)
			{
				// ensure string has enough space
				if(ls->s[x].s[y].a <= z)
				{
					int ns = ls->s[x].s[y].a ?: listwise_allocation_seed;
					while(ns <= z)
						ns = ns * 2 + ns / 2;

					fatal(xrealloc, &ls->s[x].s[y].s, sizeof(ls->s[x].s[y].s[0]), ns, ls->s[x].s[y].a);
					ls->s[x].s[y].a = ns;
				}
			}
		}
	}

	finally : coda;
}

static int writestack_alt(lstack* const restrict ls, int x, int y, const void* const restrict s, int l, uint8_t type)
{
	if(type)
	{
		// ensure stack has enough lists, list has enough strings, string has enough bytes
		fatal(allocate, ls, x, y, sizeof(s) - 1);

		// copy the pointer, set the type
		memcpy(ls->s[x].s[y].s, (void*)&s, sizeof(s));
		ls->s[x].s[y].type = type;
		ls->s[x].s[y].l = 0;
	}
	else
	{
		fatal(allocate, ls, x, y, l);

		// write and cap the string
		memcpy(ls->s[x].s[y].s, s, l);
		ls->s[x].s[y].s[l] = 0;
		ls->s[x].s[y].l = l;
		ls->s[x].s[y].type = 0;
	}

	// dirty the temp space for this entry
	ls->s[x].t[y].y = 0;
	ls->s[x].w[y].y = 0;

	finally : coda;
}

static int writestack(lstack* const restrict ls, int x, int y, const void* const restrict s, int l, uint8_t type)
{
	if(type)
	{
		// ensure stack has enough lists, list has enough strings, string has enough bytes
		fatal(ensure, ls, x, y, sizeof(s));

		// copy the pointer, set the type
		memcpy(ls->s[x].s[y].s, (void*)&s, sizeof(s));
		ls->s[x].s[y].type = type;
		ls->s[x].s[y].l = 0;
	}
	else if(l)
	{
		fatal(ensure, ls, x, y, l);

		// write and cap the string
		memcpy(ls->s[x].s[y].s, s, l);
		ls->s[x].s[y].s[l] = 0;
		ls->s[x].s[y].l = l;
		ls->s[x].s[y].type = 0;
	}

	// dirty the temp space for this entry
	ls->s[x].t[y].y = 0;
	ls->s[x].w[y].y = 0;

	finally : coda;
}

static int vwritestack(lstack* const restrict ls, int x, int y, const char* const restrict fmt, va_list va)
{
	va_list va2;
	va_copy(va2, va);

	int l = vsnprintf(0, 0, fmt, va);
	va_end(va);

	fatal(ensure, ls, x, y, l);

	vsprintf(ls->s[x].s[y].s, fmt, va2);
	va_end(va2);

	ls->s[x].s[y].l = l;
	ls->s[x].s[y].type = 0;

	// dirty the temp space for this entry
	ls->s[x].t[y].y = 0;
	ls->s[x].w[y].y = 0;

	finally : coda;
}

static int __attribute__((nonnull(1, 5, 6))) exec_internal(
	  const generator * const restrict g
	, const char ** const restrict init
	, const int * const restrict initls
	, const int initl
	, const lwx ** const restrict lx
	, const lstack ** const restrict ls
	, int dump
)
{
	// ovec workspace
	int* ovec = 0;
	int ovec_len = 0;

#if SANITY
	// sanity
	sanityblock * sb = 0;
#endif

	// empty operation for implicit y operator execution
	operator* ysop = op_lookup("ys", 1);
	operator* ywop = op_lookup("yw", 1);
	operator* oop = op_lookup("o", 1);

	struct operation ysoper = { .op = ysop };
	struct operation ywoper = { .op = ywop };

	// list stack allocation
	if(!*ls)
		fatal(xmalloc, ls, sizeof(*ls[0]));

	int curl = 0;

	// write init elements to top of list stack
	int x;
	for(x = 0; x < initl; x++)
	{
		fatal(writestack, *ls, 0, x + curl, init[x], initls[x], 0);
	}

	// write initial generator args at top of list stack
	for(x = 0; x < g->argsl; x++)
	{
		fatal(writestack, *ls, 0, x + curl + initl, g->args[x]->s, g->args[x]->l, 0);
	}

	// the initial state of the selection is all
	fatal(lstack_sel_all, *ls);

	if(dump)
		lstack_dump(*ls);

#if SANITY
	if(listwise_sanity)
	{
		fatal(sanityblock_create, &sb);
		fatal(sanity, *ls, sb);
	}
#endif

	// execute all operations
	int isor = 0;
	for(x = 0; x < g->opsl; x++)
	{
		isor = 0;
		if(g->ops[x]->op == oop)
		{
			isor = 1;
			if(++x == g->opsl)
				break;
		}

		if(!isor)
		{
			// if the previous operator staged windows, activate or unstage them
			if(x && (g->ops[x-1]->op->optype & LWOP_WINDOWS_STAGE))
			{
				if(g->ops[x]->op != yop)
				{
					if(g->ops[x-1]->op->optype & LWOP_WINDOWS_ACTIVATE)
					{
						if((*ls)->l || ywoper.op->optype & LWOP_EMPTYSTACK_YES)
							fatal(ywoper.op->op_exec, &ywoper, *ls, &ovec, &ovec_len);
					}
					else
					{
						fatal(lstack_windows_unstage, *ls);
					}
				}
			}

			// if the previous operator staged selections, activate or unstage them
			if(x && (g->ops[x-1]->op->optype & LWOP_SELECTION_STAGE))
			{
				if(g->ops[x]->op != yop)
				{
					if(g->ops[x-1]->op->optype & LWOP_SELECTION_ACTIVATE)
					{
						if((*ls)->l || ysoper.op->optype & LWOP_EMPTYSTACK_YES)
							fatal(ysoper.op->op_exec, &ysoper, *ls, &ovec, &ovec_len);
					}
					else
					{
						fatal(lstack_sel_unstage, *ls);
					}
				}
			}
		}

		if(dump)
		{
			if(x)
				lstack_dump(*ls);

			dprintf(listwise_err_fd, "\n");

			char buf[128];
			operation_write(buf, sizeof(buf), g->ops[x]);
			dprintf(listwise_err_fd, " >> %s\n", buf);
		}

		// execute the operator
		if(((*ls)->l || g->ops[x]->op->optype & LWOP_EMPTYSTACK_YES) && g->ops[x]->op->op_exec)
			fatal(g->ops[x]->op->op_exec, g->ops[x], *ls, &ovec, &ovec_len);

#if SANITY
		if(listwise_sanity)
			fatal(sanity, *ls, sb);
#endif

		if(g->ops[x]->op->optype & LWOP_WINDOW_RESET)
			fatal(lstack_windows_reset, *ls);

		if(g->ops[x]->op->optype & LWOP_SELECTION_RESET)
			fatal(lstack_sel_all, *ls);
	}

	fatal(lstack_windows_reset, *ls);		// reset windows
	fatal(lstack_sel_unstage, *ls);			// unstage selections (the selection itself remains)

	// clear string props set with the fx operator
	for(x = 0; x < object_registry.l; x++)
		xfree(&object_registry.e[x]->string_property);

	if(dump)
		lstack_dump(*ls);

finally:
	free(ovec);
#if SANITY
	sanityblock_free(sb);
#endif
coda;
}

/// deepcopy
//
// all - entire allocations are copied (the default is only up to the in-use size)
//
static int deepcopy(lstack ** const restrict A, lstack * const restrict B, int all)
	__attribute__((nonnull));

int deepcopy(lstack ** const A, lstack * const B, int all)
{
	int x;
	int y;

	fatal(xmalloc, A, sizeof(**A));

	if(all)
	{
		// copy entire allocations
		fatal(xmalloc, &(*A)->s, B->a * sizeof(*(*A)->s));
		(*A)->l = B->l;
		(*A)->a = B->a;

		for(x = 0; x < B->a; x++)
		{
			fatal(xmalloc, &(*A)->s[x].s, B->s[x].a * sizeof(*(*A)->s[0].s));
			fatal(xmalloc, &(*A)->s[x].w, B->s[x].a * sizeof(*(*A)->s[0].s));
			fatal(xmalloc, &(*A)->s[x].t, B->s[x].a * sizeof(*(*A)->s[0].s));
			(*A)->s[x].l = B->s[x].l;
			(*A)->s[x].a = B->s[x].a;

			for(y = 0; y < B->s[x].a; y++)
			{
				fatal(xmalloc, &(*A)->s[x].s[y].s, B->s[x].s[y].a);
				memcpy((*A)->s[x].s[y].s, B->s[x].s[y].s, B->s[x].s[y].a);
				(*A)->s[x].s[y].l = B->s[x].s[y].l;
				(*A)->s[x].s[y].a = B->s[x].s[y].a;
				(*A)->s[x].s[y].type = B->s[x].s[y].type;

				fatal(xmalloc, &(*A)->s[x].w[y].s, B->s[x].w[y].a);
				memcpy((*A)->s[x].w[y].s, B->s[x].w[y].s, sizeof(*(*A)->s[0].w[0].s) * B->s[x].w[y].a);
				(*A)->s[x].w[y].l = B->s[x].w[y].l;
				(*A)->s[x].w[y].a = B->s[x].w[y].a;
				(*A)->s[x].w[y].y = B->s[x].w[y].y;

				fatal(xmalloc, &(*A)->s[x].t[y].s, B->s[x].t[y].a);
				memcpy((*A)->s[x].t[y].s, B->s[x].t[y].s, sizeof(*(*A)->s[0].t[0].s) * B->s[x].t[y].a);
				(*A)->s[x].t[y].l = B->s[x].t[y].l;
				(*A)->s[x].t[y].a = B->s[x].t[y].a;
				(*A)->s[x].t[y].y = B->s[x].t[y].y;
			}
		}
	}
	else
	{
		fatal(xmalloc, &(*A)->s, B->l * sizeof(*(*A)->s));
		(*A)->l = B->l;
		(*A)->a = B->l;

		for(x = 0; x < B->l; x++)
		{
			fatal(xmalloc, &(*A)->s[x].s, B->s[x].l * sizeof(*(*A)->s[0].s));
			fatal(xmalloc, &(*A)->s[x].w, B->s[x].l * sizeof(*(*A)->s[0].s));
			fatal(xmalloc, &(*A)->s[x].t, B->s[x].l * sizeof(*(*A)->s[0].s));
			(*A)->s[x].l = B->s[x].l;
			(*A)->s[x].a = B->s[x].l;

			for(y = 0; y < B->s[x].l; y++)
			{
				fatal(xmalloc, &(*A)->s[x].s[y].s, B->s[x].s[y].l + 1);
				memcpy((*A)->s[x].s[y].s, B->s[x].s[y].s, B->s[x].s[y].l);
				(*A)->s[x].s[y].l = B->s[x].s[y].l;
				(*A)->s[x].s[y].a = B->s[x].s[y].l + 1;
				(*A)->s[x].s[y].type = B->s[x].s[y].type;

				fatal(xmalloc, &(*A)->s[x].w[y].s, B->s[x].w[y].l);
				memcpy((*A)->s[x].w[y].s, B->s[x].w[y].s, B->s[x].w[y].l);
				(*A)->s[x].w[y].l = B->s[x].w[y].l;
				(*A)->s[x].w[y].a = B->s[x].w[y].l;
				(*A)->s[x].w[y].y = B->s[x].w[y].y;

				fatal(xmalloc, &(*A)->s[x].t[y].s, B->s[x].t[y].l + 1);
				memcpy((*A)->s[x].t[y].s, B->s[x].t[y].s, B->s[x].t[y].l);
				(*A)->s[x].t[y].l = B->s[x].t[y].l;
				(*A)->s[x].t[y].a = B->s[x].t[y].l + 1;
				(*A)->s[x].t[y].y = B->s[x].t[y].y;
			}
		}
	}

	finally : coda;
}

//
// API
//

int API lstack_exec_internal(
	  const generator * const restrict g
	, const char ** const restrict init
	, const int * const restrict initls
	, const int initl
	, const lwx ** const restrict lx
	, const lstack ** const restrict ls
	, int dump
)
{
	return exec_internal(g, init, initls, initl, lx, ls, dump);
}

int API lstack_exec(
	  const generator * const restrict g
	, const char ** const restrict init
	, const int * const restrict initls
	, const int initl
	, const lwx ** const restrict lx
	, const lstack ** const restrict ls
)
{
	return exec_internal(g, init, initls, initl, lx, ls, 0);
}

int API lstack_create(lstack ** const restrict ls)
{
	return xmalloc(ls, sizeof(**ls));
}

void API lstack_free(lstack* ls)
{
	if(ls && ls != listwise_identity)
	{
		int x;
		int y;
		for(x = 0; x < ls->a; x++)
		{
			for(y = 0; y < ls->s[x].a; y++)
			{
				free(ls->s[x].s[y].s);
				free(ls->s[x].w[y].s);
				free(ls->s[x].t[y].s);
			}

			free(ls->s[x].s);
			free(ls->s[x].w);
			free(ls->s[x].t);
		}

		free(ls->s);
		free(ls->sel.s);
		free(ls->last.s);

		free(ls);
	}
}

void API lstack_xfree(lstack** ls)
{
	lstack_free(*ls);
	*ls = 0;
}

void API lstack_reset(lstack* ls)
{
	int x;
	int y;

	if(ls)
	{
		for(x = 0; x < ls->l; x++)
		{
			for(y = 0; y < ls->s[x].a; y++)
			{
				ls->s[x].s[y].l = 0;
				ls->s[x].s[y].type = 0;
				ls->s[x].w[y].y = 0;
				ls->s[x].t[y].y = 0;
			}

			ls->s[x].l = 0;
		}

		ls->l = 0;
		ls->sel.all = 1;
	}
}

int lstack_deepcopy(lstack ** const A, lstack * const B)
{
	return deepcopy(A, B, 0);
}

void API lstack_dump(lstack* ls)
{
	int x;
	int y;
	for(x = ls->l - 1; x >= 0; x--)
	{
		if(x != ls->l - 1)
			dprintf(listwise_err_fd, "\n");

		if(ls->s[x].l == 0)
		{
			dprintf(listwise_err_fd, "[%4d     ] -- empty \n", x);
		}

		for(y = 0; y < ls->s[x].l; y++)
		{
			int sel = 0;
			int last = 0;
			if(x == 0)
			{
				sel = 1;
				if(!ls->sel.all)
				{
					if(ls->sel.sl <= (y/8))
						sel = 0;
					else
					{
						if((ls->sel.s[y/8] & (0x01 << (y%8))) == 0)
							sel = 0;
					}
				}

				if(ls->last.sl > (y/8))
				{
					if(ls->last.s[y/8] & (0x01 << (y%8)))
						last = 1;
				}
			}

			dprintf(listwise_err_fd, "[%4d,%4d] %s%s "
				, x
				, y
				, sel ? ">" : " "
				, last ? ">" : " "
			);

			if(ls->s[x].s[y].type)
			{
				char * s;
				int l;
				lstack_getbytes(ls, x, y, &s, &l);

				dprintf(listwise_err_fd, "[%hhu]%p/%p (%.*s)", ls->s[x].s[y].type, *(void**)ls->s[x].s[y].s, ls->s[x].s[y].s, l, s);
			}
			else
			{
				dprintf(listwise_err_fd, "'%.*s'", ls->s[x].s[y].l, ls->s[x].s[y].s);
			}
			dprintf(listwise_err_fd, "\n");
		}
	}
}

void API lstack_clear(const lstack * const restrict ls, int x, int y)
{
	ls->s[x].s[y].l = 0;
	ls->s[x].s[y].type = 0;
	ls->s[x].w[y].y = 0;
	ls->s[x].t[y].y = 0;
}

int API lstack_append(lstack * const restrict ls, int x, int y, const char* const restrict s, int l)
{
	// ensure stack has enough lists, stack has enough strings, string has enough bytes
	fatal(ensure, ls, x, y, -1);
	fatal(ensure, ls, x, y, ls->s[x].s[y].l + l + 1);

	// append and cap the string
	memcpy(ls->s[x].s[y].s + ls->s[x].s[y].l, s, l);
	ls->s[x].s[y].s[ls->s[x].s[y].l + l] = 0;
	ls->s[x].s[y].l += l;
	ls->s[x].s[y].type = 0;

	ls->s[x].w[y].y = 0;
	ls->s[x].t[y].y = 0;

	finally : coda;
}

int API lstack_appendf(lstack* const restrict ls, int x, int y, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	va_list va2;
	va_copy(va2, va);

	int l = vsnprintf(0, 0, fmt, va);
	va_end(va);

	// ensure stack has enough lists, stack has enough strings, string has enough bytes
	fatal(ensure, ls, x, y, -1);
	fatal(ensure, ls, x, y, ls->s[x].s[y].l + l + 1);

	// append and cap the string
	vsprintf(ls->s[x].s[y].s + ls->s[x].s[y].l, fmt, va2);
	ls->s[x].s[y].s[ls->s[x].s[y].l + l] = 0;
	ls->s[x].s[y].l += l;
	ls->s[x].s[y].type = 0;

	ls->s[x].w[y].y = 0;
	ls->s[x].t[y].y = 0;

	finally : coda;
}

int API lstack_write_alt(lstack* const restrict ls, int x, int y, const char* const restrict s, int l)
{
	return writestack_alt(ls, x, y, s, l, 0);
}

int API lstack_write(lstack* const restrict ls, int x, int y, const char* const restrict s, int l)
{
	return writestack(ls, x, y, s, l, 0);
}

int API lstack_writef(lstack* const restrict ls, int x, int y, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	return vwritestack(ls, x, y, fmt, va);
}

int API lstack_obj_write_alt(lstack* const restrict ls, int x, int y, const void* const restrict o, uint8_t type)
{
	return writestack_alt(ls, x, y, o, 0, type);
}

int API lstack_obj_write(lstack* const restrict ls, int x, int y, const void* const restrict o, uint8_t type)
{
	return writestack(ls, x, y, o, 0, type);
}

int API lstack_add(lstack* const restrict ls, const char* const restrict s, int l)
{
	return writestack(ls, 0, ls->l ? ls->s[0].l : 0, s, l, 0);
}

int API lstack_addf(lstack* const restrict ls, const char* const restrict fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	return vwritestack(ls, 0, ls->l ? ls->s[0].l : 0, fmt, va);
}

int API lstack_obj_add(lstack* const restrict ls, const void* const restrict o, uint8_t type)
{
	return writestack(ls, 0, ls->l ? ls->s[0].l : 0, o, 0, type);
}

// shift removes the first list from the stack
int API lstack_shift(lstack* const restrict ls)
{
	if(ls->l)
	{
		typeof(ls->s[0]) T = ls->s[0];

		memmove(
			  &ls->s[0]
			, &ls->s[1]
			, (ls->l - 1) * sizeof(ls->s[0])
		);

		ls->s[--ls->l] = T;
	}

	return 0;
}

// pop removes the last list from the stack
int API lstack_pop(lstack* const restrict ls)
{
	if(ls->l)
		ls->l--;

	return 0;
}

// unshift allocates a new list at index 0
int API lstack_unshift(lstack* const restrict ls)
{
	// allocate new list, if necessary
	if(ls->l == ls->a)
	{
		fatal(xrealloc, &ls->s, sizeof(ls->s[0]), ls->l + 1, ls->a);
		ls->a++;
	}

	ls->l++;

	// swap an unused list into position zero
	typeof(ls->s[0]) T = ls->s[ls->l - 1];

	// copy list pointers down
	memmove(
			&ls->s[1]
		, &ls->s[0]
		, (ls->l - 1) * sizeof(ls->s[0])
	);

	ls->s[0] = T;

	// reset it
	ls->s[0].l = 0;

	finally : coda;
}

// push allocates a new list at the end
int API lstack_push(lstack* const restrict ls)
{
	// allocate new list, if necessary
	if(ls->l == ls->a)
	{
		fatal(xrealloc, &ls->s, sizeof(ls->s[0]), ls->l + 1, ls->a);
		ls->a++;
	}

	// reset
	ls->s[ls->l++].l = 0;

	finally : coda;
}

int API lstack_cycle(lstack* const restrict ls)
{
	return 1;
}

int API lstack_recycle(lstack* const restrict ls)
{
	return 1;
}

int API lstack_xchg(lstack* const restrict ls)
{
	return 1;
}

int API lstack_merge(lstack* const restrict ls, int to, int from)
{
	fatal(ensure, ls, to, -1, -1);
	int tox = ls->s[to].l;
	fatal(ensure, ls, to, ls->s[to].l + ls->s[from].l - 1, -1);

	memcpy(
		  ls->s[to].s + tox
		, ls->s[from].s
		, ls->s[from].l * sizeof(ls->s[0].s[0])
	);

	ls->s[from].a = 0;
	ls->s[from].l = 0;

	while(from < ls->l && ls->s[from].l == 0)
		from++;
	from--;

	while(from >= 0 && ls->s[from].l == 0)
		from--;
	from++;

	ls->l = from;

	finally : coda;
}

int API lstack_allocate(lstack * const restrict ls, int x, int y, int z)
{
	return allocate(ls, x, y, z);
}

int API lstack_ensure(lstack * const restrict ls, int x, int y, int z)
{
	return ensure(ls, x, y, z);
}

int API lstack_move(lstack * const restrict ls, int ax, int ay, int bx, int by)
{
	// copy of ax:ay, which is about to be overwritten
	typeof(ls->s[0].s[0]) Ts = ls->s[ax].s[ay];
	typeof(ls->s[0].w[0]) Tw = ls->s[ax].w[ay];
	typeof(ls->s[0].t[0]) Tt = ls->s[ax].t[ay];

	// copy
	ls->s[ax].s[ay] = ls->s[bx].s[by];
	ls->s[ax].w[ay] = ls->s[bx].w[by];
	ls->s[ax].t[ay] = ls->s[bx].t[by];

	// overwrite bx:by, creating a duplicate entry
	memmove(&ls->s[bx].s[by], &ls->s[bx].s[by+1], (ls->s[bx].l - by - 1) * sizeof(ls->s[0].s[0]));
	memmove(&ls->s[bx].w[by], &ls->s[bx].w[by+1], (ls->s[bx].l - by - 1) * sizeof(ls->s[0].w[0]));
	memmove(&ls->s[bx].t[by], &ls->s[bx].t[by+1], (ls->s[bx].l - by - 1) * sizeof(ls->s[0].t[0]));

	// overwrite the duplicate entry with the original dest
	ls->s[bx].s[ls->s[bx].l - 1] = Ts;
	ls->s[bx].w[ls->s[bx].l - 1] = Tw;
	ls->s[bx].t[ls->s[bx].l - 1] = Tt;

	// adjust the length
	ls->s[bx].l--;

	return 0;
}

int API lstack_displace(lstack * const restrict ls, int x, int y, int l)
{
	// notice that this is a no-op when l == 0
	// ensure there are sufficient unused entries beyond s[x].l
	fatal(lstack_allocate, ls, x, ls->s[x].l + l - 1, -1);

	// copy l allocated entries from beyond s[x].l
	typeof(ls->s[0].s[0]) * Ts = alloca(l * sizeof(ls->s[0].s[0]));
	typeof(ls->s[0].w[0]) * Tw = alloca(l * sizeof(ls->s[0].w[0]));
	typeof(ls->s[0].t[0]) * Tt = alloca(l * sizeof(ls->s[0].t[0]));
	memcpy(Ts, &ls->s[x].s[ls->s[x].l], l * sizeof(ls->s[0].s[0]));
	memcpy(Tw, &ls->s[x].w[ls->s[x].l], l * sizeof(ls->s[0].w[0]));
	memcpy(Tt, &ls->s[x].t[ls->s[x].l], l * sizeof(ls->s[0].t[0]));

	// copy entries at y down, creating hole of size l
	// this also overwrites the entries we just copied
	memmove(&ls->s[x].s[y + l], &ls->s[x].s[y], (ls->s[x].l - y) * sizeof(ls->s[0].s[0]));
	memmove(&ls->s[x].w[y + l], &ls->s[x].w[y], (ls->s[x].l - y) * sizeof(ls->s[0].w[0]));
	memmove(&ls->s[x].t[y + l], &ls->s[x].t[y], (ls->s[x].l - y) * sizeof(ls->s[0].t[0]));

	// copy the l unused entries into the hole
	memcpy(&ls->s[x].s[y], Ts, l * sizeof(ls->s[0].s[0]));
	memcpy(&ls->s[x].w[y], Tw, l * sizeof(ls->s[0].w[0]));
	memcpy(&ls->s[x].t[y], Tt, l * sizeof(ls->s[0].t[0]));

	ls->s[x].l += l;

	finally : coda;
}

int API lstack_delete(lstack * const restrict ls, int x, int y)
{
	// copy of this entry
	typeof(ls->s[0].s[0]) Ts = ls->s[x].s[y];
	typeof(ls->s[0].w[0]) Tw = ls->s[x].w[y];
	typeof(ls->s[0].t[0]) Tt = ls->s[x].t[y];

	// overwrite this entry
	memmove(
			&ls->s[x].s[y]
		, &ls->s[x].s[y+1]
		, (ls->s[x].l - y - 1) * sizeof(ls->s[0].s[0])
	);

	memmove(
			&ls->s[x].w[y]
		, &ls->s[x].w[y+1]
		, (ls->s[x].l - y - 1) * sizeof(ls->s[0].w[0])
	);

	memmove(
			&ls->s[x].t[y]
		, &ls->s[x].t[y+1]
		, (ls->s[x].l - y - 1) * sizeof(ls->s[0].t[0])
	);

	// overwrite the duplicated entry with the current entry
	ls->s[x].s[ls->s[x].l - 1] = Ts;
	ls->s[x].w[ls->s[x].l - 1] = Tw;
	ls->s[x].t[ls->s[x].l - 1] = Tt;

	ls->s[x].l--;

	return 0;
}

int API lstack_readrow(lstack* const ls, lwx * const lx, int x, int y, char ** const r, int * const rl, int obj, int win, int str, int * _raw)
{
	char * zs = ls->s[x].s[y].s;
	int zsl   = ls->s[x].s[y].l;
	int raw = 1;

	if(obj && ls->s[x].s[y].type)
	{
		listwise_object* o = idx_lookup_val(object_registry.by_type, &ls->s[x].s[y].type, 0);

		if(o == 0)
			return 1;

		o->string(*(void**)ls->s[x].s[y].s, o->string_property, &zs, &zsl);
		raw = 0;
	}

	// if there is a window in effect for this entry
	if(win && x == 0 && lx->win.s[y].active)
	{
		if(ls->s[x].t[y].y != 2)
		{
			if(ls->s[x].t[y].a <= ls->s[x].w[y].zl)
			{
				if(xrealloc(
						&ls->s[x].t[y].s
					, sizeof(ls->s[x].t[y].s[0])
					, ls->s[x].w[y].zl + 1
					, ls->s[x].t[y].a) != 0)
				{
					return 1;
				}

				ls->s[x].t[y].a = ls->s[x].w[y].zl + 1;
			}

			size_t z = 0;
			int i;
			for(i = 0; i < ls->s[x].w[y].l; i++)
			{
				memcpy(ls->s[x].t[y].s + z, zs + ls->s[x].w[y].s[i].o, ls->s[x].w[y].s[i].l);
				z += ls->s[x].w[y].s[i].l;
			}
			ls->s[x].t[y].s[ls->s[x].w[y].zl] = 0;
			ls->s[x].t[y].l = ls->s[x].w[y].zl;
			ls->s[x].t[y].y = 2;
		}

		zs = ls->s[x].t[y].s;
		zsl = ls->s[x].t[y].l;
		raw = 0;
	}
	else if(str)
	{
		if(ls->s[x].t[y].y != 1)
		{
			if(ls->s[x].t[y].a <= zsl)
			{
				if(xrealloc(
						&ls->s[x].t[y].s
					, sizeof(ls->s[x].t[y].s[0])
					, zsl + 1
					, ls->s[x].t[y].a) != 0)
				{
					return 1;
				}

				ls->s[x].t[y].a = zsl + 1;
			}

			memcpy(ls->s[x].t[y].s, zs, zsl);
			ls->s[x].t[y].s[zsl] = 0;
			ls->s[x].t[y].l = zsl;
			ls->s[x].t[y].y = 1;
		}
		
		zs = ls->s[x].t[y].s;
		zsl = ls->s[x].t[y].l;
		raw = 0;
	}

	*r  = zs;
	*rl = zsl;
	if(_raw)
		*_raw = raw;

	return 0;
}

int API lstack_getbytes(lstack* const restrict ls, int x, int y, char ** r, int * rl)
{
	return lstack_readrow(ls, x, y, r, rl, 1, 1, 0, 0);
}

int API lstack_getstring(lstack* const restrict ls, int x, int y, char ** r, int * rl)
{
	return lstack_readrow(ls, x, y, r, rl, 1, 1, 1, 0);
}

typedef char* charstar;
charstar API lstack_string(lstack* const restrict ls, int x, int y)
{
	char * r;
	int    rl;
	if(lstack_readrow(ls, x, y, &r, &rl, 1, 1, 1, 0) != 0)
		return 0;

	return r;
}
