/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_WINDOW_H
#define _LISTWISE_WINDOW_H

struct lwx_windows;		// lwx.h

#define restrict __restrict

/// lstack_window_stage
//
// SUMMARY
//  add a segment to the staged window(s) for the specified row
//
int lstack_window_stage(lwx * const restrict lx, int y, int off, int len)
	__attribute__((nonnull));

#define LWX_WINDOWED_ALL		0		/* all rows are selected */
#define LWX_WINDOWED_NONE		1		/* no rows are selected */
#define LWX_WINDOWED_SOME		2		/* 1+ rows are selected and 1+ rows are not */

/// lstack_windows_state
//
// SUMMARY
//  get a value indicating the state of the windows for the specified row
//
// PARAMETERS
//  lx    - 
//  y     - 
//  [win] - 
//
// RETURNS
//  one of LWX_WINDOWED_*
//
int lstack_windows_state(lwx * const restrict lx, int y, struct lwx_windows ** win)
	__attribute__((nonnull(1)));

#undef restrict
#endif
