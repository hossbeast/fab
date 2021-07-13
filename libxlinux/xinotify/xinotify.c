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

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xinotify.h"

int API xinotify_init(int flags)
{
  int r;

  r = inotify_init1(flags);
  RUNTIME_ASSERT(r >= 0);

  return r;
}

/// inotify_add_watch
//
// SUMMARY
//  proxy for inotify_add_watch
//
int API xinotify_add_watch(int id, const char *path, uint32_t mask)
{
  int r;

  r = inotify_add_watch(id, path, mask);
  RUNTIME_ASSERT(r >= 0);

  return r;
}
