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

#ifndef _LISTWISE_SELECTION_H
#define _LISTWISE_SELECTION_H

#include "listwise.h"

#define restrict __restrict

/// lstack_selection_stage
//
// SUMMARY
//  stage the selection 0:y
//
int lstack_selection_stage(lwx * const restrict lx, int y)
	__attribute__((nonnull));

/// lstack_selection_unstage
//
// SUMMARY
//  unstage staged selections
//
int lstack_selection_unstage(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_selection_activate
//
// SUMMARY
//  activate selections staged by the previous operation
//
int lstack_selection_activate(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_selection_reset
//
// SUMMARY
//  reset selection (select all)
//
int lstack_selection_reset(lwx * const restrict lx)
	__attribute__((nonnull));

#undef restrict
#endif
