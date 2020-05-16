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
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"

#include "snarf.h"
#include "fmt.h"

static xapi snarf(int fd, char ** const restrict dst, size_t * restrict dstlp)
{
  enter;

  size_t dstl = 0;
  size_t dsta = 0;

  do
  {
    if((dsta - dstl) == 0)
    {
      size_t newa = dsta ?: 128;
      newa += newa * 2 + newa / 2;
      fatal(xrealloc, dst, sizeof(**dst), newa, dsta);
      dsta = newa;
    }

    ssize_t count;
    fatal(xread, fd, &(*dst)[dstl], dsta - dstl, &count);
    dstl += count;
  } while(dstl == dsta);

  if(dstlp)
    *dstlp = dstl;

  finally : coda;
}

//
// public
//

xapi snarfs(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path)
{
  xproxy(snarfats, dst, dstlp, AT_FDCWD, path);
}

xapi snarff(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(snarfatvf, dst, dstlp, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

xapi snarfvf(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path_fmt, va_list va)
{
  xproxy(snarfatvf, dst, dstlp, AT_FDCWD, path_fmt, va);
}


xapi snarfats(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path)
{
  enter;

  int fd = -1;
  fatal(xopenats, &fd, O_RDONLY, dirfd, path);
  fatal(snarf, fd, dst, dstlp);

finally:
  fatal(ixclose, &fd);
coda;
}

xapi snarfatf(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(snarfatvf, dst, dstlp, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

xapi snarfatvf(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path_fmt, va_list va)
{
  enter;

  char path[512];
  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(snarfats, dst, dstlp, dirfd, path);

finally:
  va_end(va);
coda;
}


xapi usnarfs(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path)
{
  xproxy(usnarfats, dst, dstlp, AT_FDCWD, path);
}

xapi usnarff(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(usnarfatvf, dst, dstlp, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

xapi usnarfvf(char ** const restrict dst, size_t * const restrict dstlp, const char * const restrict path_fmt, va_list va)
{
  xproxy(usnarfatvf, dst, dstlp, AT_FDCWD, path_fmt, va);
}


xapi usnarfats(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path)
{
  enter;

  int fd = -1;
  fatal(uxopenats, &fd, O_RDONLY, dirfd, path);
  if(fd != -1)
    fatal(snarf, fd, dst, dstlp);

finally:
  fatal(ixclose, &fd);
coda;
}

xapi usnarfatf(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(usnarfatvf, dst, dstlp, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

xapi usnarfatvf(char ** const restrict dst, size_t * const restrict dstlp, int dirfd, const char * const restrict path_fmt, va_list va)
{
  enter;

  char path[512];
  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(usnarfats, dst, dstlp, dirfd, path);

finally:
  va_end(va);
coda;
}
