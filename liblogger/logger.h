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

/*
 * liblogger-provided program arguments, populated during logger_setup
 */
extern char **	g_argv;					// array of args as delimited by nulls
extern int			g_argc;					// count of g_argv
extern char *		g_argvs;				// single string, nulls replaced with spaces
extern int			g_argvsl;				// length of g_argvs
extern char *		g_binary;				// executable; pointer into g_argv
extern char *		g_interpreter;	// interpreter script, if executing as an interpreter; pointer into g_argv
extern char **	g_logv;					// array of logexprs
extern int			g_logc;					// count of g_logv
extern char *		g_logvs;				// single string of logexprs
extern int			g_logvsl;				// length of g_logvs

/*
 * options and modifiers that can be applied to a log message
 */
#define LOGGER_ATTR_TABLE(x, y)                                                                 \
  LOGGER_ATTR_DEF(NOCOLOR       , 0x00000001 , x , y)  /* (default) not colorized */            \
  LOGGER_ATTR_DEF(RED           , 0x00000002 , x , y)  /* terminal colorization : red */        \
  LOGGER_ATTR_DEF(GREEN         , 0x00000003 , x , y)  /* terminal colorization : green */      \
  LOGGER_ATTR_DEF(YELLOW        , 0x00000004 , x , y)  /* terminal colorization : yellow */     \
  LOGGER_ATTR_DEF(CYAN          , 0x00000005 , x , y)  /* terminal colorization : cyan */       \
  LOGGER_ATTR_DEF(BLUE          , 0x00000006 , x , y)  /* terminal colorization : blue */       \
  LOGGER_ATTR_DEF(CATEGORY      , 0x00000010 , x , y)  /* (default) include the category name*/ \
  LOGGER_ATTR_DEF(CATEGORY_OFF  , 0x00000020 , x , y)                                           \
  LOGGER_ATTR_DEF(TRACE_OFF     , 0x00000040 , x , y)  /* (default) */                          \
  LOGGER_ATTR_DEF(TRACE         , 0x00000080 , x , y)  /* include trace info */                 \
  LOGGER_ATTR_DEF(DISCOVERY_OFF , 0x00000100 , x , y)  /* (default) */                          \
  LOGGER_ATTR_DEF(DISCOVERY     , 0x00000200 , x , y)  /* include discovery info */             \
  LOGGER_ATTR_DEF(TIMESTAMP_OFF , 0x00000400 , x , y)  /* (default) */                          \
  LOGGER_ATTR_DEF(TIMESTAMP     , 0x00000800 , x , y)  /* include timestamp */

// prefix includes the category name and thread id
// timestamp is an additional part of the prefix
//   e.x.
// 12-25-2016 12:12:12 77345467 INFO 
// ^-----------------^
//   \- the timestamp
// ^-------------------------------^
//    \- the prefix

enum {
#define LOGGER_ATTR_DEF(a, b, x, y) L_ ## a = UINT32_C(b),
LOGGER_ATTR_TABLE(0, 0)
#undef LOGGER_ATTR_DEF
};


// all categories
#define L_ALL UINT64_C(0xFFFFFFFFFFFFFFFF)

#define restrict __restrict

/// logger_setup
//
// SUMMARY
//  must be the first liblogger invocation, and invoked exactly once
//
xapi logger_setup();

/// logger_initialize
//
// SUMMARY
//  parse cmdline args, populate g_argv and related variables, calls logger_category_activate
//
// PARAMETERS
//  [envp] - third argument to main
//
xapi logger_initialize(char ** restrict envp);

/// logger_teardown
//
// SUMMARY
//  cleanup
//
void logger_teardown();

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
//  subsequent log* calls append to this log until log_finish is called
//  each log_start call is accompanied by a log_finish call
//
// PARAMETERS
//  ids     - bitwise combination of category ids
//  [attrs] - bitwise combination of L_* options and modifiers
//
xapi logger_log_start(const uint64_t ids, const uint32_t attrs);

#define  log_start(ids) fatal(logger_log_start, ids, 0)
#define log_xstart(...) fatal(logger_log_start, __VA_ARGS__)

/// log_finish
//
// SUMMARY
//  complete the log that was started with log_start
//
xapi logger_log_finish();
#define log_finish() fatal(logger_log_finish)

/// log_would
//
// SUMMARY
//  true if a log with the specified ids would write to any stream
//
int logger_log_would(const uint64_t ids);
#define log_would(ids) logger_log_would(ids)

/// log_bytes
//
// SUMMARY
//  if called after log_start, returns the number of bytes written since log_start
//  otherwise, returns the number of bytes written on the last log* call or log_start/log_finish sequence
//
int logger_log_bytes();

/// log_chars
//
// SUMMARY
//  if called after log_start, returns the number of characters written since log_start (excludes control bytes)
//  otherwise, returns the number of characters written on the last log* call or log_start/log_finish sequence
//
int logger_log_chars();

#undef restrict
#endif
