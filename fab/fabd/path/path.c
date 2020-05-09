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

#include "path.h"

//
// static
//

static void getdir(char * const restrict p, char ** const restrict dir, uint16_t * restrict dirl)
{
  char * s = p + strlen(p);
  while(s != p && *s != '/')
    s--;

  if(s == p && *s == '/')
  {
    *dir = p;
    *dirl = 1;
  }
  else if(s == p)
  {
    *dir = ".";
    *dirl = 1;
  }
  else
  {
    *dir = p;
    *dirl = s - p;
  }
}

static void getname(char * const restrict p, char ** const restrict name, uint16_t * restrict namel)
{
  char * e = p + strlen(p);
  char * s = e;
  while(s != p && *s != '/')
    s--;

  if(*s == '/')
    s++;

  *name = s;
  *namel = e - s;
}

static void getext(char * const restrict p, char ** const restrict ext, uint16_t * restrict extl)
{
  char * s = p + strlen(p);
  while(s != p && *s != '/')
    s--;

  while(s != (p + strlen(p)) && *s != '.')
    s++;

  *ext = 0;
  *extl = 0;
  if(*s == '.')
  {
    *ext = s + 1;
    *extl = strlen(s + 1);
  }
}

static void getsuffix(char * const restrict p, char ** const restrict suffix, uint16_t * restrict suffixl)
{
  char * s = p + strlen(p);
  while(s != p && *s != '/' && *s != '.')
    s--;

  *suffix = 0;
  *suffixl = 0;
  if(*s == '.')
  {
    *suffix = s + 1;
    *suffixl = strlen(s + 1);
  }
}

static void getbase(char * const restrict p, char ** const restrict base, uint16_t * restrict basel)
{
  char * e = p + strlen(p);
  char * s = e;
  while(s != p && *s != '/')
    s--;

  if(*s == '/')
    s++;

  *base = s;
  while(s != e && *s != '.')
    s++;

  *basel = s - *base;
}

static void getstem(char * const restrict p, char ** const restrict stem, uint16_t * restrict steml)
{
  char * e = p + strlen(p);
  char * s = e;
  while(s != p && *s != '/')
    s--;

  while(s != e && *s != '.')
    s++;

  *stem = p;
  *steml = s - p;
}

/// path_initialize
//
// SUMMARY
//  compute derived properties
//
static void path_initialize(path * const restrict p)
{
  getdir(p->path, &p->dir, &p->dirl);
  getname(p->path, &p->name, &p->namel);
  getext(p->path, &p->ext, &p->extl);
  getsuffix(p->path, &p->suffix, &p->suffixl);
  getbase(p->path, &p->base, &p->basel);
  getstem(p->path, &p->stem, &p->steml);
}

//
// api
//

xapi path_creates(path ** const restrict p, const char * const restrict s)
{
  xproxy(path_createw, p, s, strlen(s));
}

xapi path_createw(path ** const restrict p, const char * const restrict buf, size_t bufl)
{
  enter;

  if(bufl && buf[0] != '.' && buf[0] != '/')
  {
    fatal(xmalloc, p, sizeof(**p) + bufl + 3);
    memcpy((*p)->path, "./", 2);
    memcpy((*p)->path + 2, buf, bufl);
    (*p)->path[bufl + 2] = 0;
    (*p)->pathl = bufl + 2;
  }
  else
  {
    fatal(xmalloc, p, sizeof(**p) + bufl + 1);
    memcpy((*p)->path, buf, bufl);
    (*p)->path[bufl] = 0;
    (*p)->pathl = bufl;
  }

  path_initialize(*p);

  finally : coda;
}

xapi path_createf(path ** const restrict p, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);

  fatal(path_createvf, p, fmt, va);

finally:
  va_end(va);
coda;
}

xapi path_createvf(path ** const restrict p, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  int r = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  fatal(xmalloc, p, sizeof(**p) + r + 1);
  vsprintf((*p)->path, fmt, va);

  path_initialize(*p);

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
