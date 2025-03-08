/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xunistd.h"

#include "symlinkp.h"

void symlinkps(const char * const restrict target, const char * const restrict linkpath)
{
  uxunlinks(linkpath);
  xsymlinks(target, linkpath);
}

void symlinkpf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, ...)
{
  va_list va;
  va_start(va, linkpath_fmt);

  symlinkpvf(target_fmt, linkpath_fmt, va);

  va_end(va);
}

void symlinkpvf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, va_list va)
{
  va_list va2;
  va_copy(va2, va);

  // discard
  va_arg(va2, char*);

  uxunlinkvf(linkpath_fmt, va2);
  uxsymlinkvf(target_fmt, linkpath_fmt, va);

  va_end(va2);
}
