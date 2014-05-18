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

#include <errno.h>

#include "internal.h"

#include "xunistd.h"

#include <stdio.h>

int API xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && ((*bytes) = read(fd, buf, count)) == -1)
		fail(errno);

	else if(!bytes && read(fd, buf, count) == -1)
		fail(errno);

	finally : coda;
}

int API uxread(int fd, void * buf, size_t count, ssize_t * bytes)
{
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

int API axread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	ssize_t actual;
	if((actual = read(fd, buf, count)) == -1)
		fail(errno);

	if(actual != count)
		fail(XLINUX_LESS);

finally:
	if(bytes)
		*bytes = actual;

	if(XAPI_ERRCODE == XLINUX_LESS)
	{
		XAPI_INFOF("expected", "%zu", count);
		XAPI_INFOF("actual", "%zd", actual);
	}
coda;
}

int API xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = write(fd, buf, count)) == -1)
		fail(errno);

	else if(!bytes && write(fd, buf, count) == -1)
		fail(errno);

	finally : coda;
}

int API xgetcwd(char * buf, size_t size, char ** res)
{
	if(res && (((*res) = getcwd(buf, size)) == 0))
		fail(errno);

	else if(!res && getcwd(buf, size) == 0)
		fail(errno);

	finally : coda;
}

int API xlseek(int fd, off_t offset, int whence, off_t * res)
{
	if(res && ((*res) = lseek(fd, offset, whence)) == (off_t)-1)
		fail(errno);

	else if(!res && lseek(fd, offset, whence) == (off_t)-1)
		fail(errno);

	finally : coda;
}

int API xclose(int fd)
{
	fatalize(errno, close, fd);

	finally : coda;
}

int API ixclose(int * fd)
{
	if(*fd != -1)
	{
		fatalize(errno, close, *fd);
		*fd = -1;
	}

	finally : coda;
}

int API xsymlink(const char * target, const char * linkpath)
{
	fatalize(errno, symlink, target, linkpath);

	finally : coda;
}

int API uxsymlink(const char * target, const char * linkpath)
{
	if(symlink(target, linkpath) != 0 && errno != EEXIST)
		fail(errno);

	finally : coda;
}

int API xunlink(const char * pathname, int * r)
{
	if(r && ((*r) = unlink(pathname)) != 0)
		fail(errno);

	else if(!r && unlink(pathname) != 0)
		fail(errno);

	finally : coda;
}

int API uxunlink(const char * pathname, int * r)
{
	if(r && ((*r) = unlink(pathname)) != 0 && errno != ENOENT)
		fail(errno);

	else if(!r && unlink(pathname) != 0 && errno != ENOENT)
		fail(errno);

	finally : coda;
}

int API xfork(pid_t * r)
{
	if(r && (((*r) = fork()) == -1))
		fail(errno);
	
	else if(!r && fork() == -1)
		fail(errno);

	finally : coda;
}

int API xdup(int oldfd)
{
	if(dup(oldfd) == -1)
		fail(errno);

	finally : coda;
}

int API xdup2(int oldfd, int newfd)
{
	if(dup2(oldfd, newfd) == -1)
		fail(errno);

	finally : coda;
}

int API xgetresuid(uid_t * const ruid, uid_t * const euid, uid_t * const suid)
{
	fatalize(errno, getresuid, ruid, euid, suid);

	finally : coda;
}

int API xgetresgid(gid_t * const rgid, gid_t * const egid, gid_t * const sgid)
{
	fatalize(errno, getresgid, rgid, egid, sgid);

	finally : coda;
}

int API xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	fatalize(errno, setresuid, ruid, euid, suid);

	finally : coda;
}

int API xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
	fatalize(errno, setresgid, rgid, egid, sgid);

	finally : coda;
}

int API xeuidaccess(const char * pathname, int mode, int * const r)
{
	if((r && ((*r) = euidaccess(pathname, mode)) == -1) || (!r && euidaccess(pathname, mode) == -1))
		fail(errno);

finally:
	XAPI_INFOS("path", pathname);
coda;
}	

int API uxeuidaccess(const char * pathname, int mode, int * const r)
{
	if(r && ((*r) = euidaccess(pathname, mode)) == -1 && errno != EACCES && errno != ENOENT && errno != ENOTDIR)
		fail(errno);
		
	else if(!r && euidaccess(pathname, mode) == -1 && errno != EACCES && errno != ENOENT && errno != ENOTDIR)
		fail(errno);

finally:
	XAPI_INFOS("path", pathname);
coda;
}	

int API xseteuid(uid_t euid)
{
	fatalize(errno, seteuid, euid);

finally:
	XAPI_INFOF("euid", "%d", euid);
coda;
}

int API xsetegid(gid_t egid)
{
	fatalize(errno, setegid, egid);

finally:
	XAPI_INFOF("egid", "%d", egid);
coda;
}

int API xtruncate(const char * path, off_t length)
{
	fatalize(errno, truncate, path, length);

finally :
	XAPI_INFOS("path", path);
coda;
}

int API xftruncate(int fd, off_t length)
{
	fatalize(errno, ftruncate, fd, length);

	finally : coda;
}

int API xrmdir(const char * pathname)
{
	fatalize(errno, rmdir, pathname);

finally:
	XAPI_INFOS("pathname", pathname);
coda;
}
