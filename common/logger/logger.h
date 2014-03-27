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

/// log_init
//
// SUMMARY
//  initialize logging, parse cmdline args as well as {args}
//
// PARAMETERS
//  trace  - matching logs will emit trace info
//  [bits] - bits to use when logging the description
//
int log_init(uint64_t trace)
	__attribute__((nonnull));

int log_init_and_describe(uint64_t trace, uint64_t bits)
	__attribute__((nonnull));

// log_parse
//
// SUMMARY
//  parse the logging directive to enable/disable tags
//
// REMARKS
//  log_parse MAY be called BEFORE log_init (to give cmd-line switches precedence)
//
// PARAMETERS
//  args     - directive text
//  args_len - length of args
//  [bits]   - bits to use when logging the description
//
int log_parse(char * args, int args_len)
	__attribute__((nonnull(1)));

int log_parse_and_describe(char * args, int args_len, uint64_t bits)
	__attribute__((nonnull(1)));

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
//  fmt    - format string
//
// RETURNS
//  number of visible characters written (excludes colorizing control bytes)
// 
#ifndef DEBUG
int vlogi(const uint64_t bits, const char * const restrict fmt, va_list va) __attribute__((nonnull(2)));
int  logi(const uint64_t bits, const char * const restrict fmt, ...)        __attribute__((nonnull(2)));

# define vlog(...) vlogi(__VA_ARGS__)
# define  log(...)  logi(__VA_ARGS__)
#else
int vlogi(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, va_list va) __attribute__((nonnull(1,2,5)));
int  logi(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict fmt, ...)        __attribute__((nonnull(1,2,5)));

# define vlog(...) vlogi(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  log(...)  logi(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// log_start
//
// SUMMARY
//  begin logging a message if log_would([bits])
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  bits   - log bits
//  fmt    - format string
//
// RETURNS
//  number of visible characters written (excludes colorizing control bytes)
//
#ifndef DEBUG
int vlog_starti(const uint64_t bits, const char* fmt, va_list va) __attribute__((nonnull(2)));
int  log_starti(const uint64_t bits, const char* fmt, ...)        __attribute__((nonnull(2)));

# define vlog_start(...) vlog_starti(__VA_ARGS__)
# define  log_start(...)  log_starti(__VA_ARGS__)
#else
int vlog_starti(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char* fmt, va_list va) __attribute__((nonnull(2)));
int  log_starti(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char* fmt, ...)        __attribute__((nonnull(2)));

# define vlog_start(...) vlog_starti(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
# define  log_start(...)  log_starti(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// log_add
//
// SUMMARY
//  append to the log that was started with log_start
//
// RETURNS
//  number of visible characters written (excludes colorizing control bytes)
//
int vlog_add(const char * fmt, va_list va) __attribute__((nonnull));
int  log_add(const char * fmt, ...)	       __attribute__((nonnull(1)));

/// log_finish
//
// SUMMARY
//  complete the log that was started with log_start
//
// RETURNS
//  number of visible characters written (excludes colorizing control bytes)
//
int vlog_finish(const char * fmt, va_list va);
int  log_finish(const char * fmt, ...);

/// log_write
//
// SUMMARY
//  write to log if log_would([bits])
//
// PARAMETERS
//  [func] - function name
//  [file] - file name
//  [line] - line number
//  bits   - log bits
//  src    - source buffer
//  len    - byte count
//
#ifndef DEBUG
void log_writei(const uint64_t bits, const char * const restrict src, size_t len) __attribute__((nonnull(2)));

# define log_write(...)  log_writei(__VA_ARGS__)
#else
void log_writei(const char * const restrict func, const char * const restrict file, int line, const uint64_t bits, const char * const restrict src, size_t len) __attribute__((nonnull(1,2,5)));

# define log_write(...)  log_writei(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#endif

/// logged_bytes
//
// SUMMARY
//  returns the number of bytes written thus far for a log_start/log_add* sequence
//
int logged_bytes();

/// logged_chars
//
// SUMMARY
//  returns the number of visible characters thus far for a log_start/log_add* sequence (excludes colorizing control bytes)
//
int logged_chars();

/// log_teardown
//
// cleanup
//
void log_teardown();

#undef restrict
#endif
