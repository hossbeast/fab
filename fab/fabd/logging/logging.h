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

#ifndef _LOGGING_H
#define _LOGGING_H

#include "xapi.h"

#include "logger.h"
#include "logger/category.h"

struct value;

extern struct logger_category * categories;

#define L_ERROR   categories[0].id
#define L_WARN    categories[1].id
#define L_INFO    categories[2].id
#define L_CONFIG  categories[3].id
#define L_PARAMS  categories[4].id
#define L_USAGE   categories[5].id
#if DEBUG || DEVEL || XAPI
#define L_IPC     categories[6].id
#endif

/// logging_setup
//
// SUMMARY
//  register logging categories
//
xapi logging_setup(uint32_t hash);

/// logging_reconfigure
//
// SUMMARY
//  apply new configuration
//
// PARAMETERS
//  config - root of the effective config tree
//  dry    - if true, do not commit the new config
//
xapi logging_reconfigure(const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

#endif
