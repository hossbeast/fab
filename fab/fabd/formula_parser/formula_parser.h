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

#ifndef _FABD_FORMULA_PARSER_H
#define _FABD_FORMULA_PARSER_H

#include "xapi.h"
#include "types.h"

struct formula;

typedef struct formula_parser formula_parser;

xapi formula_parser_create(formula_parser ** const restrict p)
  __attribute__((nonnull));

/* free a formula parser with free semantics */
xapi formula_parser_xfree(formula_parser * restrict);

/* free a formula parser with iwfree semantics */
xapi formula_parser_ixfree(formula_parser ** restrict)
  __attribute__((nonnull));

xapi formula_parser_parse(
    formula_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct formula * restrict fml
)
  __attribute__((nonnull(1, 2, 5)));

#endif
