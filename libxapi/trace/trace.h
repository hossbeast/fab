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

// options and modifiers for rendering traces
#define XAPI_TRACE_TABLE                                                                                         \
  DEF(XAPI_TRACE_NOCOLORIZE     , 0x0001) /* (default) no terminal color escapes                              */ \
  DEF(XAPI_TRACE_COLORIZE       , 0x0002) /* apply terminal color escape sequences                            */ \
  DEF(XAPI_TRACE_NEWLINE        , 0x0004) /* (default) include a trailing newline                             */ \
  DEF(XAPI_TRACE_NONEWLINE      , 0x000C) /* omit the trailing newline                                        */ \
  DEF(XAPI_TRACE_INFOKEYED      , 0x0010) /* (default) include only infos whose name is unique in the stack   */ \
  DEF(XAPI_TRACE_INFONAMED      , 0x0030) /* include all infos in the stack                                   */ \
  DEF(XAPI_TRACE_INFOSTACK      , 0x0080) /* (default) report infos for a stack on the summary line           */ \
  DEF(XAPI_TRACE_INFOFRAME      , 0x000C) /* report infos alongside the frame they are attached to            */ \

enum {
#undef DEF
#define DEF(a, b) a = UINT16_C(b),
XAPI_TRACE_TABLE
};

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
void xapi_pithytrace(int fd, uint16_t attrs);

/// xapi_fulltrace
/// xapi_backtrace
//
// SUMMARY
//  call xapi_trace_full and write the output to stderr
//
// PARAMETERS
//  [to] - file descriptor to write to
//
void xapi_fulltrace(int fd, uint16_t attrs);
void xapi_backtrace(int fd, uint16_t attrs);

/// xapi_trace_pithy
//
// SUMMARY
//  write a string summarizing the entire calltree
//
// PARAMETERS
//  dst   - buffer to write to
//  sz    - max bytes to write, including null byte
//  attrs - bitwise mask of XAPI_TRACE_*
//
size_t xapi_trace_pithy(char * const restrict dst, const size_t sz, uint16_t attrs)
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
//  dst   - buffer to write to
//  sz    - max bytes to write, including null byte
//  attrs - bitwise mask of XAPI_TRACE_*
//
size_t xapi_trace_full(char * const restrict dst, const size_t sz, uint16_t attrs)
  __attribute__((nonnull));

/// xapi_trace_calltree_pithy
//
// SEE xapi_trace_pithy
//
size_t xapi_trace_calltree_pithy(struct calltree * const restrict cs, char * const restrict dst, const size_t sz, uint16_t attrs)
  __attribute__((nonnull));

/// xapi_trace_calltree_full
//
// SEE xapi_trace_full
//
size_t xapi_trace_calltree_full(struct calltree * const restrict cs, char * const restrict dst, const size_t sz, uint16_t attrs)
  __attribute__((nonnull));

/// xapi_trace_info
//
// SUMMARY
//  get the value of an info kvp while unwinding
//
// PARAMETERS
//  name  - info name
//  [dst] - buffer to write the value to
//  [sz]  - size of dst
//
// RETURNS
//  number of bytes < sz written to dst, not including the null terminator, which is always written
//
size_t xapi_trace_info(const char * restrict name, char * const restrict dst, const size_t sz)
  __attribute__((nonnull(1)));

#endif
#endif
