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

#ifndef _STRSTACK_H
#define _STRSTACK_H

#define restrict __restrict

/// opaque strstack type declaration
//
struct strstack_t;
typedef struct strstack_t strstack;

/// strstack_create
//
// create an empty strstack
//
// returns nonzero on success
//
int strstack_create(strstack ** const restrict stk)
  __attribute__((nonnull(1)));

/// strstack_free
//
// free a strstack with free semantics
//
void strstack_free(strstack * const restrict stk);

/// strstack_xfree
//
// free a strstack with xfree semantics
//
void strstack_xfree(strstack ** const restrict stk)
  __attribute__((nonnull));

/// strstack_undef
//
// remove all entries
//
// returns nonzero on success
//
void strstack_undef(strstack* const restrict stk)
  __attribute__((nonnull));

/// strstack_push
//
// push a string
//
// parameters
//  stk - strstack
//  v   - string
//
// returns nonzero on success
//
int strstack_push(strstack * const restrict stk, const void* const restrict v)
  __attribute__((nonnull));

/// strstack_pop
//
// pop
//
// returns pointer to the stored value, or 0 if not found
//
void strstack_pop(strstack * const restrict stk)
  __attribute__((nonnull));

/// strstack_string
//
// get a string representing stack contents
//
// PARAMETERS
//  stk   - strstack instance
//  [ldr] - prepended to the result string
//  [d]   - delimite stk members in the result string
//  r     - results returned here - points to memory in the strstack instance
//
// returns nonzero on success
//
int strstack_string(strstack * const restrict stk, const char * const restrict ldr, const char * const restrict d, char ** const restrict r)
  __attribute__((nonnull(1, 4)));

#endif

