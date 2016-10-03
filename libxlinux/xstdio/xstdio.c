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
#include <stdarg.h>
#include <errno.h>

#include "internal.h"
#include "xstdio/xstdio.h"
#include "errtab/KERNEL.errtab.h"

#define restrict __restrict

API xapi xvdprintf(int fd, const char * const restrict fmt, va_list va)
{
  enter;

  if(vdprintf(fd, fmt, va) < 0)
    fail(errno);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}
