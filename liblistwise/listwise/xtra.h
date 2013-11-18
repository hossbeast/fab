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
	, int dump
)
	__attribute__((nonnull(1, 5)));

/// lstack_dump
//
// SUMMARY
//  print a list-stack to listwise_info_fd
//
int lstack_dump(lwx * const restrict)
	__attribute__((nonnull));

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

/// listwise_info_fd
//
// SUMMARY
//  liblistwise writes informational messages to this fd
//
// EXAMPLES
//  listwise operators unable to perform some function (ls on a nonexistent path, for example)
//  lstack_dump writes to this fd
//  lstack_exec writes to this fd when debug != 0
//
// DEFAULT
//  2 - stderr
//
extern int listwise_info_fd;

/// listwise_warn_fd
//
// SUMMARY
//  liblistwise writes warning messages to this fd
//
// EXAMPLES
//  liblistwise cannot open a directory for loading operators
//
// DEFAULT
//  2 - stderr
//
extern int listwise_warn_fd;

/// listwise_error_fd
//
// SUMMARY
//  listwise writes errors to this fd before returning nonzero from some API (indicating failure)
//
// EXAMPLES
//  listwise operator has unexpected arguments
//  listwise operator encounters a fatal filesystem error
//  other fatal errors (memory allocation, io, etc)
//
// DEFAULT
//  2 - stderr
//
extern int listwise_error_fd;

/// listwise_errors_unwind
//
// SUMMARY
//  0 - report on site of immediate error only
//  1 - unwind stack when reporting errors
//
// DEFAULT
//  0
//
extern int listwise_errors_unwind;

/// listwise_identity 
//
// SUMMARY
//  pointer to singleton liblistwise-managed lwx object with a single 0-element list
//
extern lwx * listwise_identity;

#undef restrict
#endif
