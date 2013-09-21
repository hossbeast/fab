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

#ifndef _LISTWISE_H
#define _LISTWISE_H

#include <stdint.h>

#define restrict __restrict

/// list-stack
//
// stack of lists of rows
//
struct lstack;
typedef struct lstack
{
  struct // stack
  {
    struct // list
    {
      char*   s;    // string

      int     l;    // len - number of characters
      int     a;    // alloc

      uint8_t type; // object type - see listwise/object.h interface
    }         *s;

    int     l;  // len - number of rows
    int     a;  // alloc
  }         *s;

  int     l;  // len - number of lists
  int     a;  // alloc

  uint64_t  flags;  // available for application-use
  void *    ptr;    // available for application-use
} lstack;

// listwise context, an opaque type
struct lwx_t;
typedef struct lwx_t lwx;

/// listwise_exec
//
// SUMMARY
//  execute the listwise generator, receive an lstack result
//
// PARAMETERS
//  s        - generator string
//  l        - s length, or 0 for strlen
//  [init]   - rows to write to the stack before executing
//  [initls] - lengths for rows in init, 0 for strlen
//  initl    - number of rows in init
//  [lx]     - listwise execution context
//  ls       - input/output lstack
//
// RETURNS
//  0 for success
//
// REMARKS
//  if lx is null, an internal context is used
//  otherwise, if *lx is null, a new context is created and returned
//  otherwise, an existing context is reused
//
//  if *ls is null, a new lstack is created and returned
//  otherwise, an existing lstack is reused
//
int listwise_exec(
	  const char * const restrict s
	, int l
	, const char ** const restrict init
	, const int * const restrict initls
	, const int initl
	, const lwx ** const restrict lx
	, const lstack ** const restrict ls
	, int dump
)
  __attribute__((nonnull(1, 7)));

/// lwx_alloc
//
// SUMMARY
//  allocate an lwx
//
int lwx_alloc(lwx ** const restrict lx)
  __attribute__((nonnull));

/// lwx_free
//
// SUMMARY
//  free an lwx with free semantics
//
void lwx_free(lwx * const restrict lx);

/// lwx_xfree
//
// SUMMARY
//  free an lwx with xfree semantics
//
void lwx_xfree(lwx ** const restrict lx)
  __attribute__((nonnull));

/// lstack_create
//
// SUMMARY
//  allocate an lstack
//
int lstack_create(lstack ** const restrict)
  __attribute__((nonnull));

/// lstack_free
//
// SUMMARY
//  free lstack with free-like semantics
//
void lstack_free(lstack * const restrict);

/// lstack_xfree
//
// SUMMARY
//  free an lstack with xfree-like semantics
//
void lstack_xfree(lstack ** const restrict)
  __attribute__((nonnull));

/// lstack_deepcopy
//
// SUMMARY
//  create a deep copy of an lstack
//
// PARAMETERS
//  dst - copy returned here
//  src - source lstack
//
// RETURNS
//  0 on failure (allocation) and 1 otherwise
//
// NOTES
//  selection and windows are reset in the copy
//
int lstack_deepcopy(lstack ** const restrict, lstack * const restrict)
  __attribute__((nonnull));

/// listwise_err_fd
//
// SUMMARY
//  listwise operators write errors to this fd (ls a nonexistent path, for example)
//
// DEFAULT
//  2 - stderr
//
extern int listwise_err_fd;

/// listwise_identity 
//
// SUMMARY
//  pointer to singleton liblistwise-managed lstack object with a single 0-element list
//
extern lstack * listwise_identity;

#undef restrict
#endif
