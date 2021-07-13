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

#include "types.h"

/// xread
//
// SUMMARY
//  proxy for read
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
ssize_t xread(int fd, void * restrict buf, size_t count)
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
ssize_t uxread(int fd, void * restrict buf, size_t count)
  __attribute__((nonnull(2)));


/* calls read in a loop until count bytes have been read */
void axread(int fd, void * restrict buf, size_t count)
  __attribute__((nonnull));

/* calls write in a loop until count bytes have been written */
void axwrite(int fd, const void * buf, size_t count)
  __attribute__((nonnull));

/// xgetcwd
//
// SUMMARY
//  proxy for getcwd
//
// PARAMETERS
//  [res] - *res is set to the return from getcwd
//
char * xgetcwd(char * buf, size_t size)
  __attribute__((nonnull));

/// xlseek
//
// SUMMARY
//  proxy for lseek
//
// PARAMETERS
//  [res] - *res is set to the return from lseek
//
off_t xlseek(int fd, off_t offset, int whence);

/// xclose
//
// SUMMARY
//  proxy for close
//
void xclose(int fd);

/// ixclose
//
// SUMMARY
//  idempotent proxy for close
//
void ixclose(int * const restrict fd)
  __attribute__((nonnull));

/// xsymlink
//
// SUMMARY
//  proxy for symlink
//
// VARIANTS
//  s/f/vf - different ways to pass parameters
//
void xsymlinks(const char * restrict target, const char * restrict linkpath)
  __attribute__((nonnull));

void xsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
  __attribute__((nonnull(1, 2)));

void xsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));

/// uxsymlink
//
// SUMMARY
//  proxy for symlink which only fails when errno != EEXIST
//
// VARIANTS
//  s/f/vf - different ways to pass
//
int uxsymlinks(const char * restrict target, const char * restrict linkpath)
  __attribute__((nonnull));

int uxsymlinkf(const char * restrict target_fmt, const char * restrict linkpath_fmt, ...)
  __attribute__((nonnull(1, 2)));

int uxsymlinkvf(const char * restrict target_fmt, const char * restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));

/// xunlink
//
// SUMMARY
//  proxy for unlink
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
void xunlinks(const char * restrict pathname)
  __attribute__((nonnull));

void xunlinkf(const char * restrict fmt, ...)
  __attribute__((nonnull(1)));

void xunlinkvf(const char * restrict fmt, va_list va)
  __attribute__((nonnull));

/// uxunlink
//
// SUMMARY
//  proxy for unlink that fails only when errno != ENOENT
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
int uxunlinks(const char * const restrict pathname)
  __attribute__((nonnull));

int uxunlinkf(const char * const restrict fmt, ...)
  __attribute__((nonnull(1)));

int uxunlinkvf(const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// xfork
//
// SUMMARY
//  proxy for fork
//
pid_t xfork(void);

/// xdup
//
// SUMMARY
//  proxy for dup
//
void xdup(int oldfd);

/// xdup2
//
// SUMMARY
//  proxy for dup2
//
void xdup2(int oldfd, int newfd);

/// xsetresuid
//
// SUMMARY
//  proxy for setresuid
//
void xsetresuid(uid_t ruid, uid_t euid, uid_t suid);

/// xsetresgid
//
// SUMMARY
//  proxy for setresgid
//
void xsetresgid(gid_t rgid, gid_t egid, gid_t sgid);

/// xseteuid
//
// SUMMARY
//  proxy for seteuid
//
void xseteuid(uid_t euid);

/// xsetegid
//
// SUMMARY
//  proxy for setegid
//
void xsetegid(gid_t egid);

/// xgetresuid
//
// SUMMARY
//  proxy for getresuid
//
void xgetresuid(uid_t * const restrict ruid, uid_t * const restrict euid, uid_t * const restrict suid)
  __attribute__((nonnull));

/// xgetresgid
//
// SUMMARY
//  proxy for getresgid
//
void xgetresgid(gid_t * const restrict rgid, gid_t * const restrict egid, gid_t * const restrict sgid)
  __attribute__((nonnull));

/// xtruncate
//
// SUMMARY
//  proxy for truncate
//
void xtruncate(const char * restrict path, off_t length)
  __attribute__((nonnull));

/// xftruncate
//
// SUMMARY
//  proxy for ftruncate
//
void xftruncate(int fd, off_t length);

/// xrmdir
//
// SUMMARY
//  proxy for rmdir
//
void xrmdir(const char * restrict pathname)
  __attribute__((nonnull));

/// xsetpgid
//
// SUMMARY
//  proxy for setpgid
//
void xsetpgid(pid_t pid, pid_t pgid);

/// xsetsid
//
// SUMMARY
//  proxy for setsid
//
void xsetsid(void);

/// xexecv
//
// SUMMARY
//  xapi proxy for execv
//
void xexecv(const char * path, char * const argv[])
  __attribute__((nonnull));

/// xexecve
//
// SUMMARY
//  xapi proxy for execvpe
//
void xexecve(const char * path, char * const argv[], char * const envp[])
  __attribute__((nonnull));

/// xexecvp
//
// SUMMARY
//  xapi proxy for execvp
//
void xexecvp(const char * file, char * const argv[])
  __attribute__((nonnull));

/// xfexecve
//
// SUMMARY
//  xapi proxy for fexecve
//
void xfexecve(int fd, char * const argv[], char * const envp[])
  __attribute__((nonnull));

/// xchdir
//
// SUMMARY
//  xapi proxy for chdir
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
void xchdirs(const char * const restrict path)
  __attribute__((nonnull));

void xchdirf(const char * const restrict fmt, ...)
  __attribute__((nonnull));

void xchdirvf(const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// xfchdir
//
// SUMMARY
//  xapi proxy for fchdir
//
void xfchdir(int fd);

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
void xpipe(int filedes[2])
  __attribute__((nonnull));

#endif
