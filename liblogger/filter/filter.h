
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

/// logger_filter_apply
//
// SUMMARY
//  parse the logexpr to a filter and add it to the end of the list, or append
//  it, making it the last filter
//
// PARAMETERS
//  expr       - logexpr
//  [expr_len] - length of expr, or 0 for strlen
//
xapi logger_filter_push(const char * const restrict expr, size_t exprl)
	__attribute__((nonnull));

/// logger_filter_unshift
//
// SUMMARY
//  see logger_filter_push, except the new filter is added at the front, or
//  prepended, making it the first filter
//
xapi logger_filter_unshift(const char * const restrict expr, size_t exprl)
	__attribute__((nonnull));

/// logger_filter_pop
//
// SUMMARY
//  remove the last filter
//
xapi logger_filter_pop();

/// logger_filter_clear
//
// SUMMARY
//  remove the first filter
//
xapi logger_filter_shift();

/// logger_filter_clear
//
// SUMMARY
//  remove all filters
//
xapi logger_filter_clear();

#endif
