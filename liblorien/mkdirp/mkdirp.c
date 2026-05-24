/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "xlinux/xstat.h"

#include "mkdirp.h"

void mkdirpw(mode_t mode, const char * const path, size_t pathl)
{
  char space[256];
  const char * t;
  const char * e;

  t = path;
  e = path + pathl;
  while(t != e)
  {
    t++;
    while(*t != '/' && t != e)
      t++;

    memcpy(space, path, t - path);
    space[t - path] = 0;

    uxmkdirs(mode, space);
  }
}

void mkdirps(mode_t mode, const char * const path)
{
  mkdirpw(mode, path, strlen(path));
}

void mkdirpf(mode_t mode, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  mkdirpvf(mode, fmt, va);

  va_end(va);
}

void mkdirpvf(mode_t mode, const char * const restrict fmt, va_list va)
{
  char space[256];
  size_t sz;

  sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz < sizeof(space))
  {
    mkdirpw(mode, space, sz);
  }
}
