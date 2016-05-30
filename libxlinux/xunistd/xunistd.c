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
#include <stdio.h>

#include "internal.h"
#include "xunistd/xunistd.h"
#include "errtab/SYS.errtab.h"
#include "errtab/XLINUX.errtab.h"

API xapi xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && ((*bytes) = read(fd, buf, count)) == -1)
    fail(errno);

  else if(!bytes && read(fd, buf, count) == -1)
    fail(errno);

  finally : coda;
}

API xapi uxread(int fd, void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && ((*bytes) = read(fd, buf, count)) == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
  {
    fail(errno);
  }

  else if(!bytes && read(fd, buf, count) == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
  {
    fail(errno);
  }

  finally : coda;
}

API xapi axread(int fd, void * buf, size_t count)
{
  enter;

  size_t actual = 0;
  while(count - actual)
  {
    ssize_t cur = read(fd, ((char*)buf) + actual, count - actual);

    if(cur == 0)
    {
      xapi_fail_intent();
      xapi_info_addf("expected", "%zu", count);
      xapi_info_addf("actual", "%zd", actual);
      tfail(perrtab_XLINUX, XLINUX_LESS);
    }
    else if(cur == -1)
      fail(errno);

    actual += cur;
  }

  finally : coda;
}

API xapi xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && (*bytes = write(fd, buf, count)) == -1)
    fail(errno);

  else if(!bytes && write(fd, buf, count) == -1)
    fail(errno);

  finally : coda;
}

API xapi axwrite(int fd, const void * buf, size_t count)
{
  enter;

  size_t actual = 0;
  while(count - actual)
  {
    ssize_t cur = write(fd, ((char*)buf) + actual, count - actual);

    if(cur == 0)
    {
      xapi_fail_intent();
      xapi_info_addf("expected", "%zu", count);
      xapi_info_addf("actual", "%zd", actual);
      tfail(perrtab_XLINUX, XLINUX_LESS);
    }
    else if(cur == -1)
      fail(errno);

    actual += cur;
  }

  finally : coda;
}

API xapi xgetcwd(char * buf, size_t size, char ** res)
{
  enter;

  if(res && (((*res) = getcwd(buf, size)) == 0))
    fail(errno);

  else if(!res && getcwd(buf, size) == 0)
    fail(errno);

  finally : coda;
}

API xapi xlseek(int fd, off_t offset, int whence, off_t * res)
{
  enter;

  if(res && ((*res) = lseek(fd, offset, whence)) == (off_t)-1)
    fail(errno);

  else if(!res && lseek(fd, offset, whence) == (off_t)-1)
    fail(errno);

  finally : coda;
}

API xapi xclose(int fd)
{
  enter;

  fatalize(errno, close, fd);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}

API xapi ixclose(int * fd)
{
  enter;

  if(*fd != -1)
  {
    fatalize(errno, close, *fd);
    *fd = -1;
  }

finally:
  xapi_infof("fd", "%d", *fd);
coda;
}

API xapi xsymlink(const char * target, const char * linkpath)
{
  enter;

  fatalize(errno, symlink, target, linkpath);

finally:
  xapi_infof("path", "%s", linkpath);
coda;
}

API xapi uxsymlink(const char * target, const char * linkpath)
{
  enter;

  if(symlink(target, linkpath) != 0 && errno != EEXIST)
    fail(errno);

  finally : coda;
}

API xapi xunlink(const char * pathname)
{
  xproxy(xunlinks, pathname);
}

API xapi xunlinks(const char * paths)
{
  enter;

  if(unlink(paths) != 0)
    fail(errno);

finally:
  xapi_infof("path", "%s", paths);
coda;
}

API xapi xunlinkvf(const char * fmt, va_list va)
{
  enter;

  char space[512];
  size_t sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz >= sizeof(space))
  {
    xapi_fail_intent();
    xapi_info_addf("max size", "%zu", sizeof(space));
    xapi_info_addf("actual size", "%zu", sz);
    tfail(perrtab_XLINUX, XLINUX_NAMETOOLONG);
  }

  fatal(xunlinks, space);

  finally : coda;
}

API xapi xunlinkf(const char * fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xunlinkvf, fmt, va);

  finally : coda;
}

API xapi uxunlink(const char * const restrict path)
{
  enter;

  if(unlink(path) != 0 && errno != ENOENT)
    fail(errno);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi uxunlinks(const char * const restrict path)
{
  xproxy(uxunlink, path);
}

API xapi uxunlinkf(const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(uxunlinkvf, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxunlinkvf(const char * const restrict fmt, va_list va)
{
  enter;

  char space[512];
  size_t sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz >= sizeof(space))
  {
    xapi_fail_intent();
    xapi_info_addf("max size", "%zu", sizeof(space));
    xapi_info_addf("actual size", "%zu", sz);
    tfail(perrtab_XLINUX, XLINUX_NAMETOOLONG);
  }

  fatal(uxunlinks, space);

  finally : coda;
}

API xapi xfork(pid_t * r)
{
  enter;

  if(r && (((*r) = fork()) == -1))
    fail(errno);
  
  else if(!r && fork() == -1)
    fail(errno);

  finally : coda;
}

API xapi xdup(int oldfd)
{
  enter;

  if(dup(oldfd) == -1)
    fail(errno);

  finally : coda;
}

API xapi xdup2(int oldfd, int newfd)
{
  enter;

  if(dup2(oldfd, newfd) == -1)
    fail(errno);

  finally : coda;
}

API xapi xgetresuid(uid_t * const ruid, uid_t * const euid, uid_t * const suid)
{
  enter;

  fatalize(errno, getresuid, ruid, euid, suid);

  finally : coda;
}

API xapi xgetresgid(gid_t * const rgid, gid_t * const egid, gid_t * const sgid)
{
  enter;

  fatalize(errno, getresgid, rgid, egid, sgid);

  finally : coda;
}

API xapi xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
  enter;

  fatalize(errno, setresuid, ruid, euid, suid);

  finally : coda;
}

API xapi xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
  enter;

  fatalize(errno, setresgid, rgid, egid, sgid);

  finally : coda;
}

API xapi xeuidaccess(const char * pathname, int mode, int * const r)
{
  enter;

  if((r && ((*r) = euidaccess(pathname, mode)) == -1) || (!r && euidaccess(pathname, mode) == -1))
    fail(errno);

finally:
  xapi_infos("path", pathname);
coda;
}  

API xapi uxeuidaccess(const char * pathname, int mode, int * const r)
{
  enter;

  if(r && ((*r) = euidaccess(pathname, mode)) == -1 && errno != EACCES && errno != ENOENT && errno != ENOTDIR)
    fail(errno);
    
  else if(!r && euidaccess(pathname, mode) == -1 && errno != EACCES && errno != ENOENT && errno != ENOTDIR)
    fail(errno);

finally:
  xapi_infos("path", pathname);
coda;
}  

API xapi xseteuid(uid_t euid)
{
  enter;

  fatalize(errno, seteuid, euid);

finally:
  xapi_infof("euid", "%d", euid);
coda;
}

API xapi xsetegid(gid_t egid)
{
  enter;

  fatalize(errno, setegid, egid);

finally:
  xapi_infof("egid", "%d", egid);
coda;
}

API xapi xtruncate(const char * path, off_t length)
{
  enter;

  fatalize(errno, truncate, path, length);

finally :
  xapi_infos("path", path);
coda;
}

API xapi xftruncate(int fd, off_t length)
{
  enter;

  fatalize(errno, ftruncate, fd, length);

  finally : coda;
}

API xapi xrmdir(const char * pathname)
{
  enter;

  fatalize(errno, rmdir, pathname);

finally:
  xapi_infos("path", pathname);
coda;
}

API xapi xsetpgid(pid_t pid, pid_t pgid)
{
  enter;

  fatalize(errno, setpgid, pid, pgid);

  finally : coda;
}

API xapi xsetsid()
{
  enter;

  if(setsid() == -1)
    fail(errno);

  finally : coda;
}

API xapi xexecv(const char * path, char * const argv[])
{
  enter;

  fatalize(errno, execv, path, argv);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xchdir(const char * const restrict path)
{
  enter;

  fatalize(errno, chdir, path);

finally:
  xapi_infof("path", "%s", path);
coda;
}

API xapi xchdirs(const char * const restrict path)
{
  xproxy(xchdir, path);
}

API xapi xchdirf(const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xchdirvf, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xchdirvf(const char * const restrict fmt, va_list va)
{
  enter;

  char space[512];
  size_t sz = vsnprintf(space, sizeof(space), fmt, va);
  if(sz >= sizeof(space))
  {
    xapi_fail_intent();
    xapi_info_addf("max size", "%zu", sizeof(space));
    xapi_info_addf("actual size", "%zu", sz);
    tfail(perrtab_XLINUX, XLINUX_NAMETOOLONG);
  }

  fatal(xchdirs, space);

  finally : coda;
}

API xapi xfchdir(int fd)
{
  enter;

  fatalize(errno, fchdir, fd);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}
