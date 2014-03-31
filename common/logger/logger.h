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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

#define restrict __restrict

// C
#define L_COLOR_VALUE   0xF000000000000000ULL // C range

#define L_RED           0x1000000000000000ULL // this message red in the terminal
#define L_GREEN         0x2000000000000000ULL // this message green in the terminal
#define L_YELLOW        0x3000000000000000ULL // this message yellow in the terminal
#define L_CYAN          0x4000000000000000ULL // this message cyan in the terminal
#define L_BLUE          0x5000000000000000ULL // this message blue in the terminal

#define L_TAG           0x0000FFFFFFFFFFFFULL // D range

extern struct g_logs_t {
	uint64_t		v;	// tag definition
	char *			s;	// name
	int					l;	// length of name
	char *			d;	// description
} * g_logs;

/// 
// application-provided logs
//
extern int g_logs_l;

///
// logger-provided program args
//
extern char ** g_argv;	// array of args as delimited by nulls
extern int g_argc;			// count of g_argv
extern char * g_argvs;	// single string, nulls replaced with spaces
extern int g_argvsl;		// length of g_argvs

/// (XAPI) log_init
//
// SUMMARY
//  initialize logging and parse cmdline args
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  [bits] - bits to use when logging the description
//
int log_init();
#if DEBUG
int log_log_init_and_describe(const char * const restrict func, const char * const restrict file, int line, uint64_t bits)
	__attribute__((nonnull));

# define log_init_and_describe(...) log_log_init_and_describe(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
int log_log_init_and_describe(uint64_t bits);

# define log_init_and_describe(...) log_log_init_and_describe(__VA_ARGS__)
#endif

/// log_config
// 
// SUMMARY
//  configure logging options
//
// PARAMETERS
//  [prefix] - matching logs will emit preceeding prefix
//  [trace]  - matching logs will emit trailing trace
//
#if DEBUG
void log_config(uint64_t prefix, uint64_t trace);
#else
void log_config(uint64_t trace);
#endif

// (XAPI) log_parse
//
// SUMMARY
//  parse the logging directive to enable/disable tags
//
// REMARKS
//  log_parse MAY be called BEFORE log_init (to give cmd-line switches precedence)
//
// PARAMETERS
//  [func]     - function name
//  [file]     - file name
//  [line]     - line number
//  args       - directive text
//  [args_len] - length of args, or 0 for strlen
//  [bits]     - bits to use when logging the description
//
int log_parse(char * args, int args_len)
	__attribute__((nonnull(1)));

#if DEBUG
int log_log_parse_and_describe(const char * const restrict func, const char * const restrict file, int line, char * args, int args_len, uint64_t bits)
	__attribute__((nonnull(1, 2, 4)));

#define log_parse_and_describe(...) log_log_parse_and_describe(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
int log_log_parse_and_describe(char * args, int args_len, uint64_t bits)
	__attribute__((nonnull(1)));

#define log_parse_and_describe(...) log_log_parse_and_describe(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// log_would
//
// SUMMARY
//  true if logs would print with the specified bits
//
int log_would(const uint64_t bits);

/// log
//
// SUMMARY
//  log messages if log_would([bits])
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  bits   - log bits
//  [fmt]  - format string
//  [src]  - source bytes
//  [len]  - count of bytes from src (0 != strlen)
//  [s]    - string to write
//
// RETURNS
//  if called after log_start, returns 1
//  otherwise, returns log_would(bits)
// 
#ifndef DEBUG
int log_vlogf(const uint64_t bits, const char * const restrict fmt, va_list va)
	__attribute__((nonnull(2)));
int log_logf(const uint64_t bits, const char * const restrict fmt, ...)
	__attribute__((nonnull(2)));
int log_logs(const uint64_t bits, const char * const restrict s)
	__attribute__((nonnull));
int log_log(const uint64_t bits, const char * const restrict src, size_t len)
	__attribute__((nonnull));

# define vlogf(...) log_vlogf(__VA_ARGS__)
# define  logf(...) log_logf(__VA_ARGS__)
# define  logs(...) log_logs(__VA_ARGS__)
# define   log(...) log_log(__VA_ARGS__)
#else
int log_vlogf(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, va_list va)
	__attribute__((nonnull(1,2,5)));
int log_logf(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, ...)
	__attribute__((nonnull(1,2,5)));
int log_logs(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict s)
	__attribute__((nonnull));
int log_log(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict src, size_t len)
	__attribute__((nonnull));

# define vlogf(...) log_vlogf(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  logf(...) log_logf(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  logs(...) log_logs(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define   log(...) log_log(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// logstart
//
// SUMMARY
//  begin logging a message if log_would([bits])
//  subsequent log* calls append to this log until logfinish is called
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  bits   - log bits
//  fmt    - format string
//
// RETURNS
//  log_would(bits)
//
#ifndef DEBUG
int log_log_start(const uint64_t bits);
# define log_start(bits) log_log_start(bits)
#else
int log_log_start(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits)
	__attribute__((nonnull(2)));
# define log_start(bits) log_log_start(__FUNCTION__, __FILE__, __LINE__, bits)
#endif

/// log_finish
//
// SUMMARY
//  complete the log that was started with logstart
//
void log_finish();

/// logged_bytes
//
// SUMMARY
//  if called after log_start, returns the number of bytes written since log_start
//  otherwise, returns the number of bytes written on the last log* call or log_start/log_finish sequence
//
int logged_bytes();

/// logged_chars
//
// SUMMARY
//  if called after log_start, returns the number of characters written since log_start (excludes control bytes)
//  otherwise, returns the number of characters written on the last log* call or log_start/log_finish sequence
//
int logged_characters();

/// log_teardown
//
// cleanup
//
void log_teardown();

#undef restrict
#endif
