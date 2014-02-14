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

#define restrict __restrict

#if DEBUG
/*
** set to the callstack for the executing thread on every api call
** makes it easy to access the callstack from gdb
*/
typeof(callstack) * T;
#endif

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

#undef restrict

///
/// callstack manipulation
///
int API xapi_frame_enter()
{
#if DEBUG
	T = &callstack;
#endif
printf("ENTER, l %d -> %d, l %d\n", callstack.l, callstack.l + 1, callstack.l);

	// not unwinding
	callstack.x = -1;

	// ensure frame allocation
	if(callstack.l == callstack.a)
	{
		int ns = callstack.frames.stor.a ?: 10;
		ns = ns * 2 + ns / 2;

		if(wrealloc(&callstack.frames.stor.v, sizeof(*callstack.frames.stor.v), ns, callstack.frames.stor.a) == 0)
		{
			/*
			** the frame list is allocated with 2 extra slots ; one for the base frame (at the beginning) and one
			** for the alt frame (at the end)
			*/
			if(wrealloc(&callstack.v, sizeof(*callstack.v), ns + 2, callstack.a) == 0)
			{
				callstack.a = ns;
			}
		}
	}

	if(callstack.l == callstack.a)
	{
		// if the frame list has been allocated, append the alt frames to it
		if(callstack.v == 0)
		{
			callstack.v = &callstack.frames.alt;

			// base frame
			callstack.l++;
		}

		callstack.v[callstack.l] = &callstack.frames.alt;
		callstack.v[callstack.l]->finalized = 0;
		callstack.v[callstack.l]->populated = 0;
		callstack.v[callstack.l]->msgl = 0;
		callstack.v[callstack.l]->info.l = 0;
		callstack.l++;
		
		// populate the alternate frame with an ENOMEM error
		xapi_frame_set_and_leave(perrtab_SYS, SYS_ENOMEM, 0, 0, "malloc");

		return -1;
	}
	else
	{
		// push a frame for the base frame on the first push
		while(callstack.l < 2)
		{
			callstack.v[callstack.l] = &callstack.frames.stor[callstack.l];
			callstack.v[callstack.l]->code = 0;
			callstack.l++;
		}
	}

	return 0;
}

int API xapi_frame_leave()
{
#if DEBUG
	T = &callstack;
#endif

	int16_t rc = 0;
	int16_t rt = 0;

	if((rc = callstack.v[callstack.x]->code))
	{
		rt = callstack.v[callstack.x]->etab->id;
	}
	else if(callstack.v[callstack.l - 1]->code)
	{
		// unwinding; keep the frame
	}
	else
	{
		callstack.l--;
	}

	if(callstack.x-- == -1)
	{
		/*
		** x goes to -1 when a function exits that was not itself called with UNWIND-ing, e.g. main
		*/
		callstack_free();
	}

printf("LEAVE :: [x=%2d][l=%2d][t=%2d]\n", callstack.x, callstack.l, callstack.t);
	return (rt << 16) | rc;
}

void API xapi_frame_finalize()
{
#if DEBUG
	T = &callstack;
#endif

	callstack.v[callstack.x]->finalized = 1;
}

int API xapi_frame_finalized()
{
#if DEBUG
	T = &callstack;
#endif

	return callstack.v[callstack.x]->finalized;
}

int API xapi_unwinding()
{
#if DEBUG
	T = &callstack;
#endif

	return callstack.v[callstack.l - 1]->code;
}

void API xapi_frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
#if DEBUG
	T = &callstack;
	if(!callstack.v)
	{
		/*
		** when an UNWIND-ing function is called without fatal, AND there is an active callstack, then
		** when that function returns, the callstack is freed. while processing a subsequent error this
		** condition will be true and the program will segfault
		*/
		dprintf(2, "UNWIND-ing function called without fatal in the presence of active callstack\n");
	}
#endif

printf("setting reg[%d] %s at %s:%d\n", callstack.l, func, file, line);

	//
	// no-op if this frame has already been populated
	//
	
	if(callstack.x == -1)
		callstack.x = callstack.l - 1;

	struct frame * f = callstack.v[callstack.x];

	if(f->code)
	{
		f->populated = 1;
	}
	else
	{
		f->etab = etab;
		f->code = code ?: 1;
		f->file = file;
		f->line = line;
		f->func = func;

		f->finalized = 0;
		f->populated = 0;
		f->msgl = 0;
		f->info.l= 0;
	}
}

void API xapi_frame_set_and_leave(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
#if DEBUG
	T = &callstack;
#endif

	xapi_frame_set(etab, code, file, line, func);
	xapi_frame_leave();
}

void API xapi_frame_message(const char * const msg, int msgl)
{
#if DEBUG
	T = &callstack;
#endif

	struct frame * f = callstack.v[callstack.x];

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(fmt && f->populated == 0)
	{
		if(f->type)
		{
			struct frame_static * fs = (struct frame_static*)f;

			fs->msgl = MIN(sizeof(fs->buf_msg) - 1, msgl);
			memcpy(fs->buf_msg, msg, fs->msgl);
			fs->buf_msg[fs->msgl] = 0;
			fs->msg = fs->buf_msg;
		}
		else
		{
			// ensure allocation
			if(msgl >= f->msga)
			{
				int ns = f->msga ?: 10;
				while(msgl >= ns)
					ns = ns * 2 + ns / 2;

				if(wrealloc(&f->msg, sizeof(*f->msg), ns, f->msga) == 0)
					f->msga = ns;
			}

			if(msgl < f->msga)
			{
				f->msgl = msgl;
				memcpy(f->msg, msg, msgl);
				f->msg[msgl] = 0;
			}
		}
	}
}

void API xapi_frame_vmessage(const char * const fmt, ...)
{
#if DEBUG
	T = &callstack;
#endif

	struct frame * f = callstack.v[callstack.x];

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(fmt && f->populated == 0)
	{
		va_list va;
		va_start(va, fmt);

		if(f->type)
		{
			struct frame_static * fs = (struct frame_static*)f;

			fs->msgl = vsnprintf(fs->buf_msg, sizeof(fs->buf_msg), fmt, va);
			fs->msg = fs->buf_msg;
		}
		else
		{
			// measure the message
			va_list va2;
			va_copy(va2, va);

			int w = vsnprintf(0, 0, fmt, va2);
			va_end(va2);

			// ensure allocation
			if(w >= f->msga)
			{
				int ns = f->msga ?: 10;
				while(w >= ns)
					ns = ns * 2 + ns / 2;

				if(wrealloc(&f->msg, sizeof(*f->msg), ns, f->msga) == 0)
					f->msga = ns;
			}

			if(w < f->msga)
				f->msgl = vsprintf(f->msg, fmt, va);
		}

		va_end(va);
	}
}

void API xapi_frame_info(const char * const k, int kl, const char * const v, int vl)
{
#if DEBUG
	T = &callstack;
#endif

	if(k)
		kl = kl ?: strlen(k);
	if(v)
		vl = vl ?: strlen(v);

	struct frame * f = callstack.v[callstack.x];

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(k && kl && v && vl && f->populated == 0)
	{
		if(f->type)
		{
			struct frame_static * fs = (struct frame_static*)f;
			
			if(fs->info.l < (sizeof(fs->buf_info) / sizeof(fs->buf_info[0])))
			{
				fs->info.v = fs->buf_info;
				fs->info.v[fs->info.l].ks = fs->buf_info_ks[fs->info.l];
				fs->info.v[fs->info.l].vs = fs->buf_info_vs[fs->info.l];
				
				// key
				fs->info.v[fs->info.l].kl = MIN(sizeof(fs->buf_info_ks[0]) - 1, kl);
				memcpy(fs->info.v[fs->info.l].ks, k, fs->info.v[fs->info.l].kl);
				fs->info.v[fs->info.l].ks[fs->info.v[fs->info.l].kl] = 0;

				// value
				fs->info.v[fs->info.l].vl = MIN(sizeof(fs->buf_info_vs[0]) - 1, vl);
				memcpy(fs->info.v[fs->info.l].vs, v, fs->info.v[fs->info.l].vl);
				fs->info.v[fs->info.l].vs[fs->info.v[fs->info.l].vl] = 0;

				fs->info.l++;
			}
		}
		else
		{
			// ensure allocation for the info list
			if(f->info.l == f->info.a)
			{
				int ns = f->info.a ?: 3;
				ns = ns * 2 + ns / 2;

				if(wrealloc(&f->info.v, sizeof(*f->info.v), ns, f->info.a) == 0)
					f->info.a = ns;
			}

			if(f->info.l < f->info.a)
			{
				// ensure allocation for the key
				if(kl >= f->info.v[f->info.l].ka)
				{
					int ns = f->info.v[f->info.l].ka ?: 10;
					while(ns <= kl)
						ns = ns * 2 + ns / 2;
	
					if(wrealloc(&f->info.v[f->info.l].ks, sizeof(*f->info.v[0].ks), ns, f->info.v[f->info.l].ka) == 0)
						f->info.v[f->info.l].ka = ns;
				}

				if(kl < f->info.v[f->info.l].ka)
				{
					// ensure allocation for the value
					if(vl >= f->info.v[f->info.l].va)
					{
						int ns = f->info.v[f->info.l].va ?: 10;
						while(ns <= vl)
							ns = ns * 2 + ns / 2;
	
						if(wrealloc(&f->info.v[f->info.l].vs, sizeof(*f->info.v[0].vs), ns, f->info.v[f->info.l].va) == 0)
							f->info.v[f->info.l].va = ns;
					}

					if(vl < f->info.v[f->info.l].va)
					{
						// populate the key
						memcpy(f->info.v[f->info.l].ks, k, kl);
						f->info.v[f->info.l].ks[kl] = 0;
						f->info.v[f->info.l].kl = kl;

						// populate the value
						memcpy(f->info.v[f->info.l].vs, v, vl);
						f->info.v[f->info.l].vs[vl] = 0;
						f->info.v[f->info.l].vl = vl;

						f->info.l++;
					}

				}
				va_end(va);
			}
		}
	}
}

void API xapi_frame_vinfo(const char * const k, int kl, const char * const vfmt, ...)
{
#if DEBUG
	T = &callstack;
#endif

	kl = kl ?: strlen(k);

	struct frame * f = callstack.v[callstack.x];

	// measure the value
	va_list va;
	va_start(va, vfmt);
	
	va_list va2;
	va_copy(va2, va);

	int vl = vsnprintf(0, 0, vfmt, va2);
	va_end(va2);

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(k && kl && vl && f->populated == 0)
	{
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
				fs->info.v[fs->info.l].vl = vsnprintf(fs->info.v[fs->info.l].vs, sizeof(fs->buf_info_vs[0]), vfmt, va);

				fs->info.l++;
			}
		}
		else
		{
			// ensure allocation for the info list
			if(f->info.l == f->info.a)
			{
				int ns = f->info.a ?: 3;
				ns = ns * 2 + ns / 2;

				if(wrealloc(&f->info.v, sizeof(*f->info.v), ns, f->info.a) == 0)
					f->info.a = ns;
			}

			if(f->info.l < f->info.a)
			{

				// ensure allocation for the key
				if(kl >= f->info.v[f->info.l].ka)
				{
					int ns = f->info.v[f->info.l].ka ?: 10;
					while(ns <= kl)
						ns = ns * 2 + ns / 2;
	
					if(wrealloc(&f->info.v[f->info.l].ks, sizeof(*f->info.v[0].ks), ns, f->info.v[f->info.l].ka) == 0)
						f->info.v[f->info.l].ka = ns;
				}

				if(kl < f->info.v[f->info.l].ka)
				{
					// ensure allocation for the value
					if(vl >= f->info.v[f->info.l].va)
					{
						int ns = f->info.v[f->info.l].va ?: 10;
						while(ns <= vl)
							ns = ns * 2 + ns / 2;
	
						if(wrealloc(&f->info.v[f->info.l].vs, sizeof(*f->info.v[0].vs), ns, f->info.v[f->info.l].va) == 0)
							f->info.v[f->info.l].va = ns;
					}

					if(vl < f->info.v[f->info.l].va)
					{
						// populate the key
						memcpy(f->info.v[f->info.l].ks, k, kl);
						f->info.v[f->info.l].ks[kl] = 0;
						f->info.v[f->info.l].kl = kl;

						// populate the value
						f->info.v[f->info.l].vl = vsprintf(f->info.v[f->info.l].vs, vfmt, va);
						f->info.l++;
					}
				}
			}
		}
	}

	va_end(va);
}
