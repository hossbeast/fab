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

#ifndef _SWEEPER_THREAD_H
#define _SWEEPER_THREAD_H

#include "xapi.h"

xapi sweeper_thread_setup(void);

xapi sweeper_thread_cleanup(void);

xapi sweeper_thread_launch(void);

xapi sweeper_thread_enqueue(int wd, uint32_t m, const char * restrict name, size_t namel)
  __attribute__((nonnull));

#endif
