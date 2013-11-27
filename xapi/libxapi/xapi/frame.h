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

/*
** application-visible frame manipulation functions used in XAPI_UNWIND mode
**
** none of these functions are called directly by application code; they are accessed
** through the macros provided in xapi/unwind.h
*/

#define restrict __restrict

struct etable;
struct callstack;

/// xapi_frame_push
//
// SUMMARY
//  push a frame onto the callstack
//
int xapi_frame_push();

/// xapi_frame_leave
//
// callstack.top--
//
int xapi_frame_leave();

/// xapi_frame_pop
//
// callstack.l--
//
void xapi_frame_pop();

/// xapi_frame_top_code
//
// returns the code of the top frame
//
int xapi_frame_top_code();

/// xapi_frame_set
//
// SUMMARY
//  set properties for the top frame
//
// PARAMETERS
//  [etab] - errtab table
//  code   - error code
//  file   - file name
//  line   - line number
//  func   - function name
//
void xapi_frame_set(const struct etable * const restrict etab, const int code, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull(3,5)));

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET(etab, code)	\
	xapi_frame_set(etab, code, __FILE__, __LINE__, __FUNCTION__)

/// xapi_frame_set_message
//
// SUMMARY
//  set error message for the top frame
//
// PARAMETERS
//  fmt - format string
//
int xapi_frame_set_message(const char * const restrict fmt, ...)
	__attribute__((nonnull));

/// xapi_frame_add_info
//
// SUMMARY
//  add key/value info to the top frame
//
// PARAMETERS
//  imp  - whether this info is marked as important
//  k    - key
//  [kl] - key length, or 0 for strlen
//  vfmt - format string
//
int xapi_frame_add_info(char imp, const char * const k, int kl, const char * const restrict vfmt, ...)
	__attribute__((nonnull));

#undef restrict
#endif
