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

#ifndef _LOG_H
#define _LOG_H

#include <stdarg.h>
#include <stdint.h>

// C
#define L_COLOR_VALUE   0xF000000000000000ULL // C range

#define L_RED           0x1000000000000000ULL // this message red in the terminal
#define L_GREEN         0x2000000000000000ULL // this message green in the terminal
#define L_YELLOW        0x3000000000000000ULL // this message yellow in the terminal
#define L_CYAN          0x4000000000000000ULL // this message cyan in the terminal
#define L_BLUE          0x5000000000000000ULL // this message blue in the terminal

#define L_TAG           0x0000FFFFFFFFFFFFULL // D range

// E
#define L_ERROR         0x0000000000000001ULL | L_RED		
#define L_WARN          0x0000000000000002ULL | L_YELLOW
#define L_INFO          0x0000000000000004ULL
#define L_ARGS 					0x0000000000000008ULL
#define L_PARAMS				0x0000000000000010ULL
#if DBUG
# define L_FFTOKN 			0x0000000000000020ULL
# define L_FFSTAT 			0x0000000000000040ULL
# define L_FFTREE				0x0000000000000080ULL
# define L_FFFILE				0x0000000000000100ULL
# define L_FF						0x0000000000000200ULL
#else
# define L_FFTOKN 			0
# define L_FFSTAT 			0
# define L_FFTREE				0
# define L_FFFILE				0
# define L_FF						0
#endif
#define L_BPINFO				0x0000000000000400ULL
#define L_BPEXEC				0x0000000000000800ULL
#define L_BPEVAL				0x0000000000001000ULL
#define L_BPDUMP				0x0000000000002000ULL
#define L_BP						0x0000000000004000ULL
#define L_FMLEXEC				0x0000000000008000ULL
#if DBUG
# define L_FMLTARG			0x0000000000010000ULL
#else
# define L_FMLTARG			0
#endif
#define L_FML						0x0000000000020000ULL
#define L_FAB						0x0000000000040000ULL
#define L_DSCINFO				0x0000000000080000ULL
#define L_DSCEXEC				0x0000000000100000ULL
#define L_DSCNEW				0x0000000000200000ULL
#define L_DSC						0x0000000000400000ULL
#if DBUG
# define L_DGDEPS				0x0000000000800000ULL
#else
# define L_DGDEPS				0
#endif
#define L_DGRAPH 				0x0000000001000000ULL
#define L_DG 						0x0000000002000000ULL
#define L_VARAUTO				0x0000000004000000ULL
#define L_VARUSER				0x0000000008000000ULL
#define L_VAR						0x0000000010000000ULL
#define L_INVOKE				0x0000000020000000ULL
#define L_CHANGE				0x0000000040000000ULL
#define L_CHANGEL				0x0000000080000000ULL
#define L_LWDEBUG				0x0000000100000000ULL

struct g_logs_t {
	uint64_t		v;	// tag definition
	char *			s;	// name
	int					l;	// length of name
	char *			d;	// description
} * g_logs;

int g_logs_l;

/// log_init
//
// initialize logging, parse cmdline args as well as {args}
//
int log_init(char * args);

// log_parse
//
// parse the log string to enable/disable tags
//
void log_parse(char * args, int args_len);

/// log_active
//
// write a string describing active logging categories to s
//
void log_active(char* s, size_t l);

/// log_would
//
// true if logs would print with the specified bits
//
int log_would(const uint64_t bits);

/// log
//
// writes messages to stderr if log_would([bits])
//
int log(const uint64_t bits, const char* fmt, ...)
	__attribute__((nonnull(2)));

/// log_start
//
// begin writing a single log
//
int log_start(const uint64_t bits, const char* fmt, ...)
	__attribute__((nonnull(2)));

/// log_add
//
// append to the log under construction
//
int log_add(const char* fmt, ...)
	__attribute__((nonnull(1)));

/// log_finish
//
// complete the log begun with log_start
//
int log_finish(const char* fmt, ...);

/// log_written
//
// returns the number of bytes written thus far for a log_start/log_add* sequence
//
int log_written();

/// log_teardown
//
// cleanup
//
void log_teardown();

#endif

