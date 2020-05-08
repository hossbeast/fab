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
#include <sys/syscall.h>

#include "xapi.h"
#include "xapi/exit.h"
#include "types.h"

#include "internal.h"
#include "xunistd/xunistd.h"
#include "errtab/KERNEL.errtab.h"
#include "errtab/XLINUX.errtab.h"

#include "fmt.h"

//
// api
//

API xapi xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && ((*bytes) = read(fd, buf, count)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!bytes && read(fd, buf, count) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi uxread(int fd, void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && ((*bytes) = read(fd, buf, count)) == -1)
  {
    if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
    {
      tfail(perrtab_KERNEL, errno);
    }

    *bytes = 0;
  }
  else if(!bytes && read(fd, buf, count) == -1 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}

API int aread(int fd, void * restrict buf, size_t count)
{
  size_t actual = 0;
  while(count - actual)
  {
    ssize_t cur = read(fd, ((char*)buf) + actual, count - actual);
    if(cur == 0)
    {
      return XLINUX_LESS;
    }
    else if(cur == -1)
    {
      return xapi_exit_synth(perrtab_KERNEL, errno);
    }

    actual += cur;
  }

  return actual;
}

API xapi axread(int fd, void * restrict buf, size_t count)
{
  enter;

  size_t actual = 0;
  while(count - actual)
  {
    ssize_t cur = read(fd, ((char*)buf) + actual, count - actual);

    if(cur == 0)
    {
      xapi_info_pushf("expected", "%zu", count);
      xapi_info_pushf("actual", "%zd", actual);
      fail(XLINUX_LESS);
    }
    else if(cur == -1)
      tfail(perrtab_KERNEL, errno);

    actual += cur;
  }

  finally : coda;
}

API xapi xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
{
  enter;

  if(bytes && (*bytes = write(fd, buf, count)) == -1)
    tfail(perrtab_KERNEL, errno);

  else if(!bytes && write(fd, buf, count) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API int awrite(int fd, const void * buf, size_t count)
{
  size_t actual = 0;
  while(count - actual)
  {
    ssize_t cur = write(fd, ((char*)buf) + actual, count - actual);
    if(cur == 0)
    {
      return XLINUX_LESS;
    }
    else if(cur == -1)
    {
      return xapi_exit_synth(perrtab_KERNEL, errno);
    }

    actual += cur;
  }

  return actual;
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
      xapi_info_pushf("expected", "%zu", count);
      xapi_info_pushf("actual", "%zd", actual);
      fail(XLINUX_LESS);
    }
    else if(cur == -1)
      tfail(perrtab_KERNEL, errno);

    actual += cur;
  }

  finally : coda;
}

API xapi xgetcwd(char * buf, size_t size, char ** res)
{
  enter;

  if(res && (((*res) = getcwd(buf, size)) == 0))
    tfail(perrtab_KERNEL, errno);

  else if(!res && getcwd(buf, size) == 0)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xlseek(int fd, off_t offset, int whence, off_t * res)
{
  enter;

  if(res && ((*res) = lseek(fd, offset, whence)) == (off_t)-1)
    tfail(perrtab_KERNEL, errno);

  else if(!res && lseek(fd, offset, whence) == (off_t)-1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xclose(int fd)
{
  enter;

  if(fd != -1)
    tfatalize(perrtab_KERNEL, errno, close, fd);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}

API xapi ixclose(int * fd)
{
  enter;

  fatal(xclose, *fd);
  *fd = -1;

finally:
  xapi_infof("fd", "%d", *fd);
coda;
}

API xapi xsymlinks(const char * target, const char * linkpath)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, symlink, target, linkpath);

finally:
  xapi_infos("target", target);
  xapi_infos("linkpath", linkpath);
coda;
}

API xapi xsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, linkpath_fmt);

  fatal(xsymlinkvf, target_fmt, linkpath_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  enter;

  char target[512];
  char linkpath[512];

  fatal(fmt_apply, target, sizeof(target), target_fmt, va);
  fatal(fmt_apply, linkpath, sizeof(linkpath), linkpath_fmt, va);

  fatal(xsymlinks, target, linkpath);

  finally : coda;
}

API xapi uxsymlinks(const char * target, const char * linkpath)
{
  enter;

  if(symlink(target, linkpath) != 0 && errno != EEXIST)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("target", target);
  xapi_infos("linkpath", linkpath);
coda;
}

API xapi uxsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  enter;

  va_list va;
  va_start(va, linkpath_fmt);

  fatal(uxsymlinkvf, target_fmt, linkpath_fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  enter;

  char target[512];
  char linkpath[512];

  fatal(fmt_apply, target, sizeof(target), target_fmt, va);
  fatal(fmt_apply, linkpath, sizeof(linkpath), linkpath_fmt, va);

  fatal(uxsymlinks, target, linkpath);

  finally : coda;
}

API xapi xunlinks(const char * paths)
{
  enter;

  if(unlink(paths) != 0)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("path", paths);
coda;
}

API xapi xunlinkvf(const char * fmt, va_list va)
{
  enter;

  char space[512];
  fatal(fmt_apply, space, sizeof(space), fmt, va);
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

API xapi uxunlinks(const char * const restrict path)
{
  enter;

  if(unlink(path) != 0 && errno != ENOENT)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("path", "%s", path);
coda;
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
  fatal(fmt_apply, space, sizeof(space), fmt, va);
  fatal(uxunlinks, space);

  finally : coda;
}

API xapi xfork(pid_t * r)
{
  enter;

  if(r && (((*r) = fork()) == -1))
    tfail(perrtab_KERNEL, errno);

  else if(!r && fork() == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xdup(int oldfd)
{
  enter;

  if(dup(oldfd) == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xdup2(int oldfd, int newfd)
{
  enter;

  if(dup2(oldfd, newfd) == -1)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infof("oldfd", "%d", oldfd);
  xapi_infof("newfd", "%d", newfd);
coda;
}

API xapi xgetresuid(uid_t * const ruid, uid_t * const euid, uid_t * const suid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, getresuid, ruid, euid, suid);

  finally : coda;
}

API xapi xgetresgid(gid_t * const rgid, gid_t * const egid, gid_t * const sgid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, getresgid, rgid, egid, sgid);

  finally : coda;
}

API xapi xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, setresuid, ruid, euid, suid);

  finally : coda;
}

API xapi xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, setresgid, rgid, egid, sgid);

  finally : coda;
}

API xapi xeuidaccesss(int * restrict r, int mode, const char * restrict path)
{
  enter;

  if((r && ((*r) = euidaccess(path, mode)) == -1) || (!r && euidaccess(path, mode) == -1))
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xeuidaccessf(int * restrict r, int mode, const char * restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(xeuidaccessvf, r, mode, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi xeuidaccessvf(int * restrict r, int mode, const char * restrict fmt, va_list va)
{
  enter;

  char space[512];
  fatal(fmt_apply, space, sizeof(space), fmt, va);
  fatal(xeuidaccesss, r, mode, space);

  finally : coda;
}

API xapi uxeuidaccesss(int * restrict r, int mode, const char * restrict path)
{
  enter;

  int lr;
  if(!r)
    r = &lr;

  if(((*r) = euidaccess(path, mode)) == -1 && errno != EACCES && errno != ENOENT && errno != ENOTDIR)
    tfail(perrtab_KERNEL, errno);

finally:
  xapi_infos("path", path);
coda;
}

API xapi uxeuidaccessf(int * restrict r, int mode, const char * restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(uxeuidaccessvf, r, mode, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi uxeuidaccessvf(int * restrict r, int mode, const char * restrict fmt, va_list va)
{
  enter;

  char space[512];
  fatal(fmt_apply, space, sizeof(space), fmt, va);
  fatal(uxeuidaccesss, r, mode, space);

  finally : coda;
}

API xapi xseteuid(uid_t euid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, seteuid, euid);

finally:
  xapi_infof("euid", "%d", euid);
coda;
}

API xapi xsetegid(gid_t egid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, setegid, egid);

finally:
  xapi_infof("egid", "%d", egid);
coda;
}

API xapi xtruncate(const char * path, off_t length)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, truncate, path, length);

finally :
  xapi_infos("path", path);
coda;
}

API xapi xftruncate(int fd, off_t length)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, ftruncate, fd, length);

  finally : coda;
}

API xapi xrmdir(const char * pathname)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, rmdir, pathname);

finally:
  xapi_infos("path", pathname);
coda;
}

API xapi xsetpgid(pid_t pid, pid_t pgid)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, setpgid, pid, pgid);

  finally : coda;
}

API xapi xsetsid()
{
  enter;

  if(setsid() == -1)
    tfail(perrtab_KERNEL, errno);

  finally : coda;
}

API xapi xexecv(const char * path, char * const argv[])
{
  enter;

  tfatalize(perrtab_KERNEL, errno, execv, path, argv);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xexecve(const char * path, char * const argv[], char * const envp[])
{
  enter;

  tfatalize(perrtab_KERNEL, errno, execve, path, argv, envp);

finally:
  xapi_infos("path", path);
coda;
}

API xapi xexecvp(const char * file, char * const argv[])
{
  enter;

  tfatalize(perrtab_KERNEL, errno, execvp, file, argv);

finally:
  xapi_infos("file", file);
coda;
}

API xapi xchdirs(const char * const restrict path)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, chdir, path);

finally:
  xapi_infof("path", "%s", path);
coda;
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
  fatal(fmt_apply, space, sizeof(space), fmt, va);
  fatal(xchdirs, space);

  finally : coda;
}

API xapi xfchdir(int fd)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, fchdir, fd);

finally:
  xapi_infof("fd", "%d", fd);
coda;
}

API pid_t gettid()
{
  static __thread pid_t self;

  if(!self)
    self = syscall(SYS_gettid);

  return self;
}

API xapi xpipe(int filedes[2])
{
  enter;

  tfatalize(perrtab_KERNEL, errno, pipe, filedes);

  finally : coda;
}
