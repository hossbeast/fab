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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"

#include "memblk.def.h"
#include "macros.h"

#define restrict __restrict

static void frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func, int alt)
{
  if(calltree->exe->frames.l == calltree->exe->frames.a)
  {
    size_t ns = calltree->exe->frames.a;
    ns = ns * 2 + ns / 2;
    wrealloc(&calltree->exe->frames.v, sizeof(*calltree->exe->frames.v), ns, calltree->exe->frames.a);
    calltree->exe->frames.a = ns;
  }

	struct frame * f = calltree->exe->frames.v[calltree->exe->frames.l];

  f->etab = etab;
  f->code = code ?: etab->max + 1;	// no code given
  f->line = line;
  f->msgl = 0;
  f->info.l= 0;

  size_t funcl = strlen(func);
  size_t filel = 0;
  if(file)
    filel = strlen(file);

  scatw(&f->file, &f->filel, &f->filea, 10, file, filel);
  scatw(&f->func, &f->funcl, &f->funca, 10, func, funcl);
}

#undef restrict

///
/// calltree manipulation
///

#if XAPI_RUNTIME_CHECKS
int API xapi_frame_enter(void * calling_frame)
{
  if(calltree_frames.a <= calltree_frames.l)
  {
    size_t ns = calltree_frames.a;
    while(ns <= calltree_frames.l)
      ns = ns * 2 + ns / 2;
    if((calltree_frames.v = realloc(calltree_frames.v, sizeof(*calltree_frames.v) * ns)) == 0)
    {
      write(2, "failed to allocate");
    }
    calltree_frames.a = ns;
  }

  calltree_frames->v[calltree->frames.l++] = calling_frame;
}

int API xapi_frame_leave()
{
#if XAPI_RUNTIME_CHECKS
  calltree_frames.l--;   // discard the frame
#endif

  int r = 0;
  xapi_frame_leave3(0, 0, &r);
  return r;
}

API void * xapi_frame_caller()
{
  void * f = 0;
  if(calltree_frames.l)
    f = calltree_frames.v[calltree_frames.l - 1];

  return f;
}
#endif

typedef const etable * etabstar;
etabstar API xapi_frame_errtab()
{
#if DEVEL
	CT = calltree;
#endif

	if(calltree->v[calltree->l - 1]->code)
	{
		return calltree->v[calltree->l - 1]->etab;
	}

	return 0;
}

int API xapi_frame_errcode()
{
#if DEVEL
	CT = calltree;
#endif

	if(calltree->v[calltree->l - 1]->code)
	{
		return calltree->v[calltree->l - 1]->code;
	}

	return 0;
}

int API xapi_frame_errval()
{
#if DEVEL
	CT = calltree;
#endif

	if(calltree->v[calltree->l - 1]->code)
	{
		return (calltree->v[calltree->l - 1]->etab->id << 16) | calltree->v[calltree->l - 1]->code;
	}

	return 0;
}

int API xapi_unwinding()
{
#if DEVEL
	CT = calltree;
#endif

  return calltree;
}

int API xapi_frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
	return frame_set(etab, code, file, line, func, 0);
}

int API xapi_frame_set_messagew(const etable * const etab, const int16_t code, const char * const msg, int msgl, const char * const file, const int line, const char * const func)
{
#if DEVEL
	CT = calltree;
#endif

	int r = frame_set(etab, code, file, line, func, 0);

	msgl = msgl ?: msg ? strlen(msg) : 0;

	struct frame * f = calltree->v[calltree->x];
  struct frame * f = calltree->exe->

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(msgl && f->populated == 0)
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

	return r;
}

int API xapi_frame_set_messagef(const etable * const etab, const int16_t code, const char * const fmt, const char * const file, const int line, const char * const func, ...)
{
#if DEVEL
	CT = calltree;
#endif

	int r = frame_set(etab, code, file, line, func, 0);

	struct frame * f = calltree->v[calltree->x];

	//
	// no-op if either:
	//  - fmt is null, or
	//  - this frame is already populated
	//

	if(fmt && f->populated == 0)
	{
		va_list va;
		va_start(va, func);

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

	return r;
}

void API xapi_frame_infow(const char * const k, int kl, const char * const v, int vl)
{
#if DEVEL
	CT = calltree;
#endif

	//
	// no-op if not unwinding
	//
	if(calltree->v[calltree->l - 1]->code)
	{
		if(k)
			kl = kl ?: strlen(k);
		if(v)
			vl = vl ?: strlen(v);

		struct frame * f = calltree->v[calltree->x];

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
				}
			}
		}
	}
}

void API xapi_frame_infof(const char * const k, int kl, const char * const vfmt, ...)
{
#if DEVEL
	CT = calltree;
#endif

	//
	// no-op if not unwinding
	//
	if(calltree->v[calltree->l - 1]->code)
	{
		kl = kl ?: strlen(k);

		struct frame * f = calltree->v[calltree->x];

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
}

#if 0
#if XAPI_RUNTIME_CHECKS
int API xapi_frame_enter(void * calling_frame)
#else
int API xapi_frame_enter()
#endif
{
  size_t ns;
  size_t es;

	if(calltree == 0)
	{
		/* if this allocation fails, the program will segfault
		**  it WILL fail if MEMBLOCK_LARGE < sizeof(calltree)
		**  it could of course also fail if there is actually insufficient memory
		*/
		if(wmalloc(&calltree, sizeof(*calltree)))
    {
      write(2, "libxapi failed to allocate calltree");
    }
#if DEVEL
	CT = calltree;
#endif

    xapi_calltree_allocate(1);

    calltree->exe = &calltree->stacks.v[0];
	}

  if(calltree->exe->code)
  {
    calltree->exe = &calltree->stacks.v[calltree->stacks.l++]
  }

  else if(calltree->l == calltree->scheduled_allocation)
  {
    // calculate the new frame lead
    calltree->frame_lead = xapi_frame_lead + (calltree->l * xapi_frame_lead_factor);

    // new frame allocation
    xapi_calltree_allocate(calltree->frames.a + calltree->frame_lead + 1);    

    // schedule the next allocation
    calltree->scheduled_allocation = calltree->frames.a - calltree->frame_lead;
  }

#if 0
	if(calltree->v && calltree->v[calltree->l - 1]->code)
	{
		// unwinding is underway ; no new frame
		return calltree->r = -1;
	}

	// first push
	if(calltree->x == calltree->l == 0)
		calltree->x = -1;

//printf("ENTER :: [x=%2d][l=%2d] => ", calltree->x, calltree->l);

	// ensure frame allocation
	int ns = calltree->a;
	if(calltree->l == calltree->a)
	{
		ns = calltree->a ?: 10;
		ns = ns * 2 + ns / 2;

		/*
		** the frame list is allocated with 2 extra slots ; one for the base frame (at the beginning) and one
		** for the alt frame (at the end)
		*/
		if(wrealloc(&calltree->v, sizeof(*calltree->v), ns + 2, calltree->a) == 0)
		{
			if(wrealloc(&calltree->frames.stor, sizeof(*calltree->frames.stor), ns, calltree->a) == 0)
			{
				int x;
				for(x = calltree->a; x < ns; x++)
				{
					if(wrealloc(&calltree->frames.stor[x], sizeof(*calltree->frames.stor[0]), 1, 0))
						break;
				}

				calltree->a = x;
			}
		}
	}

	if(calltree->a != ns)
	{
		// if the frame list has not yet been allocated, use the static frame list
		if(calltree->v == 0)
			calltree->v = calltree->frames.alt_list;

		if(calltree->l == 0)	// base frame
		{
			calltree->v[calltree->l] = &calltree->frames.alt[0];
			calltree->v[calltree->l]->finalized = 0;
			calltree->v[calltree->l]->populated = 0;
			calltree->l++;
		}

		calltree->v[calltree->l] = &calltree->frames.alt[1];
		calltree->v[calltree->l]->finalized = 0;
		calltree->v[calltree->l]->populated = 0;
#if XAPI_RUNTIME_CHECKS
    calltree->v[calltree->l]->calling_frame = calling_frame;
#endif
		calltree->l++;

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", calltree->x, calltree->l, calling_frame);
#endif
		
		// populate the alternate frame with an ENOMEM error
		frame_set(perrtab_SYS, SYS_ENOMEM, 0, 0, "libxapi::malloc", 1);
		xapi_frame_leave();

		return calltree->r = 1;
	}

	// push a frame for the base frame on the first push
	do
	{
		calltree->v[calltree->l] = calltree->frames.stor[calltree->l];
		calltree->v[calltree->l]->code = 0;
		calltree->v[calltree->l]->finalized = 0;
		calltree->v[calltree->l]->populated = 0;
		calltree->v[calltree->l]->filel = 0;
		calltree->v[calltree->l]->funcl = 0;
		calltree->l++;
	} while(calltree->l < 2);

#endif

  calltree->exe->l++;

#if XAPI_RUNTIME_CHECKS
	calltree->v[calltree->l - 1]->calling_frame = calling_frame;
#endif

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", calltree->x, calltree->l, calling_frame);
#endif
	return calltree->r = 0;
}
#endif

#if 0
void API xapi_frame_leave3(const etable ** etab, int * code, int * rval)
{
#if DEVEL
	CT = calltree;
#endif
//printf("LEAVE :: [x=%2d][l=%2d] => ", calltree->x, calltree->l);

//	if((rc = calltree->v[calltree->x]->code))
//	if(((*code) = calltree->v[calltree->l - 1]->code))

	const etable * E = 0;
	int C = 0;
	int R = 0;

	if(calltree->v[calltree->l - 1]->code)
	{
		E = calltree->v[calltree->l - 1]->etab;
		C = calltree->v[calltree->l - 1]->code;
		R = (calltree->v[calltree->l - 1]->etab->id << 16) | calltree->v[calltree->l - 1]->code;

		// unwinding is underway
		if(--calltree->x == -1)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", calltree->x, calltree->l);

			/*
			** x goes to -1 when a function exits that was not itself called with UNWIND-ing, e.g. main
			*/
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", calltree->x, calltree->l, calltree->v[calltree->l - 1]->calling_frame);
#endif
		}
	}

	// discard the frame
	else
	{
		if(--calltree->l == 0)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", calltree->x, calltree->l);
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", calltree->x, calltree->l, calltree->v[calltree->l - 1]->calling_frame);
#endif
		}
	}

	if(etab) (*etab) = E;
	if(code) (*code) = C;
	if(rval) (*rval) = R;
}

int API xapi_frame_leave()
{
#if DEVEL
	CT = calltree;
#endif
	
	int rval;
	xapi_frame_leave3(0, 0, &rval);
	return rval;
}
#endif
