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

/*
** callstack definition ; libxapi-visibility
*/

struct etable;

/// callstack
//
// SUMMARY
//  tracks the call stack
//
struct callstack
{
	struct
	{
		struct
		{
			struct frame
			{
				const struct etable *	etab;		// error table
				int										code;		// error code

				const char * 		file;
				int							line;
				const char * 		func;

				char *		msg;		// error message
				int				msga;
				int				msgl;

				struct
				{
					struct
					{
						char *	ks;		// key
						int			ka;
						int			kl;

						char *	vs;		// value
						int			va;
						int			vl;

						char		imp;	// important
					} * v;

					int a;
					int l;
				} info;
			} * v;	

			int a;
			int l;
		} stor;		// storage of dynamically allocated frames

		// the base frame is not dynamically allocated
		struct frame base;
	} frames;

	// stack frames
	struct frame ** v;
	int							a;

	// number of stack frames - grows with each call, shrinks with each successful exit
	int l;

	// current frame - grows and shrinks with each call
	int top;
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
