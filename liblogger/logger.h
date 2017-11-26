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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdarg.h>
#include <inttypes.h>

#include "xapi.h"
#include "types.h"

struct narrator; // libnarrator

/*

options and modifiers which may be applied to
 categories (see logger_category)
 log messages (see xlog*)
 streams (see logger_stream)

*/
#define LOGGER_ATTR_TABLE                                                                 \
  LOGGER_ATTR_DEF(NOCOLOR       , 0x00000001)  /* (default) not colorized */              \
  LOGGER_ATTR_DEF(RED           , 0x00000002)  /* terminal colorization : red */          \
  LOGGER_ATTR_DEF(GREEN         , 0x00000003)  /* terminal colorization : green */        \
  LOGGER_ATTR_DEF(YELLOW        , 0x00000004)  /* terminal colorization : yellow */       \
  LOGGER_ATTR_DEF(BLUE          , 0x00000005)  /* terminal colorization : blue */         \
  LOGGER_ATTR_DEF(MAGENTA       , 0x00000006)  /* terminal colorization : magenta */      \
  LOGGER_ATTR_DEF(CYAN          , 0x00000007)  /* terminal colorization : cyan */         \
  LOGGER_ATTR_DEF(GRAY          , 0x00000008)  /* terminal colorization : gray */         \
  LOGGER_ATTR_DEF(BOLD_RED      , 0x00000009)  /* terminal colorization : bold red */     \
  LOGGER_ATTR_DEF(BOLD_GREEN    , 0x0000000a)  /* terminal colorization : bold green */   \
  LOGGER_ATTR_DEF(BOLD_YELLOW   , 0x0000000b)  /* terminal colorization : bold yellow */  \
  LOGGER_ATTR_DEF(BOLD_BLUE     , 0x0000000c)  /* terminal colorization : bold blue */    \
  LOGGER_ATTR_DEF(BOLD_MAGENTA  , 0x0000000d)  /* terminal colorization : bold magenta */ \
  LOGGER_ATTR_DEF(BOLD_CYAN     , 0x0000000e)  /* terminal colorization : bold cyan */    \
  LOGGER_ATTR_DEF(BOLD_GRAY     , 0x0000000f)  /* terminal colorization : bold gray */    \
  LOGGER_ATTR_DEF(CATEGORY      , 0x00000010)  /* (default) include the category name */  \
  LOGGER_ATTR_DEF(NOCATEGORY    , 0x00000020)                                             \
  LOGGER_ATTR_DEF(NOTRACE       , 0x00000040)  /* (default) */                            \
  LOGGER_ATTR_DEF(TRACE         , 0x00000080)  /* append trace info */                    \
  LOGGER_ATTR_DEF(NODISCOVERY   , 0x00000100)  /* (default) */                            \
  LOGGER_ATTR_DEF(DISCOVERY     , 0x00000200)  /* append discovery info */                \
  LOGGER_ATTR_DEF(NODATESTAMP   , 0x00000400)  /* (default) */                            \
  LOGGER_ATTR_DEF(DATESTAMP     , 0x00000800)  /* parepend timestamp */                   \
  LOGGER_ATTR_DEF(NONAMES       , 0x00001000)  /* (default) */                            \
  LOGGER_ATTR_DEF(NAMES         , 0x00002000)  /* prepend process/thread names  */        \
  LOGGER_ATTR_DEF(FILTER        , 0x00004000)  /* (default) emission subject to filters */\
  LOGGER_ATTR_DEF(NOFILTER      , 0x00008000)  /* dont filter logs */                     \

enum {
#define LOGGER_ATTR_DEF(a, b) L_ ## a = UINT32_C(b),
LOGGER_ATTR_TABLE
#undef LOGGER_ATTR_DEF
};

/// logger_arguments_setup
//
// SUMMARY
//  parse cmdline args, populate g_argv and related variables
//
// PARAMETERS
//  [envp] - third argument to main
//
xapi logger_arguments_setup(char ** restrict envp);

/// logger_finalize
//
// SUMMARY
//  1. activate registered categories and streams
//  2. reset filters for all streams, and reapply filters from stream definitions and cmdline arguments
//  3. log effective logger configuration
//
xapi logger_finalize(void);

/// log
//
// SUMMARY
//  issue a logging request with fatal
//
// PARAMETERS
//  ids     - bitwise combination of category ids
//  [attrs] - bitwise combination of L_* options and modifiers
//
// VARIANT PARAMETERS
//  [fmt]   - format string
//  [src]   - source bytes
//  [len]   - count of bytes from src (0 != strlen)
//  [s]     - string
//  [c]     - character
//
// REMARKS
//  under DEBUG / DEVEL, these extra parameters are passed to enable L_TRACE
//  [func]  - function name
//  [file]  - file name
//  [line]  - line number
//
xapi logger_vlogf(uint64_t ids, uint32_t attrs, const char * restrict fmt, va_list va)
  __attribute__((nonnull(3)));
xapi logger_logf (uint64_t ids, uint32_t attrs, const char * restrict fmt, ...)
  __attribute__((nonnull(3)));
xapi logger_logs (uint64_t ids, uint32_t attrs, const char * restrict s)
  __attribute__((nonnull));
xapi logger_logw (uint64_t ids, uint32_t attrs, const char * restrict src, size_t len)
  __attribute__((nonnull));
xapi logger_logc (uint64_t ids, uint32_t attrs, char c)
  __attribute__((nonnull));

#define  logvf(ids, ...)  fatal(logger_logvf , ids , 0 , __VA_ARGS__)
#define   logf(ids, ...)  fatal(logger_logf  , ids , 0 , __VA_ARGS__)
#define   logs(ids, ...)  fatal(logger_logs  , ids , 0 , __VA_ARGS__)
#define   logw(ids, ...)  fatal(logger_logw  , ids , 0 , __VA_ARGS__)
#define xlogvf(...)       fatal(logger_logvf , __VA_ARGS__)
#define  xlogf(...)       fatal(logger_logf  , __VA_ARGS__)
#define  xlogs(...)       fatal(logger_logs  , __VA_ARGS__)
#define  xlogw(...)       fatal(logger_logw  , __VA_ARGS__)

/// log_start
//
// SUMMARY
//  begin logging a message if log_would([bits])
//  subsequent log* calls append to this log message until log_finish is called
//  each logger_start call is accompanied by a logger_finish call
//
// PARAMETERS
//  ids     - bitwise combination of category ids
//  [attrs] - bitwise combination of L_* options and modifiers
//  [N]     - (returns) narrator to write the log message to
//
xapi log_xstart(uint64_t ids, uint32_t attrs, struct narrator ** restrict N);

xapi log_start(uint64_t ids, struct narrator ** restrict N);

/// log_finish
//
// SUMMARY
//  flush the log that was started with log_start
//
xapi log_finish(void);

/// log_would
//
// SUMMARY
//  returns true if a log with the specified ids would write to any stream
//
int log_would(uint64_t ids);
int log_xwould(uint64_t ids, uint32_t attrs);

/// log_bytes
//
// SUMMARY
//  if called after log_start, returns the number of bytes written since log_start
//  otherwise, returns the number of bytes written on the last log* call or log_start/log_finish sequence
//
int log_bytes(void);

/// log_chars
//
// SUMMARY
//  if called after log_start, returns the number of characters written since log_start (excludes control bytes)
//  otherwise, returns the number of characters written on the last log* call or log_start/log_finish sequence
//
int log_chars(void);

#if XAPI_STACKTRACE
/// logger_trace
//
// SUMMARY
//  while unwinding, log the backtrace using xapi_trace
//
// PARAMETERS
//  ids           - bitwise combination of category ids
//  [attrs]       - bitwise combination of L_* options and modifiers
//  [trace_attrs] - attrs for xapi_trace
//
xapi logger_trace_full(uint64_t ids, uint16_t trace_attrs);
xapi logger_xtrace_full(uint64_t ids, uint32_t attrs, uint16_t trace_attrs);
xapi logger_trace_pithy(uint64_t ids, uint16_t trace_attrs);
xapi logger_xtrace_pithy(uint64_t ids, uint32_t attrs, uint16_t trace_attrs);
#endif

#endif
