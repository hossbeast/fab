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

#ifndef _FABD_RULE_INTERNAL_H
#define _FABD_RULE_INTERNAL_H

#include "rule.h"

struct node;
struct list;
struct map;
struct artifact;
struct list;

xapi rules_apply_rules(
    struct node * restrict consequent
  , const struct list * restrict rules_lists
  , struct node * restrict base
  , struct map * restrict scope
  , struct artifact * restrict af
  , struct list * restrict antecedents
  , struct list * restrict consequents
)
  __attribute__((nonnull));

#endif
