/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#define restrict __restrict

/// xread
//
// SUMMARY
//  proxy for read
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
int xread(int fd, void * restrict buf, size_t count, ssize_t * restrict bytes)
	__attribute__((nonnull(2)));

/// xxread
//
// SUMMARY
//  proxy for read that also fails when count != actual
//
// PARAMETERS
//  [bytes] - returns the number of bytes read
//
int axread(int fd, void * restrict buf, size_t count, ssize_t * restrict bytes)
	__attribute__((nonnull(2)));

/// xwrite
//
// SUMMARY
//  proxy for write
//
// PARAMETERS
//  [bytes] - returns the number of bytes written
//
int xwrite(int fd, const void * buf, size_t count, ssize_t * bytes);

/// xgetcwd
//
// SUMMARY
//  proxy for getcwd
//
// PARAMETERS
//  [res] - *res is set to the return from getcwd
//
int xgetcwd(char * buf, size_t size, char ** res);

/// xlseek
//
// SUMMARY
//  proxy for lseek
//
// PARAMETERS
//  [res] - *res is set to the return from lseek
//
int xlseek(int fd, off_t offset, int whence, off_t * res);

/// xclose
//
// SUMMARY
//  proxy for close
//
int xclose(int fd);

/// ixclose
//
// SUMMARY
//  idempotent proxy for close
//
int ixclose(int * const restrict fd)
	__attribute__((nonnull));

/// xsymlink
//
// SUMMARY
//  proxy for symlink
//
int xsymlink(const char * restrict target, const char * restrict linkpath)
	__attribute__((nonnull));

/// uxsymlink
//
// SUMMARY
//  proxy for symlink which only fails when errno != EEXIST
//
int uxsymlink(const char * restrict target, const char * restrict linkpath)
	__attribute__((nonnull));

/// xunlink
//
// SUMMARY
//  proxy for unlink
//
int xunlink(const char * restrict pathname, int * restrict r)
	__attribute__((nonnull(1)));

/// uxunlink
//
// SUMMARY
//  proxy for unlink that fails only when errno != ENOENT
//
int uxunlink(const char * restrict pathname, int * restrict r)
	__attribute__((nonnull(1)));

/// xfork
//
// SUMMARY
//  proxy for fork
//
int xfork(pid_t * const restrict pid);

/// xdup
//
// SUMMARY
//  proxy for dup
//
int xdup(int oldfd);

/// xdup2
//
// SUMMARY
//  proxy for dup2
//
int xdup2(int oldfd, int newfd);

/// xsetresuid
//
// SUMMARY
//  proxy for setresuid
//
int xsetresuid(uid_t ruid, uid_t euid, uid_t suid);

/// xsetresgid
//
// SUMMARY
//  proxy for setresgid
//
int xsetresgid(gid_t rgid, gid_t egid, gid_t sgid);

/// xeuidaccess
//
// SUMMARY
//  proxy for euidaccess
//
int xeuidaccess(const char * restrict pathname, int mode, int * const restrict r)
	__attribute__((nonnull(1)));

/// uxeuidaccess
//
// SUMMARY
//  proxy for euidaccess that only fails when errno !: { EACCES, ENOENT, ENOTDIR }
//
int uxeuidaccess(const char * restrict pathname, int mode, int * const restrict r)
	__attribute__((nonnull(1)));

/// xseteuid
//
// SUMMARY
//  proxy for seteuid
//
int xseteuid(uid_t euid);

/// xsetegid
//
// SUMMARY
//  proxy for setegid
//
int xsetegid(gid_t egid);

/// xgetresuid
//
// SUMMARY
//  proxy for getresuid
//
int xgetresuid(uid_t * const restrict ruid, uid_t * const restrict euid, uid_t * const restrict suid)
	__attribute__((nonnull));

/// xgetresgid
//
// SUMMARY
//  proxy for getresgid
//
int xgetresgid(gid_t * const restrict rgid, gid_t * const restrict egid, gid_t * const restrict sgid)
	__attribute__((nonnull));

/// xtruncate
//
// SUMMARY
//  proxy for truncate
//
int xtruncate(const char * restrict path, off_t length)
	__attribute__((nonnull));

/// xftruncate
//
// SUMMARY
//  proxy for ftruncate
//
int xftruncate(int fd, off_t length);

/// xrmdir
//
// SUMMARY
//  proxy for rmdir
//
int xrmdir(const char * restrict pathname)
	__attribute__((nonnull));

#undef restrict
#endif
