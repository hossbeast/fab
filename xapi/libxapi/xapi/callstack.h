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

#define restrict __restrict

struct etable;

/// callstack
//
// SUMMARY
//  tracks the call stack
//
typedef struct
{
	struct
	{
		struct frame
		{
			struct etable *	table;	// error table
			int				code;		// error code

			char * 		file;
			char *		line;
			char * 		func;

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
	} frames;

	int top;		// current frame
} callstack_t;

// per-thread callstack
extern __thread callstack_t callstack;

/// callstack_push
//
// SUMMARY
//  add a frame to the callstack
//
int callstack_push(const int n);

/// callstack_frame
//
// SUMMARY
//  pop a frame from the callstack
//
// RETURNS
//  the frames return code
//
int callstack_frame(const struct etable * const restrict etab, const int code, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull));

#define CALLSTACK_FRAME(table, code)	\
	callstack_pop(table, code, __FILE__, __LINE__, __FUNCTION__)

/// callstack_frame_message
//
// SUMMARY
//  add error message to the top frame
//
int callstack_frame_message(const char * const restrict fmt, ...)
	__attribute__((nonnull));

/// callstack_frame_info
//
// SUMMARY
//  add key/value info to the top frame
//
int callstack_frame_info(char imp, const char * const k, int kl, const char * const restrict vfmt, ...)
	__attribute__((nonnull));

#undef restrict
#endif
