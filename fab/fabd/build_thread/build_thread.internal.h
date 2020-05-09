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

#ifndef _BUILD_THREAD_INTERNAL_H
#define _BUILD_THREAD_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "build_thread.h"

struct hashtable;
struct build_slot;

// runtime
extern int build_devnull_fd;

extern struct build_slot * build_slots;
extern struct hashtable * build_slots_bypid;
extern int build_slots_bypid_lock;
extern bool build_stage_failure;

// config
extern build_thread_config build_thread_cfg;

#endif