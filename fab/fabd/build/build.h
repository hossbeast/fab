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

#ifndef _FABD_BUILD_H
#define _FABD_BUILD_H

#include "types.h"

struct build_slot;
struct configblob;
struct hashtable;
struct rcu_thread;

extern struct build_slot * build_slots;
extern struct hashtable * build_slots_bypid;
extern int build_slots_bypid_lock;
extern bool build_stage_failure;
extern int build_devnull_fd;

void build_setup(void);
void build_cleanup(void);

/*
 * reapply configuration to the build subsystem
 *
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
int build_reconfigure(struct configblob * restrict cfg, char * restrict err, uint16_t err_sz);

/*
 * carry out the build
 */
void build(struct rcu_thread *self)
  __attribute__((nonnull));

#endif
