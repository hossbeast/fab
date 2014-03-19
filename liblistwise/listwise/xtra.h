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

#ifndef _LISTWISE_XTRA_H
#define _LISTWISE_XTRA_H

#include "listwise.h"
#include "listwise/generator.h"

#define restrict __restrict

#if DEBUG || DEVEL || SANITY
struct listwise_logging
{
#if DEBUG
	// lstack dumping and exec progress
	void (*log_dump)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// listwise operators - info
	void (*log_opinfo)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif

#if DEVEL
	// generator parsing - tokens
	void (*log_tokens)(void * udata, const char * func, const char * file, int line, char * fmt, ...);

	// generator parsing - states
	void (*log_states)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif

#if SANITY
	// listwise sanity checks
	void (*log_sanity)(void * udata, const char * func, const char * file, int line, char * fmt, ...);
#endif
} * listwise_logging_config;

void listwise_configure_logging(struct listwise_logging *)
	__attribute__((nonnull));

#endif

/// exec_generator
//
// SUMMARY
//  execute the listwise generator against the lw execution context
//
// PARAMETERS
//  g        - generator
//  [init]   - initial items to write at top of list stack
//  [initls] - lenghts of items in init, 0 for strlen
//  initl    - number of items in init
//  lx       - input/output lw context
//  dump     - [debug] whether to call lstack_dump at each stage of execution
//
// REMARKS
//  if *lx is null, a new lw context is created and returned
//  otherwise, an existing lw context is reused
//
int listwise_exec_generator(
	  const generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
)
	__attribute__((nonnull(1, 5)));

#if DEBUG || SANITY
int listwise_exec_generator2(
	  const generator * const restrict g
	, char ** const restrict init
	, int * const restrict initls
	, const int initl
	, lwx ** restrict lx
	, void * udata
)
	__attribute__((nonnull(1, 5)));
#endif

/// lstack_dump
//
// SUMMARY
//  print a list-stack to stderr
//
int lstack_dump(lwx * const restrict)
	__attribute__((nonnull));

#if DEBUG
/// lstack_dump2
//
// SUMMARY
//  log an list-lstack using the configured logging mechanism
//
int lstack_dump2(lwx * const restrict, void * udata)
	__attribute__((nonnull));
#endif

/// listwise_register_opdir
//
// SUMMARY
//  register an application-specific directory as containing compiled operator .so's
//  in addition to the default locations liblistwise loads these from
//
int listwise_register_opdir(char* dir);

/// lwx_getflags
//
// get application-use flags associated with an lwx
//
uint64_t lwx_getflags(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setflags
//
// set application-use flags associated with an lwx
//
uint64_t lwx_setflags(lwx * const restrict, const uint64_t)
	__attribute__((nonnull));

/// lwx_getptr
//
// get application-use ptr associated with an lwx
//
void * lwx_getptr(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setptr
//
// set application-use ptr associated with an lwx
//
void * lwx_setptr(lwx * const, void * const)
	__attribute__((nonnull(1)));

/// lstack_readrow
//
// SUMMARY
//  read the contents of an lstack row
//
// PARAMETERS
//  lx    - lw context
//  x     - list offset
//  y     - row offset
//  [r]   - return value - ptr
//  [rl]  - return value - length
//  [rt]  - return value - type
//  [obj] - whether object references are resolved
//  [win] - whether windowing is resolved
//  [str] - whether the return value is required to be null-terminated and in tmp space
//  [raw] - indicates whether the raw storage was returned (object entry not resolved, window not resolved, and str was not specified)
//
int lstack_readrow(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl, uint8_t * const restrict rt, int obj, int win, int str, int * const restrict raw)
	__attribute__((nonnull(1)));

#if SANITY
/// listwise_sanity
//
// cause lstack_exec* family of functions to perform sanity checks on ls before
// beginning, after every operator completes, and upon completion
//
// if a sanity check fails, further use of ls will either 1) cause the program to crash
// or 2) memory will be lost
//
// if a sanity check fails, print error(s) to listwise_sanity_fd and call exit(1)
//
// DEFAULT
//  0 - no sanity checks
//
int listwise_sanity;
#endif

/// listwise_identity 
//
// SUMMARY
//  pointer to singleton liblistwise-managed lwx object with a single 0-element list
//
extern lwx * listwise_identity;

#undef restrict
#endif
