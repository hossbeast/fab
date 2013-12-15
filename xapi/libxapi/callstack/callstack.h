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

#ifndef _XAPI_CALLSTACK_H
#define _XAPI_CALLSTACK_H

#include <stdint.h>

/*
** callstack definition ; libxapi-visibility
*/

struct frame
{
	int type;		// 0=regular, 1=frame_static

	const struct etable *	etab;		// error table
	int16_t								code;		// error code

	const char * 		file;
	int							line;
	const char * 		func;

	char *		msg;		// error message
	int				msga;
	int				msgl;

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
};

/*
** the base frame and frames in the alt stack are statically allocated
** alt stack frames never have msg or info populated
*/ 
struct frame_static
{
	struct frame;

	char buf_msg[64];
	struct frame_info buf_info[3];
	char buf_info_ks[3][64];
	char buf_info_vs[3][64];
};

/// callstack
//
// SUMMARY
//  tracks the call stack
//
struct callstack
{
	struct
	{
		// storage of dynamically allocated frames
		struct
		{
			struct frame *	v;
			int							a;
		} stor;

		// the base frame is not dynamically allocated
		struct frame_static base;

		// the alt frames are not dynamically allocated
		struct frame_static alt[2];
	} frames;

	// stack frames
	struct frame ** v;
	int							a;

	// number of stack frames when unwinding
	int l;

	// number of frames deep that current execution requires
	int depth;

	struct
	{
		struct frame * v[2];
		int l;
		int depth;
	} alt;

	// transient value indicating that the alt stack is being populated
	int isalt;

	// transient value indicating that the current frame is finalized (execution has passed the XAPI_FINALLY label)
	int finalized;
};

// per-thread callstacks
extern __thread struct callstack callstack;

/// xapi_callstack_free
//
// SUMMARY
//  free the callstack for this thread with free semantics
//
void callstack_free();

#undef restrict
#endif
