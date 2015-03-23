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

#ifndef _LISTWISE_WINDOWS_H
#define _LISTWISE_WINDOWS_H

#include "macros.h"

// forward declaration
struct lwx_windows;		// lwx.h
typedef struct lwx_windows lwx_windows;

// window states
#define LWX_WINDOWS_TABLE(x)																				\
	_LWX_WINDOWS(ALL	, 0x00	, x)		/* universe-set */							\
	_LWX_WINDOWS(NONE	, 0x01	, x)		/* empty-set */									\
	_LWX_WINDOWS(SOME	, 0x02	, x)		/* non-empty proper subset */

enum {
#define _LWX_WINDOWS(a, b, x) LWX_WINDOWS_ ## a = b,
LWX_WINDOWS_TABLE(0)
#undef _LWX_WINDOWS
};

#define _LWX_WINDOWS(a, b, x) (x) == (b) ? "LWX_WINDOWS_" #a :
#define LWX_WINDOWS_STR(x) LWX_WINDOWS_TABLE(x) "UNKNOWN"

#define restrict __restrict

/// lstack_windows_stage
//
// SUMMARY
//  add a segment to the staged window(s) for the specified row
//
int lstack_windows_stage(lwx * const restrict lx, int y, int off, int len)
	__attribute__((nonnull));

/// lstack_windows_stage_all
//
// SUMMARY
//  set the staged windows to the universe-set
//
int lstack_windows_stage_all(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_windows_stage_nil
//
// SUMMARY
//  set the staged windows to the empty-set
//
int lstack_windows_stage_nil(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_windows_state
//
// SUMMARY
//  get a value indicating the state of the windows for the specified row
//
// PARAMETERS
//  lx    - lwx instance
//  y     - row index
//  [win] - (returns) the windows for the specified row
//
// RETURNS
//  one of LWX_WINDOWS_*
//
int lstack_windows_state(lwx * const restrict lx, int y, struct lwx_windows ** win)
	__attribute__((nonnull(1)));

#undef restrict
#endif
