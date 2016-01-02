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

#define LOGGER_ATTR_TABLE(x)                                                                              \
  LOGGER_ATTR_DEF(0x01  , COLOR_SET   , x)  /* apply the color option in the bits of this definition */   \
  LOGGER_ATTR_DEF(0x02  , HEADER_SET  , x)  /* */ \
  LOGGER_ATTR_DEF(0x02  , TRACE_SET   , x)  /* */ \

enum {
#define LOGGER_ATTR_DEF(a, b, x) LOGGER_ATTR_ ## b = a,
LOGGER_ATTR_TABLE(0)
#undef LOGGER_ATTR_DEF
};

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
  uint32_t attr;          // e.g. LOGGER_ATTR_COLOR_SET

  // components may specify options in bits outside the id range ; these options
  // are reconciled by liblogger according to precedence ; all definitions having
  // the same name are assigned the same bits by liblogger
  uint64_t bits;
} logger_category;

/// logger_category_register
//
// SUMMARY
//  provide a list of categories to be resolved by the next
//  logger_category_resolve call
//
// PARAMETERS
//  logs - sentinel-terminated list of categories used by the component
//
xapi logger_category_register(logger_category * logs)
  __attribute__((nonnull));

/// logger_category_resolve
//
// SUMMARY
//  assign the bits member to all categories which have been registered since
//  the last logger_category_resolve call
//
xapi logger_category_resolve();

#endif
