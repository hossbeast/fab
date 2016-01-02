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
#include <stdint.h>
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
 * options that can be applied to a log message
 */
#define LOGGER_BITS_TABLE(x)                                \
  LOGGER_BITS_DEF(CLEAR         , 0x0001000000000000  , x)  \
  LOGGER_BITS_DEF(RED           , 0x0011000000000000  , x)  \
  LOGGER_BITS_DEF(GREEN         , 0x0021000000000000  , x)  \
  LOGGER_BITS_DEF(YELLOW        , 0x0031000000000000  , x)  \
  LOGGER_BITS_DEF(CYAN          , 0x0041000000000000  , x)  \
  LOGGER_BITS_DEF(BLUE          , 0x0051000000000000  , x)  \
  LOGGER_BITS_DEF(PREFIX        , 0x0010000000000000  , x)  \
  LOGGER_BITS_DEF(PREFIX_OFF    , 0x0030000000000000  , x)  \
  LOGGER_BITS_DEF(TRACE_OFF     , 0x0040000000000000  , x)  \
  LOGGER_BITS_DEF(TRACE         , 0x00b0000000000000  , x)  \
  LOGGER_BITS_DEF(DISCOVERY_OFF , 0x0000000000000000  , x)  \
  LOGGER_BITS_DEF(DISCOVERY     , 0x0040000000000000  , x)  \

enum {
#define LOGGER_BITS_DEF(a, b, x) L_ ## a = UINT64_C(b),
LOGGER_BITS_TABLE(0)
#undef LOGGER_BITS_DEF
};

LOGGER_PREFIX_ENABLE
LOGGER_PREFIX_DISABLE

LOGGER_TRACE_ENABLE
LOGGER_TRACE_DISABLE

LOGGER_DISCOVERY_ENABLE
LOGGER_DISCOVERY_DISABLE

#define restrict __restrict

/// logger_setup
//
// SUMMARY
//  parse cmdline args, populate g_argv and related variables, call logger_register_resolve
//
// REMARKS
//  should be called after logger_register
//
// PARAMETERS
//  argc - first argument to main
//  argv - second argument to main
//  envp - third argument to main
//
xapi logger_setup(int argc, const char ** argv, const char ** restrict envp);

/// logger_teardown
//
// SUMMARY
//  cleanup
//
void logger_teardown();

/// log
//
// SUMMARY
//  issue a logging request
//
// REMARKS
//  bits is the bitwise combination of 1) logger category ids, and 2) colorization constants
//
// PARAMETERS
//  [func]  - function name
//  [file]  - file name
//  [line]  - line number
//  [attrs] - bitwise combination of L_* options and modifiers
//  ids     - bitwise combination of category ids
//  [fmt]   - format string
//  [src]   - source bytes
//  [len]   - count of bytes from src (0 != strlen)
//  [s]     - string to write
//
#if ! (DEBUG || DEVEL)
xapi logger_vlogf(const uint32_t attrs, const uint64_t ids, const char * const restrict fmt, va_list va)
	__attribute__((nonnull(2)));
xapi logger_logf (const uint32_t attrs, const uint64_t ids, const char * const restrict fmt, ...)
	__attribute__((nonnull(2)));
xapi logger_logs (const uint32_t attrs, const uint64_t ids, const char * const restrict s)
	__attribute__((nonnull));
xapi logger_logw (const uint32_t attrs, const uint64_t ids, const char * const restrict src, size_t len)
	__attribute__((nonnull));

# define vlogf(...) logger_vlogf(0, __VA_ARGS__)
# define  logf(...) logger_logf(0, __VA_ARGS__)
# define  logs(...) logger_logs(0, __VA_ARGS__)
# define  logw(...) logger_logw(0, __VA_ARGS__)

# define vxlogf(...) logger_vlogf(__VA_ARGS__)
# define  xlogf(...) logger_logf(__VA_ARGS__)
# define  xlogs(...) logger_logs(__VA_ARGS__)
# define  xlogw(...) logger_logw(__VA_ARGS__)

#else
xapi logger_vlogf(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, va_list va)
	__attribute__((nonnull(1,2,5)));
xapi logger_logf(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, ...)
	__attribute__((nonnull(1,2,5)));
xapi logger_logs(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict s)
	__attribute__((nonnull));
xapi logger_logw(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict src, size_t len)
	__attribute__((nonnull));

# define vlogf(...) logger_vlogf(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  logf(...) logger_logf(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  logs(...) logger_logs(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  logw(...) logger_logw(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// log_start
//
// SUMMARY
//  begin logging a message if log_would([bits])
//  subsequent log* calls append to this log until log_finish is called
//  each log_start call is accompanied by a log_finish call
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  bits   - log bits
//  fmt    - format string
//
#if ! (DEBUG || DEVEL)
xapi logger_log_start(const uint64_t bits);
# define log_start(bits) logger_log_start(bits)
#else
xapi logger_log_start(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits)
	__attribute__((nonnull(2)));
# define log_start(bits) logger_log_start(__FUNCTION__, __FILE__, __LINE__, bits)
#endif

/// log_finish
//
// SUMMARY
//  complete the log that was started with log_start
//
xapi logger_log_finish();
#define log_finish() logger_log_finish()

/// log_would
//
// SUMMARY
//  true if logs would print with the specified bits
//
int logger_log_would(const uint64_t bits);
#define log_would(bits) logger_log_would(bits)

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
