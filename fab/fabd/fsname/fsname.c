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

#include "macros.h"

#include "xlinux/xstring.h"

#include "fsname.h"

//
// static
//

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

//
// api
//

void fsname_init(fsname * const restrict p, const char * const restrict buf, size_t bufl)
{
#if 0
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
#endif

RUNTIME_ASSERT(bufl < (sizeof(p->name) / sizeof(p->name[0])));

  p->namel = MIN((sizeof(p->name) / sizeof(p->name[0])) - 1, bufl);
  memcpy(p->name, buf, p->namel);
  p->name[p->namel] = 0;

//  getdir(p->path, &p->dir, &p->dirl);
//  getname(p->path, &p->name, &p->namel);
  getext(p->name, &p->ext, &p->extl);
  getsuffix(p->name, &p->suffix, &p->suffixl);
  getbase(p->name, &p->base, &p->basel);
//  getstem(p->name, &p->stem, &p->steml);
}
