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

#ifndef _FF_H
#define _FF_H

#include "types.h"

/*

fab-file

*/

struct ff_node;

typedef struct ff_file
{

} ff_file;

xapi ff_setup(void);
xapi ff_cleanup(void);
xapi ff_report(void);

xapi ff_load_paths(const struct ff_node ** restrict ffn, const char * restrict path)
  __attribute__((nonnull));
xapi ff_load_pathf(const struct ff_node ** restrict ffn, const char * restrict fmt, ...)
  __attribute__((nonnull));

#endif
