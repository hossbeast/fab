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
#include <sys/types.h>

#include "xapi.h"

struct narrator; // libnarrator

/*
 * options and modifiers that can be applied to a log message
 */
#define LOGGER_ATTR_TABLE(x, y)                                                                   \
  LOGGER_ATTR_DEF(NOCOLOR       , 0x00000001 , x , y)  /* (default) not colorized */              \
  LOGGER_ATTR_DEF(RED           , 0x00000002 , x , y)  /* terminal colorization : red */          \
  LOGGER_ATTR_DEF(GREEN         , 0x00000003 , x , y)  /* terminal colorization : green */        \
  LOGGER_ATTR_DEF(YELLOW        , 0x00000004 , x , y)  /* terminal colorization : yellow */       \
  LOGGER_ATTR_DEF(BLUE          , 0x00000005 , x , y)  /* terminal colorization : blue */         \
  LOGGER_ATTR_DEF(MAGENTA       , 0x00000006 , x , y)  /* terminal colorization : magenta */      \
  LOGGER_ATTR_DEF(CYAN          , 0x00000007 , x , y)  /* terminal colorization : cyan */         \
  LOGGER_ATTR_DEF(WHITE         , 0x00000008 , x , y)  /* terminal colorization : cyan */         \
  LOGGER_ATTR_DEF(BOLD_RED      , 0x00000009 , x , y)  /* terminal colorization : bold red */     \
  LOGGER_ATTR_DEF(BOLD_GREEN    , 0x0000000a , x , y)  /* terminal colorization : bold green */   \
  LOGGER_ATTR_DEF(BOLD_YELLOW   , 0x0000000b , x , y)  /* terminal colorization : bold yellow */  \
  LOGGER_ATTR_DEF(BOLD_BLUE     , 0x0000000c , x , y)  /* terminal colorization : bold blue */    \
  LOGGER_ATTR_DEF(BOLD_MAGENTA  , 0x0000000d , x , y)  /* terminal colorization : bold magenta */ \
  LOGGER_ATTR_DEF(BOLD_CYAN     , 0x0000000e , x , y)  /* terminal colorization : bold cyan */    \
  LOGGER_ATTR_DEF(BOLD_WHITE    , 0x0000000f , x , y)  /* terminal colorization : bold cyan */    \
  LOGGER_ATTR_DEF(CATEGORY      , 0x00000010 , x , y)  /* (default) include the category name*/   \
  LOGGER_ATTR_DEF(CATEGORY_OFF  , 0x00000020 , x , y)                                             \
  LOGGER_ATTR_DEF(TRACE_OFF     , 0x00000040 , x , y)  /* (default) */                            \
  LOGGER_ATTR_DEF(TRACE         , 0x00000080 , x , y)  /* include trace info */                   \
  LOGGER_ATTR_DEF(DISCOVERY_OFF , 0x00000100 , x , y)  /* (default) */                            \
  LOGGER_ATTR_DEF(DISCOVERY     , 0x00000200 , x , y)  /* include discovery info */               \
  LOGGER_ATTR_DEF(DATESTAMP_OFF , 0x00000400 , x , y)  /* (default) */                            \
  LOGGER_ATTR_DEF(DATESTAMP     , 0x00000800 , x , y)  /* include timestamp */

enum {
#define LOGGER_ATTR_DEF(a, b, x, y) L_ ## a = UINT32_C(b),
LOGGER_ATTR_TABLE(0, 0)
#undef LOGGER_ATTR_DEF
};


// all categories
#define L_ALL UINT64_C(0xFFFFFFFFFFFFFFFF)

#define restrict __restrict

/// logger_load
//
// SUMMARY
//  initialize the library
//
xapi logger_load();

/// logger_unload
//
// SUMMARY
//  release the library
//
xapi logger_unload();

/// logger_initialize
//
// SUMMARY
//  parse cmdline args, populate g_argv and related variables, calls logger_category_activate
//
// PARAMETERS
//  [envp] - third argument to main
//
xapi logger_initialize(char ** restrict envp);

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
xapi logger_logvf(const uint64_t ids, const uint32_t attrs, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(3)));
xapi logger_logf (const uint64_t ids, const uint32_t attrs, const char * const restrict fmt, ...)
  __attribute__((nonnull(3)));
xapi logger_logs (const uint64_t ids, const uint32_t attrs, const char * const restrict s)
  __attribute__((nonnull));
xapi logger_logw (const uint64_t ids, const uint32_t attrs, const char * const restrict src, size_t len)
  __attribute__((nonnull));
xapi logger_logc (const uint64_t ids, const uint32_t attrs, const char c)
  __attribute__((nonnull));

#define  logvf(ids, ...) fatal(logger_logvf , ids , 0 , __VA_ARGS__)
#define   logf(ids, ...) fatal(logger_logf  , ids , 0 , __VA_ARGS__)
#define   logs(ids, ...) fatal(logger_logs  , ids , 0 , __VA_ARGS__)
#define   logw(ids, ...) fatal(logger_logw  , ids , 0 , __VA_ARGS__)
#define xlogvf(...) fatal(logger_logvf , __VA_ARGS__)
#define  xlogf(...) fatal(logger_logf  , __VA_ARGS__)
#define  xlogs(...) fatal(logger_logs  , __VA_ARGS__)
#define  xlogw(...) fatal(logger_logw  , __VA_ARGS__)

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
//  token    - if log_would, and if the call succeeds, set to 1
//
xapi log_xstart(const uint64_t ids, const uint32_t attrs, int * const restrict token);

xapi log_start(const uint64_t ids, int * const restrict token);

/// log_finish
//
// SUMMARY
//  complete the log that was started with log_start
//
// PARAMETERS
//  [token] - if *token, if if the call succeeds, set to 0
//
xapi log_finish(int * const restrict token);

/// log_narrator
//
// SUMMARY
//  between log_start and log_finish calls, returns a narrator which may be used to append to the
//  current log message
//
struct narrator * log_narrator();

/// log_would
//
// SUMMARY
//  true if a log with the specified ids would write to any stream
//
int log_would(const uint64_t ids);

/// log_bytes
//
// SUMMARY
//  if called after log_start, returns the number of bytes written since log_start
//  otherwise, returns the number of bytes written on the last log* call or log_start/log_finish sequence
//
int log_bytes();

/// log_chars
//
// SUMMARY
//  if called after log_start, returns the number of characters written since log_start (excludes control bytes)
//  otherwise, returns the number of characters written on the last log* call or log_start/log_finish sequence
//
int log_chars();

#undef restrict
#endif
