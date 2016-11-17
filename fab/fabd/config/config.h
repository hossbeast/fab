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

#ifndef _CONFIG_H
#define _CONFIG_H

#include "xapi.h"

struct value;   // libvalue

#define CONFIG_QUERY_NOTNULL 0x10000

#define restrict __restrict

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

/// config_files_apply
//
// SUMMARY
//  reload the config files and apply them to the staging config
//
xapi config_files_apply(void);

/// config_apply
//
// SUMMARY
//  cumulatively apply additional config to the staging config
//
// PARAMETERS
//  text - configuration text to apply
//
xapi config_apply(const char * const restrict text)
  __attribute__((nonnull));

/// config_reconfigure
//
// SUMMARY
//  promoate the staging config to the active config, reconfigure subsystems, and call config_report
//
xapi config_reconfigure(xapi * restrict res)
  __attribute__((nonnull));

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

/// config_throw
//
// SUMMARY
//  fail with CONFIG_ILLEGAL
//
// PARAMETERS
//  error  - error, CONFIG_*
//  val    - throw config object
//  [path] - path from the root of the config tree
//
xapi config_throw(xapi error, struct value * restrict val, const char * restrict path)
  __attribute__((nonnull(2)));

#if 0
/// config_list_get
//
// SUMMARY
//  get an element from a config list
//
// PARAMETERS
//  list   - config list
//  index  - index of the element to get
//  [opts] - bitwise combo of VALUE_TYPE_* and CONFIG_QUERY_*
//  val    - (returns) the value
//
// ERRORS
//  CONFIG_ILLEGAL - opts inclues VALUE_TYPE_* and the query matched a value of some other type, or opts includes
//                   CONFIG_QUERY_NOTNULL, and the query did not match any value
//
xapi config_list_get(struct value * const restrict list, size_t index, uint32_t opts, struct value ** const restrict val)
  __attribute__((nonnull));

/// config_value_query
//
// SUMMARY
//  query a config tree
//
// PARAMETERS
//  base   - config tree to query
//  query  - query
//  [opts] - bitwise combo of VALUE_TYPE_* and CONFIG_QUERY_*
//  val    - (returns) the value
//
// ERRORS
//  CONFIG_ILLEGAL - opts inclues VALUE_TYPE_* and the query matched a value of some other type, or opts includes
//                   CONFIG_QUERY_NOTNULL, and the query did not match any value
//
xapi config_value_query(struct value * base, const char * const restrict query, uint32_t opts, struct value ** const restrict val)
  __attribute__((nonnull));
#endif

#undef restrict
#endif
