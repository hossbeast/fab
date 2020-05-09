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

#ifndef FABD_FORMULA_INTERNAL_H
#define FABD_FORMULA_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "formula.h"
#include "node.h"

struct attrs32;
struct build_slot;
struct set;
struct narrator;

xapi render_vars(
    struct set * restrict vars
  , struct build_slot * restrict bs
  , struct narrator * restrict N
  , size_t ** restrict env_offs
  , size_t * restrict env_offs_len
)
  __attribute__((nonnull));

/// formula_vartab_create
//
//
//
xapi formula_vartab_create(struct set ** restrict rv)
  __attribute__((nonnull));

#endif
