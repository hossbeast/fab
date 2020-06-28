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

#ifndef FABD_FORMULA_H
#define FABD_FORMULA_H

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"

struct node;
struct formula_value;

typedef struct formula
{
  llist lln_invalidated;  // formulas_invalidated

  /* regarding the formula node itself */
  int fd;
  char *abspath;
  struct node * fml_node;
  bool shebang;

  /* parsed from the embedded bacon block */
  struct formula_value *file;   // primitive
  struct formula_value *envs;   // mappings set
  struct formula_value *args;   // list
} formula;

xapi formula_xfree(formula * restrict fml);

void formula_invalidated(formula * restrict fml)
  __attribute__((nonnull));

/// formula_load
//
// SUMMARY
//
// PARAMETERS
//  parser
//  fml    - (returns)
//
xapi formula_node_initialize(struct node * restrict fml_node)
  __attribute__((nonnull));

xapi formula_full_refresh(void);

xapi formula_setup(void);
xapi formula_cleanup(void);

#endif
