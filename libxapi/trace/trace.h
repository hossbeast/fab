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

#ifndef _XAPI_TRACE_H
#define _XAPI_TRACE_H

#if XAPI_STACKTRACE

#include <sys/types.h>

struct calltree;

#define restrict __restrict

/*
** called after finally iff XAPI_UNWINDING
*/

/// xapi_pithytrace
//
// SUMMARY
//  call xapi_trace_pithy and write the output to stderr
//
// PARAMETERS
//  [to] - file descriptor to write to
//
void xapi_pithytrace(void);
void xapi_pithytrace_to(int fd);

/// xapi_fulltrace
/// xapi_backtrace
//
// SUMMARY
//  call xapi_trace_full and write the output to stderr
//
// PARAMETERS
//  [to] - file descriptor to write to
//
void xapi_fulltrace(void);
void xapi_fulltrace_to(int fd);
void xapi_backtrace(void);
void xapi_backtrace_to(int fd);

/// xapi_trace_pithy
//
// SUMMARY
//  write a string summarizing the entire calltree
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//
size_t xapi_trace_pithy(char * const restrict dst, const size_t sz)
  __attribute__((nonnull));

/// xapi_trace_full
//
// SUMMARY
//  write a string describing the entire calltree in full
//
// REMARKS
//  will contain newlines for a multi-frame calltree, but does not terminate with a newline
//
// PARAMETERS
//  dst  - buffer to write to
//  sz   - max bytes to write, including null byte
//
size_t xapi_trace_full(char * const restrict dst, const size_t sz)
  __attribute__((nonnull));

/// xapi_trace_calltree_pithy
//
// SEE xapi_trace_pithy
//
size_t xapi_trace_calltree_pithy(struct calltree * const restrict cs, char * const restrict dst, const size_t sz)
  __attribute__((nonnull));

/// xapi_trace_calltree_full
//
// SEE xapi_trace_full
//
size_t xapi_trace_calltree_full(struct calltree * const restrict cs, char * const restrict dst, const size_t sz)
  __attribute__((nonnull));

#undef restrict
#endif
#endif
