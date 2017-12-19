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

#ifndef _CONFIG_CURSOR_H
#define _CONFIG_CURSOR_H

#include "types.h"
#include "xapi.h"

struct pstring;

typedef struct config_cursor
{
  struct pstring * path;
  size_t mark;
} config_cursor;

xapi config_cursor_init(config_cursor * restrict cursor)
  __attribute__((nonnull));

void config_cursor_destroy(config_cursor * restrict cursor)
  __attribute__((nonnull));

xapi config_cursor_sets(config_cursor * restrict cursor, const char * restrict s)
  __attribute__((nonnull));

void config_cursor_mark(config_cursor * restrict cursor)
  __attribute__((nonnull));

xapi config_cursor_pushd(config_cursor * restrict cursor, int d)
  __attribute__((nonnull));

xapi config_cursor_pushs(config_cursor * restrict cursor, const char * restrict s)
  __attribute__((nonnull));

xapi config_cursor_pushf(config_cursor * restrict cursor, const char * restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

/// config_cursor_query
//
// SUMMARY
//  get the current query - the substring of the path from the mark to the end
//
const char * config_cursor_query(const config_cursor * restrict cursor)
  __attribute__((nonnull));

/// config_cursor_path
//
// SUMMARY
//  get the full path
//
const char * config_cursor_path(const config_cursor * restrict cursor)
  __attribute__((nonnull));

#endif
