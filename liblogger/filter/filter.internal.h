
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

#ifndef _LOGGER_FILTER_INTERNAL_H
#define _LOGGER_FILTER_INTERNAL_H

#include "xapi.h"

#include "filter.h"

struct narrator;
struct list;

typedef struct filter
{
  uint64_t  v;    // tag
  char      u;    // unknown, whether the tag text has any unrecognized components
  char      o;    // operation, '+' or '-'
} filter;

xapi filter_clone(filter * const restrict A, filter ** const restrict B)
  __attribute__((nonnull));

/// filter_free
//
// SUMMARY
//  free a filter with free semantics
//
void filter_free(filter * filterp);

/// filter_ifree
//
// SUMMARY
//  free a filter with iwfree semantics
//
void filter_ifree(filter ** const restrict filterp)
  __attribute__((nonnull));

/// filter_say
//
// SUMMARY
//
//
xapi filter_say(filter * filterp, struct narrator * N)
  __attribute__((nonnull));

/// filter_would
//
// SUMMARY
//  returns a boolean value indicating whether a log is passed by the filter
//
// PARAMETERS
//  filters -
//  ids     - category ids
//
int filters_would(const struct list * const restrict filters, const uint64_t ids)
  __attribute__((nonnull));

#endif
