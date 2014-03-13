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
#if XAPI_RUNTIME_CHECKS
int xapi_frame_enter(void * calling_frame);
void * xapi_frame_caller();
int xapi_frame_depth();
#else
int xapi_frame_enter();
#endif

int xapi_frame_enter_last();

/// xapi_frame_leave
//
// SUMMARY
//  pop a frame from the callstack
//
// RETURNS
//  zero when no error has been raised; otherwise, the error id
//
int xapi_frame_leave();

/// xapi_frame_leave2
//
// SUMMARY
//  as for xapi_frame_leave, but returns a pointer to the error table and the error code
//
// REMARKS
//  the return value from xapi_frame_leave is a composite of the etab id and error code
//
int xapi_frame_leave2(const etable ** etab, int * code);

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
//  etab   - errtab table
//  code   - error code
//  [msg]  - message
//  [msgl] - message length (0 for strlen)
//  [fmt]  - format string
//  [file] - file name
//  [line] - line number
//  func   - function name
//
// VARIANTS
//  messagew - message is specified as pointer/length pair
//  messagef - message is specified in printf/style
//
// RETURNS
//  a value indicating whether to jump to the finally label for the current frame
//
int xapi_frame_set(const struct etable * const restrict etab, const int16_t code, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull(5)));

int xapi_frame_set_messagew(const struct etable * const restrict etab, const int16_t code, const char * const restrict msg, int msgl, const char * const restrict file, const int line, const char * const restrict func)
	__attribute__((nonnull(7)));

int xapi_frame_set_messagef(const struct etable * const restrict etab, const int16_t code, const char * const restrict fmt, const char * const restrict file, const int line, const char * const restrict func, ...)
	__attribute__((nonnull(6)));

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET(etab, code)	\
	xapi_frame_set(etab, code, __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET_MESSAGEW(etab, code, msg, msgl)	\
	xapi_frame_set_messagew(etab, code, msg, msgl, __FILE__, __LINE__, __FUNCTION__)

// call xapi_frame_set with current file name, line number, and function name
#define XAPI_FRAME_SET_MESSAGEF(etab, code, fmt, ...)	\
	xapi_frame_set_messagef(etab, code, fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/// xapi_frame_info
//
// SUMMARY
//  add key/value info to the top frame (no-op if key or value is null or 0-length)
//
// PARAMETERS
//  kstr   - key string
//  [kl]   - key length or 0 for strlen
//  [vstr] - value string
//  [vl]   - vstr length or 0 for strlen
//  [vfmt] - format string for value
//
// VARIANTS
//  infow - value is specified in as pointer/length pair
//  infof - value is specified in printf/style
//
void xapi_frame_infow(const char * const restrict k, int kl, const char * const restrict vstr, int vlen);
void xapi_frame_infof(const char * const restrict k, int kl, const char * const restrict vfmt, ...);

#undef restrict
#endif
