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

#ifndef _XAPI_STACK_INTERNAL_H
#define _XAPI_STACK_INTERNAL_H

#include <sys/types.h>
#include <stdint.h>

struct stack;     // has a set of frames and an error
struct frame;     // has a set of stacks
struct memblk;

/// stack
//
// SUMMARY
//  a single thread of execution
//
typedef struct stack
{
	const struct etable *	etab;		// error table
	xapi_code             code;		// error code

	char *	msg;	  	            // error message
  size_t  msgl;
  size_t  msga;

  struct
  {
    struct frame *  v;      // frames in the stack
    size_t          l;
    size_t          a;
  } frames;

  struct stack *  parent;   // parent execution stack
} stack;

// per-thread stack storage
extern __thread struct stacks
{
  stack *   v;
  size_t    l;
  size_t    a;
} stacks;

// per-thread execution stack
extern __thread stack * g_stack;

#if DEVEL
/*
** set to the calltree for the executing thread on every api call
** makes it easy to access the calltree from gdb
*/
stack * S;
#endif

#define restrict __restrict

/// stack_setup
//
// SUMMARY
//  called during unwinding to setup the stack
//
// PARAMETERS
//  new_stack - whether to 
//
void stack_push();

/// stack_freeze
//
//
//
void stack_freeze(struct memblk * const restrict mb, stack * restrict s)
  __attribute__((nonnull));

/// stack_unfreeze
//
//
//
void stack_unfreeze(struct memblk * const restrict mb, stack * restrict s)
  __attribute__((nonnull));

/// stack_thaw
//
//
//
void stack_thaw(char * const restrict mb, stack * restrict s)
  __attribute__((nonnull));

#undef restrict
#endif
