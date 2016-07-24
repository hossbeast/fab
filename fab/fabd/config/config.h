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

#define restrict __restrict

/// config_load
//
// SUMMARY
//  (re)-load the config files, discard
//
xapi config_load();

/// config_apply
//
// SUMMARY
//  apply new config
//
// PARAMETERS
//  src
//  len
//
xapi config_apply(const char * const restrict src, size_t len)
  __attribute__((nonnull));

/// config_query
//
// SUMMARY
//  query the config tree for a value
//
// PARAMETERS
//  query - query
//  val   - (returns) the value
//
xapi config_query(const char * const restrict query, struct value ** const restrict val)
  __attribute__((nonnull));

/// config_setup
//
// SUMMARY
//  initialize the config module
//
xapi config_setup();

/// config_teardown
//
// SUMMARY
//  free resources
//
void config_teardown();

#undef restrict
#endif
