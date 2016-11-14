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

#include "xapi.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "config_cursor.h"

//
// static
//

static void config_cursor_reset(config_cursor * restrict cursor)
{
  psclear(cursor->path);
  cursor->mark = 0;
}

//
// public
//

xapi config_cursor_init(config_cursor * restrict cursor)
{
  enter;

  fatal(pscreate, &cursor->path);

  finally : coda;
}

void config_cursor_destroy(config_cursor * restrict cursor)
{
  psfree(cursor->path);
}

void config_cursor_mark(config_cursor * restrict cursor)
{
  cursor->mark = cursor->path->l;
}

const char * config_cursor_query(const config_cursor * restrict cursor)
{
  int del = cursor->mark;
  if(cursor->mark != 0 && cursor->path->l > cursor->mark)
    del++;

  return cursor->path->s + del;
}

const char * config_cursor_path(const config_cursor * restrict cursor)
{
  return cursor->path->s;
}

xapi config_cursor_sets(config_cursor * restrict cursor, const char * restrict s)
{
  enter;

  config_cursor_reset(cursor);
  fatal(config_cursor_pushs, cursor, s);

  finally : coda;
}

xapi config_cursor_pushd(config_cursor * restrict cursor, int d)
{
  enter;

  char space[16];

  snprintf(space, sizeof(space), "%d", d);
  fatal(config_cursor_pushs, cursor, space);

  finally : coda;
}

xapi config_cursor_pushf(config_cursor * restrict cursor, const char * restrict fmt, ...)
{
  enter;

  char space[128];

  va_list va;
  va_start(va, fmt);
  vsnprintf(space, sizeof(space), fmt, va);
  fatal(config_cursor_pushs, cursor, space);

finally:
  va_end(va);
coda;
}

xapi config_cursor_pushs(config_cursor * restrict cursor, const char * restrict s)
{
  enter;

  const char * start = s;
  const char * end = 0;

  cursor->path->l = cursor->mark;

  while(*start)
  {
    end = start + 1;
    while(*end && *end != '.')
      end++;

    if(cursor->path->l)
      fatal(pscatc, cursor->path, '.');

    fatal(pscatw, cursor->path, start, end - start);

    start = end;
    if(*start == '.')
      start++;
  }

  finally : coda;
}
