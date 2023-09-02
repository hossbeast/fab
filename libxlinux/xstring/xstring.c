/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "xstring.h"
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
  free(o);
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

  free(o);
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

  free(o);
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
    free(*s1);
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

  free(*s);
  xmalloc(s, req + 1);

  va_start(va, fmt);
  vsprintf((*s), fmt, va);
  va_end(va);
}
