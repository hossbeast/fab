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

#include <errno.h>

#include "internal.h"
#include "xfcntl/xfcntl.h"
#include "errtab/KERNEL.errtab.h"

#include "fmt.internal.h"

API xapi xopens(int * fd, int flags, const char * path)
{
  enter;

  if(fd && (*fd = open(path, flags)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!fd && open(path, flags) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xopenf(int * fd, int flags, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopenvf, fd, flags, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopenvf(int * fd, int flags, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xopens, fd, flags, path);

  finally : coda;
}

API xapi uxopens(int * fd, int flags, const char * path)
{
  enter;

  if((*fd = open(path, flags)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST)
      tfail(perrtab_KERNEL, errno);

    *fd = -1;
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxopenf(int * fd, int flags, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);
  fatal(uxopenvf, fd, flags, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopenvf(int * fd, int flags, const char * path_fmt, va_list va)
{
  enter;

  char path[512];
  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxopens, fd, flags, path);

finally:
  va_end(va);
coda;
}

API xapi xopen_modes(int * fd, int flags, mode_t mode, const char * path)
{
  enter;

  if(fd && (*fd = open(path, flags, mode)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!fd && open(path, flags, mode) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopen_modevf, fd, flags, mode, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xopen_modes, fd, flags, mode, path);

finally:
  va_end(va);
coda;
}

API xapi uxopen_modes(int * fd, int flags, mode_t mode, const char * path)
{
  enter;

  if((*fd = open(path, flags, mode)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST)
      tfail(perrtab_KERNEL, errno);

    *fd = -1;
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxopen_modevf, fd, flags, mode, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxopen_modes, fd, flags, mode, path);

finally:
  va_end(va);
coda;
}
