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

#include "internal.h"

#include "memblk.def.h"
#include "macros.h"

// per-thread callstack storage
__thread struct callstack * callstack;
__thread struct memblk callstack_mb;

#define restrict __restrict

static int wmalloc(void * p, size_t sz)
{
#define MEMBLOCK_SMALL			0x1000 /* 1k : first THRESHOLD blocks */
#define MEMBLOCK_THRESHOLD	0x4			
#define MEMBLOCK_LARGE			0x8000 /* 8k : additional blocks */

	// request is too large to satisfy
	if(sz > MEMBLOCK_LARGE)
		return 1;

	struct memblk * mb = &callstack_mb;

	/* current block is full */
	if(mb->blocksl == 0 || ((mb->blocks[mb->blocksl - 1].l + sz) > mb->blocks[mb->blocksl - 1].a))
	{
		/* reallocate the block container */
		if(mb->blocksl == mb->blocksa)
		{
			size_t ns = mb->blocksa ?: 3;
			ns = ns * 2 + ns / 2;
			if((mb->blocks = calloc(sizeof(*mb->blocks) * ns, 1)) == 0)
				return 1;
			mb->blocksa = ns;
		}

		if((mb->blocksl < MEMBLOCK_THRESHOLD) && (sz <= MEMBLOCK_SMALL))
			mb->blocks[mb->blocksl].a = MEMBLOCK_SMALL;
		else
			mb->blocks[mb->blocksl].a = MEMBLOCK_LARGE;

		if((mb->blocks[mb->blocksl].s = calloc(sizeof(*mb->blocks[0].s) * mb->blocks[mb->blocksl].a, 1)) == 0)
			return 1;

		// cumulative offset
		if(mb->blocksl)
			mb->blocks[mb->blocksl].o = mb->blocks[mb->blocksl - 1].o + mb->blocks[mb->blocksl - 1].l;

		mb->blocksl++;
	}

	*(void**)p = mb->blocks[mb->blocksl - 1].s + mb->blocks[mb->blocksl - 1].l;
	mb->blocks[mb->blocksl - 1].l += sz;

	return 0;
}

static int wrealloc(void * p, size_t es, size_t ec, size_t oec)
{
	void * old = *(void**)p;
	if(wmalloc(p, es * ec))
		return 1;
	if(old)
		memcpy(*(void**)p, old, es * oec);

	return 0;
}

static int scatw(char ** dst, size_t * dstl, size_t * dsta, size_t seed, const char * s, size_t l)
{
	if(l >= (*dsta))
	{
		size_t ns = (*dsta) ?: seed;
		while(ns <= l)
			ns = ns * 2 + ns / 2;
		if(wrealloc(dst, 1, ns, *dsta))
			return 1;
		(*dsta) = ns;
	}

	(*dstl) = l;
	memcpy(*dst, s, l);
	(*dst)[l] = 0;

	return 0;
}

static int frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func, int alt)
{
	//
	// first frame_set initiates unwinding ; initialize x
	//
	if(callstack->x == -1)
	{
		callstack->x = callstack->l - 1;
	}

	struct frame * f = callstack->v[callstack->x];

	if(f->funcl)
	{
		//
		// no-op if this frame has already been populated
		//
		
		f->populated = 1;
	}
	else
	{
		f->etab = etab;
		f->code = code ?: etab->max + 1;	// no code given
		f->line = line;
		f->msgl = 0;
		f->info.l= 0;

		size_t funcl = strlen(func);
		size_t filel = 0;
		if(file)
			filel = strlen(file);

		if(f->type)
		{
			struct frame_static * fs = (struct frame_static*)f;

			fs->filel = MIN(sizeof(fs->buf_file) - 1, filel);
			memcpy(fs->buf_file, file, fs->filel);
			fs->buf_file[fs->filel] = 0;
			fs->file = fs->buf_file;

			fs->funcl = MIN(sizeof(fs->buf_func) - 1, funcl);
			memcpy(fs->buf_func, func, fs->funcl);
			fs->buf_func[fs->funcl] = 0;
			fs->func = fs->buf_func;
		}
		else
		{
			if(scatw(&f->file, &f->filel, &f->filea, 10, file, filel))
				return 1;

			if(scatw(&f->func, &f->funcl, &f->funca, 10, func, funcl))
				return 1;
		}
	}

	// whether to jump
	return !f->finalized;
}

#undef restrict

///
/// callstack manipulation
///
int API xapi_frame_enter_last()
{
	return callstack->r;
}

#if XAPI_RUNTIME_CHECKS
int API xapi_frame_enter(void * calling_frame)
#else
int API xapi_frame_enter()
#endif
{
	if(callstack == 0)
	{
		/* if this allocation fails, the program will segfault
		**  it WILL fail if MEMBLOCK_LARGE < sizeof(callstack)
		**  it could of course also fail if there is actually insufficient memory
		*/
		wmalloc(&callstack, sizeof(*callstack));
	}
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v && callstack->v[callstack->l - 1]->code)
	{
		// unwinding is underway ; no new frame
		callstack->r = -1;
		return -1;
	}

	// first push
	if(callstack->x == 0 && callstack->l == 0)
		callstack->x = -1;

//printf("ENTER :: [x=%2d][l=%2d] => ", callstack->x, callstack->l);

	// ensure frame allocation
	int ns = callstack->a;
	if(callstack->l == callstack->a)
	{
		ns = callstack->a ?: 10;
		ns = ns * 2 + ns / 2;

		/*
		** the frame list is allocated with 2 extra slots ; one for the base frame (at the beginning) and one
		** for the alt frame (at the end)
		*/
		if(wrealloc(&callstack->v, sizeof(*callstack->v), ns + 2, callstack->a) == 0)
		{
			if(wrealloc(&callstack->frames.stor, sizeof(*callstack->frames.stor), ns, callstack->a) == 0)
			{
				int x;
				for(x = callstack->a; x < ns; x++)
				{
					if(wrealloc(&callstack->frames.stor[x], sizeof(*callstack->frames.stor[0]), 1, 0))
						break;
				}

				callstack->a = x;
			}
		}
	}

	if(callstack->a != ns)
	{
		// if the frame list has not yet been allocated, use the static frame list
		if(callstack->v == 0)
			callstack->v = callstack->frames.alt_list;

		if(callstack->l == 0)	// base frame
		{
			callstack->v[callstack->l] = &callstack->frames.alt[0];
			callstack->v[callstack->l]->finalized = 0;
			callstack->v[callstack->l]->populated = 0;
			callstack->l++;
		}

		callstack->v[callstack->l] = &callstack->frames.alt[1];
		callstack->v[callstack->l]->finalized = 0;
		callstack->v[callstack->l]->populated = 0;
#if XAPI_RUNTIME_CHECKS
	callstack->v[callstack->l]->calling_frame = calling_frame;
#endif
		callstack->l++;

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", callstack->x, callstack->l, calling_frame);
#endif
		
		// populate the alternate frame with an ENOMEM error
		frame_set(perrtab_SYS, SYS_ENOMEM, 0, 0, "libxapi::malloc", 1);
		xapi_frame_leave();

		callstack->r = 1;
		return 1;
	}

	// push a frame for the base frame on the first push
	do
	{
		callstack->v[callstack->l] = callstack->frames.stor[callstack->l];
		callstack->v[callstack->l]->code = 0;
		callstack->v[callstack->l]->finalized = 0;
		callstack->v[callstack->l]->populated = 0;
		callstack->v[callstack->l]->filel = 0;
		callstack->v[callstack->l]->funcl = 0;
		callstack->l++;
	} while(callstack->l < 2);

#if XAPI_RUNTIME_CHECKS
	callstack->v[callstack->l - 1]->calling_frame = calling_frame;
#endif

#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", callstack->x, callstack->l, calling_frame);
#endif
	callstack->r = 0;
	return 0;
}

int API xapi_frame_depth()
{
	return callstack ? callstack->l : 0;
}

#if XAPI_RUNTIME_CHECKS
typedef void * voidstar;
voidstar API xapi_frame_caller()
{
	if(callstack->v[callstack->l - 1]->code)
		return callstack->v[callstack->x]->calling_frame;
	else
		return callstack->v[callstack->l - 1]->calling_frame;
}
#endif

void API xapi_frame_leave3(const etable ** etab, int * code, int * rval)
{
#if DEVEL
	CS = callstack;
#endif
//printf("LEAVE :: [x=%2d][l=%2d] => ", callstack->x, callstack->l);

//	if((rc = callstack->v[callstack->x]->code))
//	if(((*code) = callstack->v[callstack->l - 1]->code))

	const etable * E = 0;
	int C = 0;
	int R = 0;

	if(callstack->v[callstack->l - 1]->code)
	{
		E = callstack->v[callstack->l - 1]->etab;
		C = callstack->v[callstack->l - 1]->code;
		R = (callstack->v[callstack->l - 1]->etab->id << 16) | callstack->v[callstack->l - 1]->code;

		// unwinding is underway
		if(--callstack->x == -1)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", callstack->x, callstack->l);

			/*
			** x goes to -1 when a function exits that was not itself called with UNWIND-ing, e.g. main
			*/
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", callstack->x, callstack->l, callstack->v[callstack->l - 1]->calling_frame);
#endif
		}
	}

	// discard the frame
	else
	{
		if(--callstack->l == 0)
		{
//printf("[x=%2d][l=%2d] CALLSTACK_FREE\n", callstack->x, callstack->l);
			callstack_free();
		}
		else
		{
#if XAPI_RUNTIME_CHECKS
//printf("[x=%2d][l=%2d] %p\n", callstack->x, callstack->l, callstack->v[callstack->l - 1]->calling_frame);
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
	CS = callstack;
#endif
	
	int rval;
	xapi_frame_leave3(0, 0, &rval);
	return rval;
}

typedef const etable * etabstar;
etabstar API xapi_frame_errtab()
{
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v[callstack->l - 1]->code)
	{
		return callstack->v[callstack->l - 1]->etab;
	}

	return 0;
}

int API xapi_frame_errcode()
{
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v[callstack->l - 1]->code)
	{
		return callstack->v[callstack->l - 1]->code;
	}

	return 0;
}

int API xapi_frame_errval()
{
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v[callstack->l - 1]->code)
	{
		return (callstack->v[callstack->l - 1]->etab->id << 16) | callstack->v[callstack->l - 1]->code;
	}

	return 0;
}

void API xapi_frame_finalize()
{
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v[callstack->l - 1]->code)
		callstack->v[callstack->x]->finalized = 1;
	else
		callstack->v[callstack->l - 1]->finalized = 1;
}

int API xapi_frame_finalized()
{
#if DEVEL
	CS = callstack;
#endif

	if(callstack->v[callstack->l - 1]->code)
		return callstack->v[callstack->x]->finalized;
	else
		return callstack->v[callstack->l - 1]->finalized;
}

int API xapi_unwinding()
{
#if DEVEL
	CS = callstack;
#endif

	return callstack->v[callstack->l - 1]->code;
}

int API xapi_frame_set(const etable * const etab, const int16_t code, const char * const file, const int line, const char * const func)
{
	return frame_set(etab, code, file, line, func, 0);
}

int API xapi_frame_set_messagew(const etable * const etab, const int16_t code, const char * const msg, int msgl, const char * const file, const int line, const char * const func)
{
#if DEVEL
	CS = callstack;
#endif

	int r = frame_set(etab, code, file, line, func, 0);

	msgl = msgl ?: msg ? strlen(msg) : 0;

	struct frame * f = callstack->v[callstack->x];

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
	CS = callstack;
#endif

	int r = frame_set(etab, code, file, line, func, 0);

	struct frame * f = callstack->v[callstack->x];

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
	CS = callstack;
#endif

	//
	// no-op if not unwinding
	//
	if(callstack->v[callstack->l - 1]->code)
	{
		if(k)
			kl = kl ?: strlen(k);
		if(v)
			vl = vl ?: strlen(v);

		struct frame * f = callstack->v[callstack->x];

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
	CS = callstack;
#endif

	//
	// no-op if not unwinding
	//
	if(callstack->v[callstack->l - 1]->code)
	{
		kl = kl ?: strlen(k);

		struct frame * f = callstack->v[callstack->x];

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
