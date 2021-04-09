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

#ifndef _ARGS_H
#define _ARGS_H

#include "xapi.h"
#include "types.h"

extern struct g_args_t {
  /* devel only */
  char *system_config_path;
  char *user_config_path;
  char *project_config_path;
  uint16_t default_filesystem_invalidate;
  uint64_t sweeper_period_nsec;
  bool kill;

  uint64_t hash;
  char *hash_str;
} g_args;

xapi args_setup(void);
xapi args_teardown(void);

#endif