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
struct reconfigure_context;

extern struct logger_category * categories;

#define BASE 0
#if DEBUG || DEVEL || XAPI
#define L_IPC      categories[0x0].id
#define L_PROTOCOL categories[0x1].id
#undef BASE
#define BASE 2
#endif

#define L_ERROR    categories[BASE + 0x0].id
#define L_WARN     categories[BASE + 0x1].id
#define L_INFO     categories[BASE + 0x2].id
#define L_CONFIG   categories[BASE + 0x3].id
#define L_PARAMS   categories[BASE + 0x4].id
#define L_USAGE    categories[BASE + 0x5].id

#define L_NOTIFY   categories[BASE + 0x6].id
#define L_SERVER   categories[BASE + 0x7].id
#define L_SWEEPER  categories[BASE + 0x8].id
#define L_MONITOR  categories[BASE + 0x9].id
#define L_FABD     categories[BASE + 0xa].id

/// logging_setup
//
// SUMMARY
//  register logging categories
//
xapi logging_setup(uint32_t hash);

xapi logging_finalize(void);

/// logging_reconfigure
//
// SUMMARY
//  apply new configuration
//
// PARAMETERS
//  config - root of the effective config tree
//  dry    - if true, do not commit the new config
//
xapi logging_reconfigure(struct reconfigure_context * ctx, const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

#if DEBUG || DEVEL
// dont reconfigure logging if true
extern int g_logging_skip_reconfigure;
#endif

#endif
