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

#ifndef _LOGGER_FILTER_H
#define _LOGGER_FILTER_H

/// logger_filter_validate
//
// SUMMARY
//  test a filter expression
//
// PARAMETERS
//  expr       - space-delimited filter exprs
//  [exprl]    - expr length
//
// RETURNS
//  true if the filter expression is syntactically valid and all categories resolve
//
int logger_filter_validates(const char * const restrict expr)
  __attribute__((nonnull));

int logger_filter_validatew(const char * const restrict expr, size_t exprl)
  __attribute__((nonnull));

/// logger_filter_pushs
//
// SUMMARY
//  append filters to a stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//  expr       - space-delimited filter exprs
//  [exprl]    - expr length
//
xapi logger_filter_pushs(int stream_id, const char * const restrict expr)
  __attribute__((nonnull));

xapi logger_filter_pushw(int stream_id, const char * const restrict expr, size_t exprl)
  __attribute__((nonnull));

/// logger_filter_unshift
//
// SUMMARY
//  prepend filters to a stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//  expr       - space-delimited filter exprs
//  [exprl]    - expr length
//
xapi logger_filter_unshifts(int stream_id, const char * const restrict expr)
  __attribute__((nonnull));

xapi logger_filter_unshiftw(int stream_id, const char * const restrict expr, size_t exprl)
  __attribute__((nonnull));

/// logger_filter_pop
//
// SUMMARY
//  remove the last filter from the filters for a stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_pop(int stream_id);

/// logger_filter_clear
//
// SUMMARY
//  remove the first filter from the filters for a stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_shift(int stream_id);

/// logger_filter_clear
//
// SUMMARY
//  remove all filters from a stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_clear(int stream_id);

#endif
