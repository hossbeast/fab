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

#include "xmem.h"

// per-thread callstacks
__thread struct callstack callstack;

// refers to the top frame
#define TOP (callstack.v[callstack.top])

static int nomem()
{
	/*
	** overwrite the current frame with an ENOMEM error (if there is one)
	*/
	TOP->etab = &errtab_SYS;
	TOP->code = SYS_ENOMEM;
	TOP->file = 0;
	TOP->line = 0;
	TOP->func = "xrealloc";

	return -1;
}

///
/// callstack manipulation
///
int API xapi_frame_push()
{
	/* ensure frame list allocation for at least 2 frames */
	if(callstack.a < (callstack.l + 1))
	{
		int ns = callstack.a ?: 10;
		ns = ns * 2 + ns / 2;
		if(xrealloc(&callstack.v, sizeof(*callstack.v), ns, callstack.a) != 0)
			return nomem();

		callstack.a = ns;
	}

	/* for the initial push, use the base frame */
	if(callstack.l == 0)
	{
		callstack.v[0] = &callstack.frames.base;
		callstack.l++;
		/* callstack.top++; */
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
//printf("-> %d\n", callstack.top);

	return 0;
}

int API xapi_frame_leave()
{
	int r = callstack.v[callstack.top--]->code;
//printf("%d <-\n", callstack.top);

	if(callstack.top == -1)
	{
		callstack_free();
	}

	return r;
}

void API xapi_frame_pop()
{
	callstack.l--;
	TOP->code = 0;
}

int API xapi_frame_top_code()
{
	return TOP->code;
}

void API xapi_frame_set(const etable * const etab, const int code, const char * const file, const int line, const char * const func)
{
	TOP->etab = etab;
	TOP->code = code;
	TOP->file = file;
	TOP->line = line;
	TOP->func = func;
}

int API xapi_frame_set_message(const char * const fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	
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
	va_end(va);

	return 0;
}

int API xapi_frame_add_info(char imp, const char * const k, int kl, const char * const vfmt, ...)
{
	if(TOP->code == 0)
		return 0;

	kl = kl ?: strlen(k);

	// reallocate the info list
	if(TOP->info.l == TOP->info.a)
	{
		int ns = TOP->info.a ?: 3;
		ns = ns * 2 + ns / 2;

		if(xrealloc(&TOP->info.v, sizeof(*TOP->info.v), ns, TOP->info.a) != 0)
			return nomem();

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

	return 0;
}
