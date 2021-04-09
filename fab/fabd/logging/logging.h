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

struct configblob;

extern struct logger_category * categories;

#define L_ERROR     categories[0x00].id
#define L_WARN      categories[0x01].id
#define L_INFO      categories[0x02].id
#define L_PROTOCOL  categories[0x03].id
#define L_CONFIG    categories[0x04].id
#define L_PARAMS    categories[0x05].id
#define L_USAGE     categories[0x06].id
#define L_FSEVENT   categories[0x07].id
#define L_MODULE    categories[0x08].id
#define L_BUILDPLAN categories[0x09].id
#define L_BUILD     categories[0x0a].id
#define L_REQUEST   categories[0x0b].id
#define L_FORMULA   categories[0x0c].id
#define L_SELECTOR  categories[0x0d].id
#define L_PATTERN   categories[0x0e].id
#define L_RULE      categories[0x0f].id
#define L_NODE      categories[0x10].id
#define L_FSGRAPH   categories[0x11].id
#define L_DEPGRAPH  categories[0x12].id
#define L_RULEGRAPH categories[0x13].id
#if DEBUG || DEVEL || XAPI
#define L_IPC       categories[0x14].id
#define L_B 0x15
#else
#define L_B 0x14
#endif
#define L_NOTIFY    categories[L_B + 0x0].id
#define L_SERVER    categories[L_B + 0x1].id
#define L_SWEEPER   categories[L_B + 0x2].id
#define L_BUILDER   categories[L_B + 0x3].id
#define L_MONITOR   categories[L_B + 0x4].id
#define L_HANDLER   categories[L_B + 0x5].id
#define L_BEHOLDER  categories[L_B + 0x6].id
#define L_LOADER    categories[L_B + 0x7].id
#define L_WORKER    categories[L_B + 0x8].id

/// logging_setup
//
// SUMMARY
//  register logging categories
//
xapi logging_setup(uint64_t hash);

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
xapi logging_reconfigure(struct configblob * restrict config, bool dry)
  __attribute__((nonnull));

#endif
