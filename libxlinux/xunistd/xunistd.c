/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>

#include "types.h"
#include "macros.h"

#include "xunistd.h"
#include "fmt.h"

/* pidfd_open was added in linux 5.3 but your libc might not have exported the syscall num */
#ifdef SYS_pidfd_open
# define pidfd_syscall_num SYS_pidfd_open
#else
# define pidfd_syscall_num 434
#endif

//
// api
//

ssize_t xread(int fd, void * buf, size_t count)
{
  ssize_t rv;

  rv = read(fd, buf, count);
  RUNTIME_ASSERT(rv != -1);

  return rv;
}

ssize_t uxread(int fd, void * buf, size_t count)
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

void axread(int fd, void * restrict buf, size_t count)
{
  size_t actual;
  ssize_t cur;

  actual = 0;
  while(count - actual)
  {
    cur = read(fd, ((char*)buf) + actual, count - actual);
    RUNTIME_ASSERT(cur > 0);
    actual += cur;
  }
}

void axwrite(int fd, const void * buf, size_t count)
{
  size_t actual;
  ssize_t r;

  actual = 0;
  while(count - actual)
  {
    r = write(fd, ((char*)buf) + actual, count - actual);
    if(r == 0) {
      break;
    }
    RUNTIME_ASSERT(r > 0);
    actual += r;
  }
}

char * xgetcwd(char * buf, size_t size)
{
  char * res;

  res = getcwd(buf, size);
  RUNTIME_ASSERT(res);

  return res;
}

off_t xlseek(int fd, off_t offset, int whence)
{
  off_t res;

  res = lseek(fd, offset, whence);
  RUNTIME_ASSERT(res != (off_t)-1);

  return res;
}

void xclose(int fd)
{
  int rv;

  if(fd == -1) {
    return;
  }

  rv = close(fd);
  RUNTIME_ASSERT(rv == 0);
}

void ixclose(int * fd)
{
  xclose(*fd);
  *fd = -1;
}

void xsymlinks(const char * target, const char * linkpath)
{
  int rv;

  rv = symlink(target, linkpath);

  RUNTIME_ASSERT(rv == 0);
}

void xsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  va_list va;

  va_start(va, linkpath_fmt);
  xsymlinkvf(target_fmt, linkpath_fmt, va);
  va_end(va);
}

void xsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  char target[PATH_MAX];
  char linkpath[PATH_MAX];

  fmt_apply(target, sizeof(target), target_fmt, va);
  fmt_apply(linkpath, sizeof(linkpath), linkpath_fmt, va);

  return xsymlinks(target, linkpath);
}

int uxsymlinks(const char * target, const char * linkpath)
{
  int rv;

  rv = symlink(target, linkpath);
  RUNTIME_ASSERT(rv == 0 || errno == EEXIST);

  return rv;
}

int uxsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
{
  int rv;
  va_list va;

  va_start(va, linkpath_fmt);
  rv = uxsymlinkvf(target_fmt, linkpath_fmt, va);
  va_end(va);

  return rv;
}

int uxsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
{
  char target[PATH_MAX];
  char linkpath[PATH_MAX];

  fmt_apply(target, sizeof(target), target_fmt, va);
  fmt_apply(linkpath, sizeof(linkpath), linkpath_fmt, va);

  return uxsymlinks(target, linkpath);
}

void xunlinks(const char * paths)
{
  RUNTIME_ASSERT(unlink(paths) == 0);
}

void xunlinkvf(const char * fmt, va_list va)
{
  char space[PATH_MAX];

  fmt_apply(space, sizeof(space), fmt, va);
  xunlinks(space);
}

void xunlinkf(const char * fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  xunlinkvf(fmt, va);
}

int uxunlinks(const char * const restrict path)
{
  int rv;

  rv = unlink(path);
  RUNTIME_ASSERT(rv == 0 || errno == ENOENT);

  return rv;
}

int uxunlinkf(const char * const restrict fmt, ...)
{
  int rv;
  va_list va;

  va_start(va, fmt);
  rv = uxunlinkvf(fmt, va);
  va_end(va);

  return rv;
}

int uxunlinkvf(const char * const restrict fmt, va_list va)
{
  char space[PATH_MAX];

  fmt_apply(space, sizeof(space), fmt, va);
  return uxunlinks(space);
}

pid_t xfork(void)
{
  pid_t rv;

  rv = fork();
  RUNTIME_ASSERT(rv != -1);

  return rv;
}

void xdup(int oldfd)
{
  RUNTIME_ASSERT(dup(oldfd) != -1);
}

void xdup2(int oldfd, int newfd)
{
  RUNTIME_ASSERT(dup2(oldfd, newfd) != -1);
}

void xgetresuid(uid_t * const ruid, uid_t * const euid, uid_t * const suid)
{
  RUNTIME_ASSERT(getresuid(ruid, euid, suid) == 0);
}

void xgetresgid(gid_t * const rgid, gid_t * const egid, gid_t * const sgid)
{
  RUNTIME_ASSERT(getresgid(rgid, egid, sgid) == 0);
}

void xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
  RUNTIME_ASSERT(setresuid(ruid, euid, suid) == 0);
}

void xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
  RUNTIME_ASSERT(setresgid(rgid, egid, sgid) == 0);
}

void xseteuid(uid_t euid)
{
  RUNTIME_ASSERT(seteuid(euid) == 0);
}

void xsetegid(gid_t egid)
{
  RUNTIME_ASSERT(setegid(egid) == 0);
}

void xtruncate(const char * path, off_t length)
{
  RUNTIME_ASSERT(truncate(path, length) == 0);
}

void xftruncate(int fd, off_t length)
{
  int r;
  r = ftruncate(fd, length);
  RUNTIME_ASSERT(r == 0);
}

void xrmdir(const char * pathname)
{
  RUNTIME_ASSERT(rmdir(pathname) == 0);
}

void xsetpgid(pid_t pid, pid_t pgid)
{
  RUNTIME_ASSERT(setpgid(pid, pgid));
}

void xsetsid()
{
  RUNTIME_ASSERT(setsid() != -1);
}

void xexecv(const char * path, char * const argv[])
{
  execv(path, argv);
  RTA;
  exit(1);
}

void xexecve(const char * path, char * const argv[], char * const envp[])
{
  execve(path, argv, envp);
  RTA;
  exit(1);
}

void xexecvp(const char * file, char * const argv[])
{
  execvp(file, argv);
  RTA;
  exit(1);
}

void xfexecve(int fd, char * const argv[], char * const envp[])
{
  fexecve(fd, argv, envp);
  RTA;
  exit(1);
}

void xchdirs(const char * const restrict path)
{
  RUNTIME_ASSERT(chdir(path) == 0);
}

void xchdirf(const char * const restrict fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  xchdirvf(fmt, va);
  va_end(va);
}

void xchdirvf(const char * const restrict fmt, va_list va)
{
  char space[PATH_MAX];

  fmt_apply(space, sizeof(space), fmt, va);
  xchdirs(space);
}

void xfchdir(int fd)
{
  RUNTIME_ASSERT(fchdir(fd) == 0);
}

pid_t gettid()
{
  pid_t self;

  self = syscall(SYS_gettid);

  return self;
}

void xpipe(int filedes[2])
{
  RUNTIME_ASSERT(pipe(filedes) == 0);
}

int uxeuidaccesss(const char * restrict path, int mode)
{
  int r;

  if((r = euidaccess(path, mode)) == -1)
  {
    RUNTIME_ASSERT(errno == EACCES || errno == ENOENT || errno == ENOTDIR);
    r = errno;
  }

  return r;
}

int uxpidfd_open(pid_t pid, unsigned int flags)
{
  int fd;

  if((fd = syscall(pidfd_syscall_num, pid, flags)) == -1) {
    RUNTIME_ASSERT(errno == ESRCH);
  }

  return fd;
}
