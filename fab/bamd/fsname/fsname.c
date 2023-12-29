/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "fsname.h"

#include "macros.h"

//
// static
//

static void getext(char * const restrict p, char ** const restrict ext, uint16_t * restrict extl)
{
  const char * s;
  const char * e;

  s = p + strlen(p);
  e = s;

  while(s != p && *s != '/') {
    s--;
  }

  while(s != e && *s != '.') {
    s++;
  }

  *ext = 0;
  *extl = 0;
  if(*s == '.')
  {
    *ext = (char*)s + 1;
    *extl = strlen(s + 1);
  }
}

static void getsuffix(char * const restrict p, char ** const restrict suffix, uint16_t * restrict suffixl)
{
  const char * s;

  s = p + strlen(p);
  while(s != p && *s != '/' && *s != '.') {
    s--;
  }

  *suffix = 0;
  *suffixl = 0;
  if(*s == '.')
  {
    *suffix = (char*)s + 1;
    *suffixl = strlen(s + 1);
  }
}

static void getbase(char * const restrict p, char ** const restrict base, uint16_t * restrict basel)
{
  const char * e;
  const char * s;

  e = p + strlen(p);
  s = e;

  while(s != p && *s != '/') {
    s--;
  }

  if(*s == '/') {
    s++;
  }

  *base = (char*)s;
  while(s != e && *s != '.') {
    s++;
  }

  *basel = s - *base;
}

//
// api
//

void fsname_init(fsname * const restrict p, const char * const restrict buf, size_t bufl)
{
  RUNTIME_ASSERT(bufl < (sizeof(p->name) / sizeof(p->name[0])));

  p->namel = MIN((sizeof(p->name) / sizeof(p->name[0])) - 1, bufl);
  memcpy(p->name, buf, p->namel);
  p->name[p->namel] = 0;

  getext(p->name, &p->ext, &p->extl);
  getsuffix(p->name, &p->suffix, &p->suffixl);
  getbase(p->name, &p->base, &p->basel);
}
