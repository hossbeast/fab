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

#ifndef FABD_PATH_CACHE_H
#define FABD_PATH_CACHE_H

#include "xapi.h"
#include "types.h"

struct config;

extern char *path_cache_env_path; // $PATH

typedef struct path_cache_entry {
  int fd;
  uint16_t len;
  const char *filename; // pointer into s
  char s[];
} path_cache_entry;

xapi path_cache_setup(void);
xapi path_cache_cleanup(void);

/*
 * reapply path cache configuration
 *
 * config - effective config tree
 * dry    - whether to perform a dry-run
 */
xapi path_cache_reconfigure(struct config * restrict cfg, bool dry)
  __attribute__((nonnull));

xapi path_cache_search(const path_cache_entry ** restrict entry, const char * restrict file, uint16_t len)
  __attribute__((nonnull));

/*
 * reset the path cache
 */
xapi path_cache_reset(void);

#endif
