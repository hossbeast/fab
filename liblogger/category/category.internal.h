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

#ifndef _LOGGER_CATEGORY_INTERNAL_H
#define _LOGGER_CATEGORY_INTERNAL_H

#include <stdint.h>

#include "xapi.h"

#include "category.h"

struct narrator;  // libnarrator
struct list;

extern struct list * activated;

/// category_name_max_length
//
// SUMMARY
//  length of the longest category name, calculated in categories_activate
//
extern int category_name_max_length;

/// category_optional_mask
//
// SUMMARY
//  bitmask of categories which are marked as optional for filtering, calculated in categories_activate
//
extern uint64_t category_optional_mask;

/// category_setup
//
// SUMMARY
//  module initialization
//
xapi category_setup(void);

/// category_cleanup
//
// SUMMARY
//  release resources owned by this module
//
xapi category_cleanup(void);


/// category_list_merge
//
// SUMMARY
//  merge two lists of categories in such a way that preserves the ordering of
//  each element relative to other elements in its source list
//
// PARAMETERS
//  A - source list
//  B - source list
//  C - dest list
//
// THROWS
//  ILLORDER - relative ordering of all elements cannot be maintained
//
xapi category_list_merge(struct list * restrict A, struct list * restrict B, struct list * restrict C)
  __attribute__((nonnull));

/// categories_activate
//
// SUMMARY
//  assign the bits member to all categories which have been registered since the last
//  categories_activate call
//
// THROWS
//  TOOMANY
//  ILLREPEAT
//  ILLORDER
//
xapi categories_activate(void);

/// category_byname
//
// SUMMARY
//  lookup an active category by name
//
// PARAMETERS
//  name     - name of the category to lookup
//  namel    - name length or 0 for strlen
//
// RETURNS
//  category definition, or null if no such category was found
//
logger_category * category_byname(const char * const restrict name, size_t namel)
  __attribute__((nonnull));

/// category_byid
//
// SUMMARY
//  lookup an active category by id
//
// PARAMETERS
//  id       - id
//  category - (returns) category definition
//
// RETURNS
//  category definition, or null if no such category was found
//
logger_category * category_byid(uint64_t id);

/// category_report_verbose
//
// SUMMARY
//  log a summary of activated categories to LOGGER
//
xapi categories_report_verbose(void);

/// category_say
//
// SUMMARY
//  write a description of a logger category to a narrator
//
// PARAMETERS
//  cat - logger category
//  N   - narrator
//
xapi category_say(logger_category * const restrict cat, struct narrator * const restrict N)
  __attribute__((nonnull));

/// category_say_verbose
//
// SUMMARY
//  write a verbose description of a logger category to a narrator
//
// PARAMETERS
//  cat - logger category
//  N   - narrator
//
xapi category_say_verbose(logger_category * const restrict cat, struct narrator * const restrict N)
  __attribute__((nonnull));

// base attributes : category attributes + log site attributes
uint32_t categories_attrs(uint64_t ids);

#endif
