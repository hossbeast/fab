
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

#ifndef _LOGGER_EXPR_INTERNAL_H
#define _LOGGER_EXPR_INTERNAL_H

#include <stdint.h>

#include "xapi.h"
#include "expr.h"

struct list;
struct filter;

/// expr_lex
//
// SUMMARY
//  parse an expr
//
// PARAMETERS
//  expr      - expr to parse
//  exprl     - length of expr
//  [filterp] - (returns) filter properties
//  [attrsp]  - (returns) attributes
//
// RETURNS
//  boolean indicating whether the expr was syntactically valid
//
int expr_lex(const char * restrict expr, size_t exprl, struct filter * restrict filterp, uint32_t * restrict attrsp)
  __attribute__((nonnull));

/// expr_parse
//
// SUMMARY
//  parse a logger expression
//
// PARAMETERS
//  expr     - logger expr
//  filters  - (returns) parsed filters
//  attrs    - (returns) parsed attrs
//
// RETURNS
//  boolean indicating whether parsing was successful, that is, no syntax errors were encountered
//
//  -1 if any syntax errors in the expression were encountered, otherwise a number <= filtersz of elements in filters
//  that were written to
//
xapi expr_parse(const char * restrict expr, struct list * restrict filters, uint32_t * restrict attrs)
  __attribute__((nonnull));

/// expr_resolve
//
// SUMMARY
//  resolve a logger expression
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
int expr_resolves(const char * restrict expr, struct filter * restrict filterp, uint32_t * restrict attrsp)
  __attribute__((nonnull(1)));

int expr_resolvew(const char * restrict expr, size_t exprl, struct filter * restrict filterp, uint32_t * restrict attrsp)
  __attribute__((nonnull(1)));

/// expr_validate
//
// SUMMARY
//  test an expr
//
// PARAMETERS
//  expr    - expr to test
//  resolve - whether to also check whether all categories and attributes resolve
//
// RETURNS
//  true if the logger expression is valid
//
int expr_validate(const char * restrict expr, int resolve)
  __attribute__((nonnull));

#endif
