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

#ifndef _LOGGER_CATEGORY_H
#define _LOGGER_CATEGORY_H

#include <stdint.h>

/// logger_category
//
// SUMMARY
//  application components provide logger_category definitions to liblogger
//  liblogger assigns the same bits to definitions having the same name
//
typedef struct logger_category
{
  // name is unique among all components in an application
  char * name;            // e.g. INFO
  size_t namel;

  // if more than one definition is registered with a given name, their
  // properties are combined according to precedence ; higher rank means
  // greater precedence
  int rank;

  // the description accompanying the definition with the greatest precedence
  // is used ; if more than one definition has greatest precedence, it is
  // unspecified which description is used
  char * description;     // e.g. informational messages

  // options and modifiers
  uint32_t attr;          // e.g. L_RED | L_TRACE

  // all definitions having the same name are assigned the same bits by liblogger
  uint64_t id;
} logger_category;

#define restrict __restrict

/// logger_category_register
//
// SUMMARY
//  provide a list of categories to be resolved by the next
//  logger_category_resolve call
//
// PARAMETERS
//  logs       - sentinel-terminated list of categories used by the component
//  [identity] - name of application component, used in error message when
//               category definitions are found to be in conflict
//
xapi logger_category_register(logger_category * logs, char * const restrict identity)
  __attribute__((nonnull(1)));

/// logger_category_resolve
//
// SUMMARY
//  assign the bits member to all categories which have been registered since
//  the last logger_category_resolve call
//
xapi logger_category_resolve();

#undef restrict
#endif