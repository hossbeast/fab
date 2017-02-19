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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "lorien/path_normalize.h"

#include "path.h"

#include "zbuffer.h"

#define restrict __restrict

//
// static
//

static void getdir(const char * const p, char ** const dir, size_t * dirl)
{
  const char * s = p + strlen(p);
  while(s != p && *s != '/')
    s--;

  if(s == p && *s == '/')
  {
    *dir = (void*)p;
    *dirl = 1;
  }
  else if(s == p)
  {
    *dir = ".";
    *dirl = 1;
  }
  else
  {
    *dir = (void*)p;
    *dirl = s - p;
  }
}

static void getname(const char * const p, char ** const name, size_t * namel)
{
  const char * s = p + strlen(p);
  while(s != p && *s != '/')
    s--;

  if(*s == '/')
    s++;

  *name = (void*)s;
  *namel = strlen(s);
}

static void getext(const char * const p, char ** const ext, size_t * extl)
{
  const char * s = p + strlen(p);
  while(s != p && *s != '/')
    s--;

  while(s != (p + strlen(p)) && *s != '.')
    s++;

  *ext = 0;
  *extl = 0;
  if(*s == '.')
  {
    *ext = (void*)s + 1;
    *extl = strlen(s + 1);
  }
}

static void getsuffix(const char * const p, char ** const suffix, size_t * suffixl)
{
  const char * s = p + strlen(p);
  while(s != p && *s != '/' && *s != '.')
    s--;

  *suffix = 0;
  *suffixl = 0;
  if(*s == '.')
  {
    *suffix = (void*)s + 1;
    *suffixl = strlen(s + 1);
  }
}

//
// api
//

xapi path_createf(path ** const p, const char * const fmt, ...)
{
  enter;

  char space[512];

  va_list va;
  va_start(va, fmt);

  size_t sz = vsnprintf(space, sizeof(space), fmt, va);
  fatal(xmalloc, p, sizeof(**p) + sz + 1 + 2);
  (*p)->pathl = path_normalize((*p)->path, sz + 1 + 2, space);

  // properties
  getdir((*p)->path, &(*p)->dir, &(*p)->dirl);
  getname((*p)->path, &(*p)->name, &(*p)->namel);
  getext((*p)->path, &(*p)->ext, &(*p)->extl);
  getsuffix((*p)->path, &(*p)->suffix, &(*p)->suffixl);

finally:
  va_end(va);
coda;
}

void path_free(path * const p)
{
  wfree(p);
}

void path_xfree(path ** const p)
{
  path_free(*p);
  *p = 0;
}
