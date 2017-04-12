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

#ifndef _LOGGER_EXPR_H
#define _LOGGER_EXPR_H

#include <stdint.h>

/// logger_expr_resolve
//
// SUMMARY
//  test a logger expression
//
// PARAMETERS
//  expr    - logger expression
//  [exprl] - expr length
//
// RETURNS
//  true if the logger expression is syntactically valid and all categories and attributes resolve
//
int logger_expr_resolve(const char * const restrict expr)
  __attribute__((nonnull));

/// logger_expr_validate
//
// SUMMARY
//  test a logger expression
//
// PARAMETERS
//  expr    - logger expression
//  [exprl] - expr length
//
// RETURNS
//  true if the logger expression is syntactically valid
//
int logger_expr_validate(const char * const restrict expr)
  __attribute__((nonnull));

/// logger_expr_push
//
// SUMMARY
//  append filters and attributes to streams
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//  expr       - logger expression
//  [exprl]    - expr length
//
xapi logger_expr_push(uint8_t stream_id, const char * const restrict expr)
  __attribute__((nonnull));

/// logger_expr_pop
//
// SUMMARY
//  reverse the effects of a logger_expr_push operation
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_expr_pop(uint8_t stream_id);

/// logger_expr_unshift
//
// SUMMARY
//  prepend filters and attributes to streams
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//  expr       - logger expression
//  [exprl]    - expr length
//
xapi logger_expr_unshift(uint8_t stream_id, const char * const restrict expr)
  __attribute__((nonnull));

/// logger_expr_shift
//
// SUMMARY
//  reverse the effects of a logger_expr_unshift operation
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_expr_shift(uint8_t stream_id);

/// logger_expr_clear
//
// SUMMARY
//  remove all exprs from streams
//
// PARAMETERS
//  stream_id  - nonzero stream id, or 0 to apply the operation to all streams
//
xapi logger_expr_clear(uint8_t stream_id);

#endif
