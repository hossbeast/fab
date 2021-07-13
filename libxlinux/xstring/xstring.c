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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "xstring/xstring.h"
#include "xstdlib.h"

void API ixstrncat(char ** s1, const char * s2, int s2len)
{
  char * o = *s1;
  int s1len = 0;
  if(*s1)
    s1len = strlen(*s1);
  s2len = s2len ?: strlen(s2);

  xmalloc(s1, s1len + s2len + 1);
  if(o)
    memcpy(*s1, o, s1len);
  wfree(o);
  memcpy((*s1) + s1len, s2, s2len);
}

void API ixstrcat(char ** s1, const char * s2)
{
  char * o = *s1;
  int s1len = 0;
  if(o)
    s1len = strlen(o);

  int s2len = strlen(s2);

  xmalloc(s1, s1len + s2len + 1);

  if(o)
    memcpy(*s1, o, s1len);

  wfree(o);
  memcpy((*s1) + s1len, s2, s2len);
}

void API ixstrcatf(char ** s, char * fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  int req = vsnprintf(0, 0, fmt, va);
  va_end(va);

  char * o = *s;
  int len = 0;
  if(o)
    len = strlen(o);
  xmalloc(s, len + req + 1);
  if(o)
    memcpy(*s, o, len);

  wfree(o);
  va_start(va, fmt);
  vsprintf((*s) + len, fmt, va);
  va_end(va);
}

void API ixstrdup(char ** s1, const char * s2)
{
  return ixstrndup(s1, s2, strlen(s2));
}

void API ixstrndup(char ** s1, const char * s2, const size_t l)
{
  int s1len = 0 ;
  int s2len = l ?: strlen(s2);
  if(*s1)
    s1len = strlen(*s1);

  if(*s1 && s1len >= s2len)
  {
    memcpy(*s1, s2, s2len + 1);
  }
  else
  {
    wfree(*s1);
    *s1 = 0;

    ixstrncat(s1, s2, l);
  }
}

void API ixsprintf(char ** s, char * fmt, ...)
{
  va_list va;
  int req;

  va_start(va, fmt);
  req = vsnprintf(0, 0, fmt, va);
  va_end(va);

  wfree(*s);
  xmalloc(s, req + 1);

  va_start(va, fmt);
  vsprintf((*s), fmt, va);
  va_end(va);
}
