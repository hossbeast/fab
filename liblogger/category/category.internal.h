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

/// category_name_max_length
//
// SUMMARY
//  length of the longest category name
//  written in logger_category_resolve
//
extern int category_name_max_length;

/// category_setup
//
// SUMMARY
//
//
xapi category_setup();

/// category_teardown
//
// SUMMARY
//  
//
void category_teardown();

/// category_byname
//
// SUMMARY
//  lookup an active category by name
//
// PARAMETERS
//  name     - name of the category to lookup
//  namel    - name length or 0 for strlen
//  category - (returns) category definition
//
xapi category_byname(const char * const restrict name, size_t namel, logger_category ** const restrict category)
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
xapi category_byid(uint64_t id, logger_category ** const restrict category)
  __attribute__((nonnull));

/// category_report_verbose
//
// SUMMARY
//  log a summary of activated categories to LOGGER
//
xapi category_report_verbose();

/// category_say_verbose
//
// SUMMARY
//  write a description of a logger category to a narrator
//
// PARAMETERS
//  cat - logger category
//  N   - narrator
//
xapi category_say_verbose(logger_category * const restrict cat, struct narrator * const restrict N)
  __attribute__((nonnull));

#endif
