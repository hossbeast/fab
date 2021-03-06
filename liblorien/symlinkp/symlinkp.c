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

#include "xlinux/xunistd.h"

#include "symlinkp.h"

xapi API symlinkps(const char * const restrict target, const char * const restrict linkpath)
{
  enter;

  fatal(uxunlinks, linkpath);
  fatal(xsymlinks, target, linkpath);

  finally : coda;
}

xapi API symlinkpf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, linkpath_fmt);

  fatal(symlinkpvf, target_fmt, linkpath_fmt, va);

finally:
  va_end(va);
coda;
}

xapi API symlinkpvf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  // discard
  va_arg(va2, char*);

  fatal(uxunlinkvf, linkpath_fmt, va2);
  fatal(uxsymlinkvf, target_fmt, linkpath_fmt, va);

finally:
  va_end(va2);
coda;
}
