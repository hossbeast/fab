
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
  char      m;    // mode, ' ' or '%'
  char      o;    // operation, '+' or '-'
} filter;

/// filter_setup
//
// SUMMARY
//  initialize
//
xapi filter_setup();

/// filter_teardown
//
// SUMMARY
//  cleanup
//
void filter_teardown();

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

/// filter_parse
//
// SUMMARY
//  parse a filter expression
//
// PARAMETERS
//  expr         - filter expression
//  exprl        - expr length
//  [filterp]    - (returns) if parsing was successful, returns the filter
//
// RETURNS
//  true if the expression was valid syntactically
//
// REMARKS
//  to check whether ANY categories were resolved, check filterp->v
//  it is not possible to check whether ALL categories were resolved
//
int filter_parses(const char * const restrict expr, filter * const restrict filterp)
  __attribute__((nonnull(1)));

int filter_parsew(const char * const restrict expr, size_t exprl, filter * const restrict filterp)
  __attribute__((nonnull(1)));

/// filter_expr_parse
//
// SUMMARY
//  parse a whitespace-delimited list of filter expressions
//
// PARAMETERS
//  expr       - space-delimited filter exprs
//  exprl      - expr length
//  [filters]  - (returns) parsed filters
//  [filtersz] - size of filters in elements
//
// RETURNS
//  -1 if any syntax errors in the expression were encountered, otherwise a number <= filtersz of elements in filters
//  that were written to
//
int filter_expr_parse(const char * const restrict expr, size_t exprl, filter * filters, size_t filtersz)
  __attribute__((nonnull(1)));

/// filter_expr_process
//
// SUMMARY
//  process a whitespace-delimited list of filter expressions
//
// PARAMETERS
//
// RETURNS
//
xapi filter_expr_process(
    const char * const restrict expr
  , size_t exprl
  , struct list * const restrict list
  , xapi (*listop)(struct list * const restrict, void *)
)
  __attribute__((nonnull(1)));

/// filter_push
//
// SUMMARY
//  append a filter to stream(s)
//
// PARAMETERS
//  [stream_id] - id of a stream to append the filter to, or 0 to apply to all active streams
//  filterp     - filter
//
// REMARKS
//  takes ownership of the filter instance
//
xapi filter_push(const int stream_id, filter * restrict filterp)
  __attribute__((nonnull));

/// filter_unshift
//
// SUMMARY
//
xapi filter_unshift(const int stream_id, filter * const restrict filterp)
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
