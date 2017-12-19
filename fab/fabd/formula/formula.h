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

typedef struct formula
{

} formula;

/// formula_setup
//
// SUMMARY
//
xapi formula_setup(void);

/// formula_cleanup
//
// SUMMARY
//  free resources
//
xapi formula_cleanup(void);

xapi formula_load(formula ** restrict fmlp);

xapi formula_exec(const formula * restrict fml)
  __attribute__((nonnull));

#endif
