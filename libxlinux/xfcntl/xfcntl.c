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

#include "fmt.h"

API xapi xopens(int * fd, int flags, const char * path)
{
  xproxy(xopenats, fd, flags, AT_FDCWD, path);
}

API xapi xopenf(int * fd, int flags, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopenatvf, fd, flags, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopenvf(int * fd, int flags, const char * path_fmt, va_list va)
{
  xproxy(xopenatvf, fd, flags, AT_FDCWD, path_fmt, va);
}

API xapi xopenats(int * fd, int flags, int dirfd, const char * path)
{
  enter;

  if(fd && (*fd = openat(dirfd, path, flags)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!fd && openat(dirfd, path, flags) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("dirfd", "%d", dirfd);
  xapi_infof("path", "%s", path);
coda;
}

API xapi xopenatf(int * fd, int flags, int dirfd, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopenatvf, fd, flags, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopenatvf(int * fd, int flags, int dirfd, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xopenats, fd, flags, dirfd, path);

  finally : coda;
}

API xapi uxopens(int * fd, int flags, const char * path)
{
  xproxy(uxopenats, fd, flags, AT_FDCWD, path);
}

API xapi uxopenf(int * fd, int flags, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);
  fatal(uxopenatvf, fd, flags, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopenvf(int * fd, int flags, const char * path_fmt, va_list va)
{
  xproxy(uxopenatvf, fd, flags, AT_FDCWD, path_fmt, va);
}

API xapi uxopenats(int * fd, int flags, int dirfd, const char * path)
{
  enter;

  if((*fd = openat(dirfd, path, flags)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST)
      tfail(perrtab_KERNEL, errno);

    *fd = -1;
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxopenatf(int * fd, int flags, int dirfd, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);
  fatal(uxopenatvf, fd, flags, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopenatvf(int * fd, int flags, int dirfd, const char * path_fmt, va_list va)
{
  enter;

  char path[512];
  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxopenats, fd, flags, dirfd, path);

finally:
  va_end(va);
coda;
}

API xapi xopen_modes(int * fd, int flags, mode_t mode, const char * path)
{
  xproxy(xopenat_modes, fd, flags, mode, AT_FDCWD, path);
}

API xapi xopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopenat_modevf, fd, flags, mode, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
{
  xproxy(xopenat_modevf, fd, flags, mode, AT_FDCWD, path_fmt, va);
}

API xapi xopenat_modes(int * fd, int flags, mode_t mode, int dirfd, const char * path)
{
  enter;

  if(fd && (*fd = openat(dirfd, path, flags, mode)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!fd && openat(dirfd, path, flags, mode) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xopenat_modef(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(xopenat_modevf, fd, flags, mode, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xopenat_modevf(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(xopenat_modes, fd, flags, mode, dirfd, path);

finally:
  va_end(va);
coda;
}

API xapi uxopen_modes(int * fd, int flags, mode_t mode, const char * path)
{
  xproxy(uxopenat_modes, fd, flags, mode, AT_FDCWD, path);
}

API xapi uxopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxopenat_modevf, fd, flags, mode, AT_FDCWD, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
{
  xproxy(uxopenat_modevf, fd, flags, mode, AT_FDCWD, path_fmt, va);
}

API xapi uxopenat_modes(int * fd, int flags, mode_t mode, int dirfd, const char * path)
{
  enter;

  if((*fd = openat(dirfd, path, flags, mode)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST)
      tfail(perrtab_KERNEL, errno);

    *fd = -1;
  }

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxopenat_modef(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, path_fmt);

  fatal(uxopenat_modevf, fd, flags, mode, dirfd, path_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxopenat_modevf(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
{
  enter;

  char path[512];

  fatal(fmt_apply, path, sizeof(path), path_fmt, va);
  fatal(uxopenat_modes, fd, flags, mode, dirfd, path);

finally:
  va_end(va);
coda;
}
