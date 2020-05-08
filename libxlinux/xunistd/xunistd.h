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

#ifndef _XUNISTD_H
#define _XUNISTD_H

#include <unistd.h>
#include <stdarg.h>

#include "xapi.h"
#include "types.h"

/// xread
//
// SUMMARY
//  proxy for read
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
xapi xread(int fd, void * restrict buf, size_t count, ssize_t * restrict bytes)
  __attribute__((nonnull(2)));

/// uxread
//
// SUMMARY
//  proxy for read that only fails when errno not in { EAGAIN, EINTR }
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
// REMARKS
//  using uxread with an fd not marked with O_NONBLOCK is probably an error
//
xapi uxread(int fd, void * restrict buf, size_t count, ssize_t * restrict bytes)
  __attribute__((nonnull(2)));

/// aread
//
// SUMMARY
//  proxy for read that also fails when count != actual
//
// PARAMETERS
//
int aread(int fd, void * restrict buf, size_t count)
  __attribute__((nonnull));

xapi axread(int fd, void * restrict buf, size_t count)
  __attribute__((nonnull));

/// xwrite
//
// SUMMARY
//  proxy for write
//
// PARAMETERS
//  [bytes] - returns the number of bytes written
//
xapi xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
  __attribute__((nonnull(2)));

/// awrite
//
// SUMMARY
//  proxy for write that also fails when count != actual
//
// PARAMETERS
//
int awrite(int fd, const void * buf, size_t count)
  __attribute__((nonnull));

xapi axwrite(int fd, const void * buf, size_t count)
  __attribute__((nonnull));

/// xgetcwd
//
// SUMMARY
//  proxy for getcwd
//
// PARAMETERS
//  [res] - *res is set to the return from getcwd
//
xapi xgetcwd(char * buf, size_t size, char ** res);

/// xlseek
//
// SUMMARY
//  proxy for lseek
//
// PARAMETERS
//  [res] - *res is set to the return from lseek
//
xapi xlseek(int fd, off_t offset, int whence, off_t * res);

/// xclose
//
// SUMMARY
//  proxy for close
//
xapi xclose(int fd);

/// ixclose
//
// SUMMARY
//  idempotent proxy for close
//
xapi ixclose(int * const restrict fd)
  __attribute__((nonnull));

/// xsymlink
//
// SUMMARY
//  proxy for symlink
//
// VARIANTS
//  s/f/vf - different ways to pass parameters
//
xapi xsymlinks(const char * restrict target, const char * restrict linkpath)
  __attribute__((nonnull));

xapi xsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi xsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));

/// uxsymlink
//
// SUMMARY
//  proxy for symlink which only fails when errno != EEXIST
//
// VARIANTS
//  s/f/vf - different ways to pass
//
xapi uxsymlinks(const char * restrict target, const char * restrict linkpath)
  __attribute__((nonnull));

xapi uxsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi uxsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));

/// xunlink
//
// SUMMARY
//  proxy for unlink
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xunlinks(const char * restrict pathname)
  __attribute__((nonnull));

xapi xunlinkf(const char * restrict fmt, ...)
  __attribute__((nonnull(1)));

xapi xunlinkvf(const char * restrict fmt, va_list va)
  __attribute__((nonnull));

/// uxunlink
//
// SUMMARY
//  proxy for unlink that fails only when errno != ENOENT
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxunlinks(const char * const restrict pathname)
  __attribute__((nonnull));

xapi uxunlinkf(const char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

xapi uxunlinkvf(const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// xfork
//
// SUMMARY
//  proxy for fork
//
xapi xfork(pid_t * const restrict pid);

/// xdup
//
// SUMMARY
//  proxy for dup
//
xapi xdup(int oldfd);

/// xdup2
//
// SUMMARY
//  proxy for dup2
//
xapi xdup2(int oldfd, int newfd);

/// xsetresuid
//
// SUMMARY
//  proxy for setresuid
//
xapi xsetresuid(uid_t ruid, uid_t euid, uid_t suid);

/// xsetresgid
//
// SUMMARY
//  proxy for setresgid
//
xapi xsetresgid(gid_t rgid, gid_t egid, gid_t sgid);

/// xeuidaccess
//
// SUMMARY
//  proxy for euidaccess
//
xapi xeuidaccesss(int * restrict r, int mode, const char * restrict path)
  __attribute__((nonnull(3)));

xapi xeuidaccessf(int * restrict r, int mode, const char * restrict path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

xapi xeuidaccessvf(int * restrict r, int mode, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// uxeuidaccess
//
// SUMMARY
//  proxy for euidaccess that only fails when errno !: { EACCES, ENOENT, ENOTDIR }
//
xapi uxeuidaccesss(int * restrict r, int mode, const char * restrict path)
  __attribute__((nonnull(3)));

xapi uxeuidaccessf(int * restrict r, int mode, const char * restrict path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

xapi uxeuidaccessvf(int * restrict r, int mode, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xseteuid
//
// SUMMARY
//  proxy for seteuid
//
xapi xseteuid(uid_t euid);

/// xsetegid
//
// SUMMARY
//  proxy for setegid
//
xapi xsetegid(gid_t egid);

/// xgetresuid
//
// SUMMARY
//  proxy for getresuid
//
xapi xgetresuid(uid_t * const restrict ruid, uid_t * const restrict euid, uid_t * const restrict suid)
  __attribute__((nonnull));

/// xgetresgid
//
// SUMMARY
//  proxy for getresgid
//
xapi xgetresgid(gid_t * const restrict rgid, gid_t * const restrict egid, gid_t * const restrict sgid)
  __attribute__((nonnull));

/// xtruncate
//
// SUMMARY
//  proxy for truncate
//
xapi xtruncate(const char * restrict path, off_t length)
  __attribute__((nonnull));

/// xftruncate
//
// SUMMARY
//  proxy for ftruncate
//
xapi xftruncate(int fd, off_t length);

/// xrmdir
//
// SUMMARY
//  proxy for rmdir
//
xapi xrmdir(const char * restrict pathname)
  __attribute__((nonnull));

/// xsetpgid
//
// SUMMARY
//  proxy for setpgid
//
xapi xsetpgid(pid_t pid, pid_t pgid);

/// xsetsid
//
// SUMMARY
//  proxy for setsid
//
xapi xsetsid(void);

/// xexecv
//
// SUMMARY
//  xapi proxy for execv
//
xapi xexecv(const char * path, char * const argv[])
  __attribute__((nonnull));

/// xexecve
//
// SUMMARY
//  xapi proxy for execvpe
//
xapi xexecve(const char * path, char * const argv[], char * const envp[])
  __attribute__((nonnull));

/// xexecvp
//
// SUMMARY
//  xapi proxy for execvp
//
xapi xexecvp(const char * file, char * const argv[])
  __attribute__((nonnull));

/// xchdir
//
// SUMMARY
//  xapi proxy for chdir
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xchdirs(const char * const restrict path)
  __attribute__((nonnull));

xapi xchdirf(const char * const restrict fmt, ...)
  __attribute__((nonnull));

xapi xchdirvf(const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// xfchdir
//
// SUMMARY
//  xapi proxy for fchdir
//
xapi xfchdir(int fd);

/// gettid
//
// SUMMARY
//  get the id of the current thread
//
pid_t gettid(void);

/// xpipe
//
// SUMMARY
//  xapi proxy for pipe
//
xapi xpipe(int filedes[2])
  __attribute__((nonnull));

#endif
