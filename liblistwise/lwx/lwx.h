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

#ifndef _LISTWISE_LWX_H
#define _LISTWISE_LWX_H

#include <stdint.h>

//
// listwise execution context
//
struct lwx_t;

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

#endif
