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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "xapi/internal.h"

#include "macros.h"
#include "xmem.h"

// per-thread callstacks
__thread struct callstack callstack;

// refers to the top frame
#define TOP (callstack.v[callstack.top])

#define restrict __restrict

typeof(callstack) * T;

static void frame_set(struct frame * const restrict frame, const etable * const restrict etab, const uint16_t code, const char * const restrict file, const int line, const char * const restrict func)
{
T = &callstack;

	frame->etab = etab;
	frame->code = code;
	frame->file = file;
	frame->line = line;
	frame->func = func;

	frame->msgl = 0;
	frame->info.l = 0;
}

static int nomem()
{
	// populate the alternate frame with an ENOMEM error
	frame_set(&callstack.frames.alt[0], perrtab_SYS, SYS_ENOMEM, 0, 0, "xrealloc");

	xapi_frame_leave();

	return -1;
}

#undef restrict

///
/// callstack manipulation
///
int API xapi_frame_push()
{
	// if the current frame has already been finalized, do nothing ; we are now using the alt stack
	if(callstack.finalized)
	{
		return 0;
	}

	/* ensure frame list allocation for at least 2 frames */
	if(callstack.a < (callstack.l + 1))
	{
		int ns = callstack.a ?: 10;
		ns = ns * 2 + ns / 2;
		if(xrealloc(&callstack.v, sizeof(*callstack.v), ns, callstack.a) != 0)
			return nomem();

		callstack.a = ns;
	}

	/* the initial frame-push, i.e. in a function that was not itself fatally called */
	if(callstack.l == 0)
	{
		/* use the base frame for the current call */
		callstack.v[0] = &callstack.frames.base;
		callstack.v[0]->type = 1;
		callstack.l = 1;

		/* reset some state */
		callstack.top = 0;
		callstack.frames.stor.l = 0;
		callstack.frames.alt[0].code = 0;
		callstack.frames.alt[1].code = 0;

		callstack.alt_len = 0;
	}

	/* dynamic frame allocation */
	if(callstack.frames.stor.a == callstack.frames.stor.l)
	{
		int ns = callstack.frames.stor.a ?: 10;
		ns = ns * 2 + ns / 2;
		if(xrealloc(&callstack.frames.stor.v, sizeof(*callstack.frames.stor.v), ns, callstack.frames.stor.a) != 0)
			return nomem();

		callstack.frames.stor.a = ns;
	}

	callstack.v[callstack.l] = &callstack.frames.stor.v[callstack.frames.stor.l++];
	callstack.l++;
	callstack.top++;

	return 0;
}

void API xapi_frame_leave()
{
	if(callstack.finalized)
	{
		callstack.alt_len++;
	}
	else
	{
		callstack.top--;
	}
}

int API xapi_frame_exit()
{
	uint16_t rc = callstack.v[callstack.l - 1]->code;
	int16_t rt = (rc == 0 || rc == 0xFFFF) ? 0 : callstack.v[callstack.l - 1]->etab->id;

	callstack.top--;

	if(callstack.top == -1)
	{
		callstack_free();
	}

	callstack.finalized = 0;

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

void API xapi_frame_pop()
{
	callstack.l--;
	TOP->code = 0;
}

int API xapi_frame_top_code()
{
	uint16_t rc = TOP->code;
	int16_t rt = (rc == 0 || rc == 0xFFFF) ? 0 : TOP->etab->id;

	return (rt << 16) | rc;
}

int API xapi_frame_top_code_alt()
{
	uint16_t rc = callstack.frames.alt[0].code;
	int16_t rt = (rc == 0 || rc == 0xFFFF) ? 0 : callstack.frames.alt[0].etab->id;

	return (rt << 16) | rc;
}

void API xapi_frame_set(const etable * const etab, const uint16_t code, const char * const file, const int line, const char * const func)
{
	if(callstack.finalized)
	{
		if(callstack.alt_len < (sizeof(callstack.frames.alt) / sizeof(callstack.frames.alt[0])))
		{
			frame_set(&callstack.frames.alt[callstack.alt_len], etab, code, file, line, func);
		}
		else
		{
			/* lost */
		}
	}
	else
	{
		frame_set(TOP, etab, code, file, line, func);
	}
}

int API xapi_frame_set_message(const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);

	if(callstack.finalized)
	{
		dprintf(2, "MESSAGE ON ALT STACK\n");
/*
		int w;

		if(callstack.alt_len < (sizeof(callstack.frames.alt) / sizeof(callstack.frames.alt[0])))
		{
			struct frame_static * f = &callstack.frames.alt[callstack.alt_len];

			w = vsnprintf(f->buf_msg, sizeof(f->buf_msg), fmt, va);

			f->msg  = f->buf_msg;
			f->msgl = w;
		}
*/
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
			
			if(xrealloc(&TOP->msg, sizeof(*TOP->msg), ns, TOP->msga) != 0)
				return nomem();

			TOP->msga = ns;
		}

		TOP->msgl = vsprintf(TOP->msg, fmt, va);
	}

	va_end(va);

	return 0;
}

int API xapi_frame_add_info(char imp, const char * const k, int kl, const char * const vfmt, ...)
{
	if(TOP->code == 0)
		return 0;

	kl = kl ?: strlen(k);

	if(TOP->type)
	{
		struct frame_static * f = (struct frame_static*)TOP;
		
		if(f->info.l < (sizeof(f->buf_info) / sizeof(f->buf_info[0])))
		{
			f->info.v = f->buf_info;
			f->info.v[f->info.l].imp = imp;
			f->info.v[f->info.l].ks = f->buf_info_ks[f->info.l];
			f->info.v[f->info.l].vs = f->buf_info_vs[f->info.l];
			
			// key
			memcpy(f->info.v[f->info.l].ks, k, MIN(sizeof(f->buf_info_ks[0]), kl - 1));
			f->info.v[f->info.l].ks[kl] = 0;
			f->info.v[f->info.l].kl = kl;

			// value
			va_list va;
			va_start(va, vfmt);

			f->info.v[f->info.l].vl = vsnprintf(f->info.v[f->info.l].vs, sizeof(f->buf_info_vs[0]), vfmt, va);

			f->info.l++;
		}
	}
	else
	{
		// reallocate the info list
		if(TOP->info.l == TOP->info.a)
		{
			int ns = TOP->info.a ?: 3;
			ns = ns * 2 + ns / 2;

static int C;
			if(0 && C++ == 0)
			{
				if(xrealloc(&TOP->info.v, sizeof(*TOP->info.v), ns, TOP->info.a) == 0)
					return nomem();
			}
			else
			{
				if(xrealloc(&TOP->info.v, sizeof(*TOP->info.v), ns, TOP->info.a) != 0)
					return nomem();
			}

			TOP->info.a = ns;
		}

		TOP->info.v[TOP->info.l].imp = imp;

		// populate key
		if(TOP->info.v[TOP->info.l].ka <= kl)
		{
			int ns = TOP->info.v[TOP->info.l].ka;
			while(ns <= kl)
			{
				ns = ns ?: 10;
				ns = ns * 2 + ns / 2;
			}
			
			if(xrealloc(&TOP->info.v[TOP->info.l].ks, sizeof(*TOP->info.v[0].ks), ns, TOP->info.v[TOP->info.l].ka) != 0)
				return nomem();

			TOP->info.v[TOP->info.l].ka = ns;
		}

		memcpy(TOP->info.v[TOP->info.l].ks, k, kl);
		TOP->info.v[TOP->info.l].ks[kl] = 0;
		TOP->info.v[TOP->info.l].kl = kl;

		// populate value
		va_list va;
		va_start(va, vfmt);
		
		va_list va2;
		va_copy(va2, va);

		// measure
		int w = vsnprintf(0, 0, vfmt, va2);
		va_end(va2);

		// reallocate
		if(TOP->info.v[TOP->info.l].va <= w)
		{
			int ns = TOP->info.v[TOP->info.l].va;
			while(ns <= w)
			{
				ns = ns ?: 10;
				ns = ns * 2 + ns / 2;
			}
			
			if(xrealloc(&TOP->info.v[TOP->info.l].vs, sizeof(*TOP->info.v[0].vs), ns, TOP->info.v[TOP->info.l].va) != 0)
				return nomem();

			TOP->info.v[TOP->info.l].va = ns;
		}

		TOP->info.v[TOP->info.l].vl = vsprintf(TOP->info.v[TOP->info.l].vs, vfmt, va);
		va_end(va);

		TOP->info.l++;
	}

	return 0;
}
