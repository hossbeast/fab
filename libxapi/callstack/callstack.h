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

#ifndef _XAPI_CALLSTACK_H
#define _XAPI_CALLSTACK_H

struct memblk;			// memblk.h
struct callstack;

#define restrict __restrict

/// xapi_callstack_unwindto
//
// SUMMARY
//  unwind to the specified frame, discarding any error
//
// PARAMETERS
//  frame - frame to unwind to
//
void xapi_callstack_unwindto(int frame);

/// xapi_callstack_freeze
//
// SUMMARY
//  freeze the callstack for the current thread
//
// RETURNS
//  containing memblk
//
struct memblk * xapi_callstack_freeze();

/// xapi_callstack_unfreeze
//
// SUMMARY
//  reverse the effects of xapi_callstack_freeze
//
// RETURNS
//  containing memblk
//
void xapi_callstack_unfreeze();

/// xapi_callstack_thaw
//
// SUMMARY
//  recover a callstack frozen with xapi_callstack_freeze
//
// RETURNS
//  callstack instance (pointer-equivalent with mb)
//
struct callstack * xapi_callstack_thaw(char * const restrict mb)
	__attribute__((nonnull));

/* trace */

/// xapi_callstack_trace_pithy
//
// see xapi_trace_pithy
//
size_t xapi_callstack_trace_pithy(struct callstack * const restrict cs, char * const restrict dst, const size_t sz)
	__attribute__((nonnull));

/// xapi_callstack_trace_full
//
// see xapi_trace_full
//
size_t xapi_callstack_trace_full(struct callstack * const restrict cs, char * const restrict dst, const size_t sz)
	__attribute__((nonnull));

#undef restrict
#endif
