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

#ifndef _XINOTIFY_H
#define _XINOTIFY_H

#include <sys/types.h>
#include <sys/inotify.h>

#include "xapi.h"

/// xinotify_init
//
// SUMMARY
//  proxy for inotify_init
//
xapi xinotify_init(int * id, int flags)
  __attribute__((nonnull));

/// inotify_add_watch
//
// SUMMARY
//  proxy for inotify_add_watch
//
xapi xinotify_add_watch(int * wd, int id, const char *path, uint32_t mask)
  __attribute__((nonnull));

#endif
