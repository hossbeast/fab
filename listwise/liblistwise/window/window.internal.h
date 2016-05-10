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

#ifndef _LISTWISE_WINDOW_INTERNAL_H
#define _LISTWISE_WINDOW_INTERNAL_H

#include "window.h"

struct lwx;

#define restrict __restrict

/// lstack_windows_activate
//
// SUMMARY
//  activate staged windows (even if their lease has expired)
//
xapi lstack_windows_activate(struct lwx* const restrict lx)
	__attribute__((nonnull));

/// lstack_window_deactivate
//
// SUMMARY
//  reset the active window for the specified row
//
xapi lstack_window_deactivate(struct lwx* const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_windows_staged_state
//
// SUMMARY
//  get a value indicating the state of the staged windows for the specified row
//
// PARAMETERS
//  lx    - lwx instance
//  y     - row index
//  [win] - (returns) the staged windows for the specified row
//
// RETURNS
//  one of LWX_WINDOWS_*
//
int lstack_windows_staged_state(struct lwx* const restrict lx, int y, struct lwx_windows ** win)
	__attribute__((nonnull(1)));

#undef restrict
#endif
