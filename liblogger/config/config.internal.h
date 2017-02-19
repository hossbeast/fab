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

#ifndef _LOGGER_CONFIG_INTERNAL_H
#define _LOGGER_CONFIG_INTERNAL_H

#include <stdint.h>

#include "config.h"

extern char logger_process_name[32];
extern __thread char logger_thread_name[32];

extern uint64_t logger_process_categories;
extern __thread uint64_t logger_thread_categories;

#endif
