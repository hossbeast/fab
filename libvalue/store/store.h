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

#ifndef _VALUE_STORE_H
#define _VALUE_STORE_H

#include "xapi.h"

struct value_store;
typedef struct value_store value_store;

/// value_store_create
//
// SUMMARY
//  create a value_store
//
xapi value_store_create(value_store ** const restrict store)
  __attribute__((nonnull));

/// value_store_free
//
// SUMMARY
//  free a value_store with free semantics
//
void value_store_free(value_store * const restrict);

/// value_store_ifree
//
// SUMMARY
//  free a value_store with ifree semantics
//
void value_store_ifree(value_store ** const)
  __attribute__((nonnull));

#endif
