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

#ifndef FABD_CONFIG_H
#define FABD_CONFIG_H

#include "types.h"
#include "xapi.h"

struct value;         // libvalue
struct value_store;

#define CONFIG_QUERY_NOTNULL 0x10000

/// g_config
//
// root of the active config tree
//
struct value * g_config;

/// config_setup
//
// SUMMARY
//  initializes the config module, does not apply any config files
//
xapi config_setup(void);

/// config_cleanup
//
// SUMMARY
//  free resources
//
xapi config_cleanup(void);

/// config_report
//
// SUMMARY
//  log the active config with L_CONFIG
//
xapi config_report(void);

/// config_query
//
// SUMMARY
//  query a config tree
//
// PARAMETERS
//  base   - config tree to query
//  path   - path from the root of the config tree
//  query  - path from base
//  [opts] - bitwise combo of VALUE_TYPE_* and CONFIG_QUERY_*
//  val    - (returns) the value
//
// ERRORS
//  CONFIG_ILLEGAL - opts inclues VALUE_TYPE_* and the query matched a value of some other type, or opts includes
//                   CONFIG_QUERY_NOTNULL, and the query did not match any value
//
xapi config_query(const struct value * restrict base, const char * restrict path, const char * restrict query, uint32_t opts, struct value ** const restrict val)
  __attribute__((nonnull));

#endif
