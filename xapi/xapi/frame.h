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

#ifndef _XAPI_FRAME_H
#define _XAPI_FRAME_H

#include <stdint.h>

/*
** application-visible frame manipulation functions used in XAPI_UNWIND mode
**
** none of these functions are called directly by application code; they are accessed
** through the macros provided in xapi/unwind.h
*/

#define restrict __restrict

struct etable;
struct callstack;

/// xapi_frame_enter
//
// SUMMARY
//  push a frame onto the callstack
//
// RETURNS
//  zero on success
//
int xapi_frame_enter();

/// xapi_frame_leave
//
// SUMMARY
//  pop a frame from the callstack
//
// RETURNS
//  zero when no error has been raised; otherwise, the error id
//
int xapi_frame_leave();

/// xapi_unwinding
//
// SUMMARY
//  true if an error has been raised
//
int xapi_unwinding();

/// xapi_frame_finalize
//
// SUMMARY
//  record that execution has passed the XAPI_FINALLY label in the current frame
//
void xapi_frame_finalize();

/// xapi_frame_finalized
//
// SUMMARY
//  true if execution has passed the XAPI_FINALLY label in current frame
//
int xapi_frame_finalized();

/// xapi_frame_set
//
// SUMMARY
//  set properties for the top frame
//
// PARAMETERS
//  [etab] - errtab table
//  code   - error code
//  [file] - file name
//  [line] - line number
//  [func] - function name
//
void xapi_frame_set(const struct etable * const restrict etab, const int16_t code, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull));

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET(etab, code)	\
	xapi_frame_set(etab, code, __FILE__, __LINE__, __FUNCTION__)

/// xapi_frame_message/xapi_frame_vmessage
//
// SUMMARY
//  set error message for the top frame (no-op if msg/fmt is null or 0-length)
//
// PARAMETERS
//  [msg] - message
//  [fmt] - format string
//
void xapi_frame_message(const char * const restrict msg);

void xapi_frame_vmessage(const char * const restrict fmt, ...);

/// xapi_frame_info/xapi_frame_vinfo
//
// SUMMARY
//  add key/value info to the top frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  k      - key
//  [kl]   - key length, or 0 for strlen
//  [v]    - value
//  [vl]   - value length
//  [vfmt] - format string for value
//
void xapi_frame_info(const char * const restrict k, int kl, const char * const restrict v, int vl);

void xapi_frame_vinfo(const char * const restrict k, int kl, const char * const restrict vfmt, ...);

/// xapi_frame_set_and_leave
//
// SUMMARY
//  call xapi_frame_set then xapi_frame_leave
//
void xapi_frame_set_and_leave(const struct etable * const restrict etab, const int16_t code, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull));

// call xapi_frame_set_and_leave with current file name, line number, and function name
#define XAPI_FRAME_SET_AND_LEAVE(etab, code)	\
	xapi_frame_set_and_leave(etab, code, __FILE__, __LINE__, __FUNCTION__)

#undef restrict
#endif
