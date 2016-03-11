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

/// logger_filter_push
//
// SUMMARY
//  append a filter to the filters for the specified stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//  expr       - logexpr
//  [expr_len] - length of expr, or 0 for strlen
//
xapi logger_filter_push(const int stream_id, const char * const restrict expr, size_t exprl)
	__attribute__((nonnull));

/// logger_filter_unshift
//
// SUMMARY
//  prepend a filter to the filters for the specified stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_unshift(const int stream_id, const char * const restrict expr, size_t exprl)
	__attribute__((nonnull));

/// logger_filter_pop
//
// SUMMARY
//  remove the last filter from the filters for the specified stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_pop(const int stream_id);

/// logger_filter_clear
//
// SUMMARY
//  remove the first filter from the filters for the specified stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_shift(const int stream_id);

/// logger_filter_clear
//
// SUMMARY
//  remove all filters from the specified stream
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_filter_clear(const int stream_id);

#endif
