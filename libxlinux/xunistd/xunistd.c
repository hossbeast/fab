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

#include "types.h"
#include "macros.h"

#include "xunistd/xunistd.h"
#include "common/fmt.h"

//
// api
//

ssize_t API xread(int fd, void * buf, size_t count)
{
  ssize_t rv;

  rv = read(fd, buf, count);
  RUNTIME_ASSERT(rv != -1);

  return rv;
}

ssize_t API uxread(int fd, void * buf, size_t count)
{
  ssize_t rv;

  rv = read(fd, buf, count);
  if(rv == -1)
  {
    RUNTIME_ASSERT(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
    rv = 0;
  }

  return rv;
}

void API axread(int fd, void * restrict buf, size_t count)
{
  size_t actual;
  size_t cur;

  actual = 0;
  while(count - actual)
  {
    cur = read(fd, ((char*)buf) + actual, count - actual);
    RUNTIME_ASSERT(cur > 0);
    actual += cur;
  }
}

void API axwrite(int fd, const void * buf, size_t count)
{
  size_t actual;
  size_t cur;

  actual = 0;
  while(count - actual)
  {
    cur = write(fd, ((char*)buf) + actual, count - actual);
    RUNTIME_ASSERT(cur > 0);
    actual += cur;
  }
}

char * API xgetcwd(char * buf, size_t size)
{
  char * res;

  res = getcwd(buf, size);
  RUNTIME_ASSERT(res);

  return res;
}

off_t API xlseek(int fd, off_t offset, int whence)
{
  off_t res;

  res = lseek(fd, offset, whence);
  RUNTIME_ASSERT(res != (off_t)-1);

  return res;
}

void API xclose(int fd)
{
  int rv;

  if(fd == -1) {
    return;
  }

  rv = close(fd);
  RUNTIME_ASSERT(rv == 0);
}

void API ixclose(int * fd)
{
  xclose(*fd);
  *fd = -1;
}

void API xsymlinks(const char * target, const char * linkpath)
{
  int rv;

  rv = symlink(target, linkpath);

  RUNTIME_ASSERT(rv == 0);
}

void API xsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  va_list va;

  va_start(va, linkpath_fmt);
  xsymlinkvf(target_fmt, linkpath_fmt, va);
  va_end(va);
}

void API xsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  char target[512];
  char linkpath[512];

  fmt_apply(target, sizeof(target), target_fmt, va);
  fmt_apply(linkpath, sizeof(linkpath), linkpath_fmt, va);

  return xsymlinks(target, linkpath);
}

int API uxsymlinks(const char * target, const char * linkpath)
{
  int rv;

  rv = symlink(target, linkpath);
  RUNTIME_ASSERT(rv == 0 || errno == EEXIST);

  return rv;
}

int API uxsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  int rv;
  va_list va;

  va_start(va, linkpath_fmt);
  rv = uxsymlinkvf(target_fmt, linkpath_fmt, va);
  va_end(va);

  return rv;
}

int API uxsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  char target[512];
  char linkpath[512];

  fmt_apply(target, sizeof(target), target_fmt, va);
  fmt_apply(linkpath, sizeof(linkpath), linkpath_fmt, va);

  return uxsymlinks(target, linkpath);
}

void API xunlinks(const char * paths)
{
  RUNTIME_ASSERT(unlink(paths) == 0);
}

void API xunlinkvf(const char * fmt, va_list va)
{
  char space[512];

  fmt_apply(space, sizeof(space), fmt, va);
  xunlinks(space);
}

void API xunlinkf(const char * fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  xunlinkvf(fmt, va);
}

int API uxunlinks(const char * const restrict path)
{
  int rv;

  rv = unlink(path);
  RUNTIME_ASSERT(rv == 0 || errno == ENOENT);

  return rv;
}

int API uxunlinkf(const char * const restrict fmt, ...)
{
  int rv;
  va_list va;

  va_start(va, fmt);
  rv = uxunlinkvf(fmt, va);
  va_end(va);

  return rv;
}

int API uxunlinkvf(const char * const restrict fmt, va_list va)
{
  char space[512];

  fmt_apply(space, sizeof(space), fmt, va);
  return uxunlinks(space);
}

pid_t API xfork(void)
{
  pid_t rv;

  rv = fork();
  RUNTIME_ASSERT(rv != -1);

  return rv;
}

void API xdup(int oldfd)
{
  RUNTIME_ASSERT(dup(oldfd) != -1);
}

void API xdup2(int oldfd, int newfd)
{
  RUNTIME_ASSERT(dup2(oldfd, newfd) != -1);
}

void API xgetresuid(uid_t * const ruid, uid_t * const euid, uid_t * const suid)
{
  RUNTIME_ASSERT(getresuid(ruid, euid, suid) == 0);
}

void API xgetresgid(gid_t * const rgid, gid_t * const egid, gid_t * const sgid)
{
  RUNTIME_ASSERT(getresgid(rgid, egid, sgid) == 0);
}

void API xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
  RUNTIME_ASSERT(setresuid(ruid, euid, suid) == 0);
}

void API xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
  RUNTIME_ASSERT(setresgid(rgid, egid, sgid) == 0);
}

void API xseteuid(uid_t euid)
{
  RUNTIME_ASSERT(seteuid(euid) == 0);
}

void API xsetegid(gid_t egid)
{
  RUNTIME_ASSERT(setegid(egid) == 0);
}

void API xtruncate(const char * path, off_t length)
{
  RUNTIME_ASSERT(truncate(path, length) == 0);
}

void API xftruncate(int fd, off_t length)
{
  RUNTIME_ASSERT(ftruncate(fd, length));
}

void API xrmdir(const char * pathname)
{
  RUNTIME_ASSERT(rmdir(pathname) == 0);
}

void API xsetpgid(pid_t pid, pid_t pgid)
{
  RUNTIME_ASSERT(setpgid(pid, pgid));
}

void API xsetsid()
{
  RUNTIME_ASSERT(setsid() != -1);
}

void API xexecv(const char * path, char * const argv[])
{
  execv(path, argv);
  RUNTIME_ABORT();
}

void API xexecve(const char * path, char * const argv[], char * const envp[])
{
  execve(path, argv, envp);
  RUNTIME_ABORT();
}

void API xexecvp(const char * file, char * const argv[])
{
  execvp(file, argv);
  RUNTIME_ABORT();
}

void API xfexecve(int fd, char * const argv[], char * const envp[])
{
  fexecve(fd, argv, envp);
}

void API xchdirs(const char * const restrict path)
{
  RUNTIME_ASSERT(chdir(path) == 0);
}

void API xchdirf(const char * const restrict fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  xchdirvf(fmt, va);
  va_end(va);
}

void API xchdirvf(const char * const restrict fmt, va_list va)
{
  char space[512];

  fmt_apply(space, sizeof(space), fmt, va);
  xchdirs(space);
}

void API xfchdir(int fd)
{
  RUNTIME_ASSERT(fchdir(fd) == 0);
}

pid_t API gettid()
{
  static __thread pid_t self;

  if(!self)
    self = syscall(SYS_gettid);

  return self;
}

void API xpipe(int filedes[2])
{
  RUNTIME_ASSERT(pipe(filedes) == 0);
}
