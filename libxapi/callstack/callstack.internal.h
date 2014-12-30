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

#ifndef _XAPI_CALLSTACK_INTERNAL_H
#define _XAPI_CALLSTACK_INTERNAL_H

#include <stdint.h>

/*
** callstack definition ; libxapi-visibility
**
** the callstack represents execution from the first UNWIND-ing call to the site of the first error
**
** errors that take place beyond the first (which must necessarily begin after the XAPI_FINALLY label
** in the failing function) are not tracked. normally, the first error should trigger cleanup/unwinding
** but there are certain cleanup functions which can fail (close is the best example)
*/

#if DEVEL
/*
** set to the callstack for the executing thread on every api call
** makes it easy to access the callstack from gdb
*/
struct callstack;
struct callstack * CS;
#endif

struct frame
{
	int type;		// 0=frame, 1=frame_static

	const struct etable *	etab;		// error table
	int16_t								code;		// error code

	int							line;

	char * 					file;		// file name
	size_t					filel;
	size_t					filea;

	char * 					func;		// function name
	size_t					funcl;
	size_t					funca;

	char *					msg;		// error message
	int							msga;
	int							msgl;

	struct
	{
		struct frame_info
		{
			char *	ks;		// key
			int			ka;
			int			kl;

			char *	vs;		// value
			int			va;
			int			vl;
		} * v;

		int a;
		int l;
	} info;

	int finalized;		// whether execution has passed the XAPI_FINALLY label in this frame
	int populated;		// whether this frame has already been populated (not necessarily set, but if it is set, then its true)

#if XAPI_RUNTIME_CHECKS
	void * calling_frame;	// address of the calling stack frame
#endif
};

/*
** there are 3 statically allocated frames:
**  - the base frame
**  - 2 frames for out of memory reporting if libxapi cannot allocate more frames
*/ 
struct frame_static
{
	struct frame;

	char buf_file[64];
	char buf_func[64];
	char buf_msg[64];
	struct frame_info buf_info[3];
	char buf_info_ks[3][64];
	char buf_info_vs[3][64];
};

/// callstack
//
// SUMMARY
//  its a call stack
//
struct callstack
{
	struct
	{
		// dynamically allocated frames
		struct frame ** 		stor;

		// statically allocated frames
		struct frame_static	alt[2];

		struct frame *			alt_list[2];
	} frames;

	// stack frame list
	struct frame ** v;
	int							a;

	int l;	// 1-based count of stack frames (depth)
	int x;	// while unwinding, 0-based index of current stack frame; otherwise, undefined

	int r;	// return value from the last xapi_frame_set call
};

// per-thread callstacks
extern __thread struct callstack * callstack;

// per-thread memblocks to hold the callstacks
extern __thread struct memblk callstack_mb;

/// xapi_callstack_free
//
// SUMMARY
//  free the callstack for this thread with free semantics
//
void callstack_free();

#undef restrict
#endif
