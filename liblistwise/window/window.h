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

#include "listwise.h"

#define restrict __restrict

/// lstack_window_stage
//
// SUMMARY
//  add a segment to the staged window for the specified row
//
int lstack_window_stage(lwx * const restrict lx, int y, int off, int len)
	__attribute__((nonnull));

/// lstack_window_deactivate
//
// SUMMARY
//  reset the active window for the specified row
//
int lstack_window_deactivate(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_window_unstage
//
// SUMMARY
//  reset the staged window for the specified row
//
int lstack_window_unstage(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_windows_activate
//
// SUMMARY
//  activate staged windows (even if their lease has expired)
//
int lstack_windows_activate(lwx * const restrict lx)
	__attribute__((nonnull));

#undef restrict
#endif
