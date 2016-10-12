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
  dstl--;

  if(dstlp)
    *dstlp = dstl;

  finally : coda;
}

xapi usnarfs(const char * const restrict path, char ** const restrict dst, size_t * const restrict dstlp)
{
  enter;

  int fd = -1;
  fatal(uxopen, path, O_RDONLY, &fd);
  if(fd != -1)
    fatal(snarf, fd, dst, dstlp);

finally:
  fatal(ixclose, &fd);
coda;
}

xapi snarfs(const char * const restrict path, char ** const restrict dst, size_t * const restrict dstlp)
{
  enter;

  int fd = -1;
  fatal(xopen, path, O_RDONLY, &fd);
  fatal(snarf, fd, dst, dstlp);

finally:
  fatal(ixclose, &fd);
coda;
}
