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

#ifndef _XAPI_FRAME_INTERNAL_H
#define _XAPI_FRAME_INTERNAL_H

#include <sys/types.h>

#include "frame.h"

struct stack;
struct info;
struct memblk;

typedef struct frame
{
	char * 					file;		// file name
  size_t          filel;
  size_t          filea;
	char * 					func;		// function name
  size_t          funcl;
  size_t          funca;
	int							line;   // line number

	struct                  // infos for the frame
	{
		struct info * v;
		size_t        l;
		size_t        a;
	} infos;

  struct stack *  child;    // child stack
} frame;

#if XAPI_RUNTIME_CHECKS
// per-thread stack of frame addresses
extern __thread struct frame_addresses
{
  void **   v;
  size_t    l;
  size_t    a;
} g_frame_addresses;
#endif

#define restrict __restrict 

/// frame_freeze
//
//
//
void frame_freeze(struct memblk * const restrict mb, frame * f)
  __attribute__((nonnull));

/// frame_unfreeze
//
//
//
void frame_unfreeze(struct memblk * const restrict mb, frame * f)
  __attribute__((nonnull));

/// frame_thaw
//
//
//
void frame_thaw(char * const restrict mb, frame * f)
  __attribute__((nonnull));

/// frame_teardown
//
// SUMMARY
//  release memory
//
void frame_teardown();

#undef restrict
#endif
