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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "xapi/internal.h"

#include "macros.h"

// per-thread callstacks
__thread struct callstack callstack;

// refers to the top frame
#define TOP (callstack.v[callstack.l])

#define restrict __restrict

typeof(callstack) * T;

static int wrealloc(void* target, size_t es, size_t ec, size_t oec)
{
	void** t = ((void**)target);
	*t = realloc(*t, es * ec);

	if(es * ec)
	{
		if(*t)
		{
			if(((ssize_t)ec - (ssize_t)oec) > 0)
				memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);

			return 0;
		}
		return 1;
	}
	return 0;
}

static int nomem()
{
	// switch to alt stack
	callstack.isalt = 1;

	// add alt frames
	xapi_frame_push();
	xapi_frame_push();

	// populate the alternate frame with an ENOMEM error
	xapi_frame_set_and_leave(perrtab_SYS, SYS_ENOMEM, 0, 0, "realloc");

	return -1;
}

#undef restrict

///
/// callstack manipulation
///
int API xapi_frame_push()
{
T = &callstack;

	if(callstack.isalt || callstack.finalized)
	{
//printf("isalt enter, l %d, depth %d -> %d\n", callstack.alt.l, callstack.alt.depth, callstack.alt.depth + 2);

		callstack.isalt = 1;
		callstack.alt.depth++;
	}
	else
	{
//printf("isreg enter, l %d, depth %d -> %d\n", callstack.l, callstack.depth, callstack.depth + 1);
		callstack.depth++;

		/* ensure dynamic frame allocation */
		if(callstack.depth > callstack.frames.stor.a)
		{
			int ns = callstack.frames.stor.a ?: 10;
			ns = ns * 2 + ns / 2;

#if 0
ns = callstack.depth;
			if(wrealloc(&callstack.frames.stor.v, sizeof(*callstack.frames.stor.v), ns, callstack.frames.stor.a) == 0)
			{
				xapi_frame_leave();
				return nomem();
			}
#else
			if(wrealloc(&callstack.frames.stor.v, sizeof(*callstack.frames.stor.v), ns, callstack.frames.stor.a) != 0)
			{
				xapi_frame_leave();
				return nomem();
			}
#endif

			callstack.frames.stor.a = ns;
		}

		/* ensure frame list allocation for at least 2 frames */
		if(callstack.depth >= callstack.a)
		{
			int ns = callstack.a ?: 10;
			ns = ns * 2 + ns / 2;
			if(wrealloc(&callstack.v, sizeof(*callstack.v), ns, callstack.a) != 0)
			{
				xapi_frame_leave();
				return nomem();
			}

			callstack.a = ns;
		}
	}

	return 0;
}

void API xapi_frame_alt_push()
{
	callstack.isalt = 1;

	xapi_frame_push();
}

int API xapi_frame_depth()
{
	if(callstack.isalt || callstack.finalized)
		return callstack.alt.depth + 1;
	else
		return callstack.depth;
}

void API xapi_frame_leave()
{
	if(callstack.isalt)
	{
//printf("isalt leaving, l %d -> %d, depth %d -> %d%s\n", callstack.alt.l, callstack.alt.l + 1, callstack.alt.depth, callstack.alt.depth - 1, callstack.alt.depth == 1 ? " ISALT OFF" : "");
		if(callstack.alt.v[callstack.alt.l])
			callstack.alt.l++;

		if(--callstack.alt.depth == 0)
			callstack.isalt = 0;
	}
	else
	{
//printf("isreg leaving, depth %d -> %d%s\n", callstack.depth, callstack.depth - 1, callstack.depth == 0 ? " FREEING" : "");
		if(callstack.v && callstack.v[callstack.l])
			callstack.l++;

		/*
		** depth goes to -1 when a function exits that was not itself called with UNWIND-ing
		*/
		if(callstack.depth-- == 0)
		{
//printf("callstack free\n");
			callstack_free();
		}
	}
}

int API xapi_frame_exit()
{
	int16_t rc = 0;
	int16_t rt = 0;

	if(callstack.l && callstack.v[0]->code)
	{
		rc = callstack.v[0]->code;
		rt = (rc == 0 || rc == 0xFFFF) ? 0 : callstack.v[0]->etab->id;
	}
	else if(callstack.alt.l)
	{
		rc = callstack.frames.alt[0].code;
		rt = (rc == 0 || rc == 0xFFFF) ? 0 : callstack.frames.alt[0].etab->id;
	}

	if(callstack.isalt)
	{
		/*
		** this can happen when 
		*/
		dprintf(2, "EXIT WHILE ISALT DEPTH=%d\n", callstack.alt.depth);
		exit(0);
	}

	xapi_frame_leave();

	callstack.finalized = 0;

//	printf("exiting with %d\n", (rt << 16) | rc);
	return (rt << 16) | rc;
}

void API xapi_frame_finalize()
{
	callstack.finalized = 1;
}

int API xapi_frame_finalized()
{
	return callstack.finalized;
}

int API xapi_frame_unwinding()
{
T = &callstack;
	return callstack.l || callstack.alt.l;
}

void API xapi_frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
T = &callstack;

	struct frame * f = 0;

	if(callstack.isalt)
	{
//printf("setting alt[%d] %s at %s:%d\n", callstack.alt.l, func, file, line);
		if(callstack.alt.l < (sizeof(callstack.frames.alt) / sizeof(callstack.frames.alt[0])))
		{
			f = callstack.alt.v[callstack.alt.l] = &callstack.frames.alt[callstack.alt.l];
			f->type = 1;
		}
		else
		{
			/* lost */
		}
	}
	else
	{
#if 1
if(!callstack.v)
{
	/*
	** when and UNWIND-ing function is called without fatal, AND there is an active callstack, then
	** when that function returns, the callstack is freed. while processing a subsequent error this
	** condition will be true and the program will segfault
	*/
}
#endif
		if(callstack.depth == 0)
		{
			// use the base frame for the initial call
			if(!TOP)
			{
//printf("setting reg[%d] %s at %s:%d\n", callstack.l, func, file, line);
				f = TOP = &callstack.frames.base;
				f->type = 1;
			}
		}
		else
		{
			if(!TOP)
			{
//printf("setting reg[%d] %s at %s:%d\n", callstack.l, func, file, line);
				f = TOP = &callstack.frames.stor.v[callstack.l];
			}
		}
	}

	if(f)
	{
		f->etab = etab;
		f->code = code;
		f->file = file;
		f->line = line;
		f->func = func;

		f->msgl = 0;
		f->info.l = 0;
	}
}

void API xapi_frame_set_and_leave(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
	xapi_frame_set(etab, code, file, line, func);
	xapi_frame_leave();
}

int API xapi_frame_set_message(const char * const fmt, ...)
{
	if(fmt)
	{
		va_list va;
		va_start(va, fmt);

		if(callstack.isalt)
		{
			dprintf(2, "MESSAGE ON ALT STACK\n");
		}
		else
		{
			va_list va2;
			va_copy(va2, va);

			// measure
			int w = vsnprintf(0, 0, fmt, va2);
			va_end(va2);

			// reallocate
			if(TOP->msga <= w)
			{
				int ns = TOP->msga;
				while(ns <= w)
				{
					ns = ns ?: 10;
					ns = ns * 2 + ns / 2;
				}
				
	static int C;
				if(C++ == -1)
				{
					if(wrealloc(&TOP->msg, sizeof(*TOP->msg), ns, TOP->msga) == 0)
						return nomem();
				}
				else
				{
					if(wrealloc(&TOP->msg, sizeof(*TOP->msg), ns, TOP->msga) != 0)
						return nomem();
				}	

				TOP->msga = ns;
			}

			TOP->msgl = vsprintf(TOP->msg, fmt, va);
		}

		va_end(va);
	}

	return 0;
}

int API xapi_frame_add_info(const char * const k, int kl, const char * const vfmt, ...)
{
	struct frame * f = 0;

	if(callstack.v)
		f = callstack.v[callstack.l];
	else if(callstack.alt.l)
		f = callstack.alt.v[callstack.alt.l - 1];	

	if(f)
	{
		kl = kl ?: strlen(k);

		if(f->type)
		{
			struct frame_static * fs = (struct frame_static*)f;
			
			if(fs->info.l < (sizeof(fs->buf_info) / sizeof(fs->buf_info[0])))
			{
				fs->info.v = fs->buf_info;
				fs->info.v[fs->info.l].ks = fs->buf_info_ks[fs->info.l];
				fs->info.v[fs->info.l].vs = fs->buf_info_vs[fs->info.l];
				
				// key
				memcpy(fs->info.v[fs->info.l].ks, k, MIN(sizeof(fs->buf_info_ks[0]) - 1, kl));
				fs->info.v[fs->info.l].ks[kl] = 0;
				fs->info.v[fs->info.l].kl = kl;

				// value
				va_list va;
				va_start(va, vfmt);

				fs->info.v[fs->info.l].vl = vsnprintf(fs->info.v[fs->info.l].vs, sizeof(fs->buf_info_vs[0]), vfmt, va);

				fs->info.l++;
			}
		}
		else
		{
			// reallocate the info list
			if(f->info.l == f->info.a)
			{
				int ns = f->info.a ?: 3;
				ns = ns * 2 + ns / 2;

#if 0
ns = f->info.l + 1;
				if(wrealloc(&f->info.v, sizeof(*f->info.v), ns, f->info.a) == 0)
					return nomem();
#else
				if(wrealloc(&f->info.v, sizeof(*f->info.v), ns, f->info.a) != 0)
					return nomem();
#endif
				f->info.a = ns;
			}

			// populate key
			if(f->info.v[f->info.l].ka <= kl)
			{
				int ns = f->info.v[f->info.l].ka;
				while(ns <= kl)
				{
					ns = ns ?: 10;
					ns = ns * 2 + ns / 2;
				}
				
				if(wrealloc(&f->info.v[f->info.l].ks, sizeof(*f->info.v[0].ks), ns, f->info.v[f->info.l].ka) != 0)
					return nomem();

				f->info.v[f->info.l].ka = ns;
			}

			memcpy(f->info.v[f->info.l].ks, k, kl);
			f->info.v[f->info.l].ks[kl] = 0;
			f->info.v[f->info.l].kl = kl;

			// populate value
			va_list va;
			va_start(va, vfmt);
			
			va_list va2;
			va_copy(va2, va);

			// measure
			int w = vsnprintf(0, 0, vfmt, va2);
			va_end(va2);

			// reallocate
			if(f->info.v[f->info.l].va <= w)
			{
				int ns = f->info.v[f->info.l].va;
				while(ns <= w)
				{
					ns = ns ?: 10;
					ns = ns * 2 + ns / 2;
				}
				
				if(wrealloc(&f->info.v[f->info.l].vs, sizeof(*f->info.v[0].vs), ns, f->info.v[f->info.l].va) != 0)
					return nomem();

				f->info.v[f->info.l].va = ns;
			}

			f->info.v[f->info.l].vl = vsprintf(f->info.v[f->info.l].vs, vfmt, va);
			va_end(va);

			f->info.l++;
		}
	}

	return 0;
}
