
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
	uint64_t	v;		// tag
  char      m;    // mode, ( or % or ^
	char			o;		// operation, + or -
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
//  free a filter with ifree semantics
//
void filter_ifree(filter ** const restrict filterp)
  __attribute__((nonnull));

/// filter_say
//
// SUMMARY
//  
//
xapi filter_say(filter * filterp, struct narrator * n)
  __attribute__((nonnull));

/// filter_parse
//
// SUMMARY
//  parse a filter expression
//
// PARAMETERS
//  expr  - filter expression
//  exprl - length of expr ; 0 for strlen
//  res   - (returns) the filter
//
xapi filter_parse(const char * const restrict expr, size_t exprl, filter ** const restrict res)
  __attribute__((nonnull));

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
int filters_would(const struct list * const restrict filters, const uint64_t ids)
  __attribute__((nonnull));

#endif
