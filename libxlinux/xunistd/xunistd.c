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

int API xread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = read(fd, buf, count)) == -1)
		sysfatality("read");

	else if(read(fd, buf, count) == -1)
		sysfatality("read");

	finally : coda;
}

int API axread(int fd, void * buf, size_t count, ssize_t * bytes)
{
	size_t actual;

	if((actual = read(fd, buf, count) == -1) || actual != count)
		sysfatality("read");

finally:
	if(bytes)
		*bytes = actual;

	XAPI_INFO("expected", "%zu", count);
	XAPI_INFO("actual", "%zu", actual);
coda;
}

int API xwrite(int fd, const void * buf, size_t count, ssize_t * bytes)
{
	if(bytes && (*bytes = write(fd, buf, count)) == -1)
		sysfatality("write");

	else if(write(fd, buf, count) == -1)
		sysfatality("write");

	finally : coda;
}

int API xgetcwd(char * buf, size_t size, char ** res)
{
	if(res && (((*res) = getcwd(buf, size)) == 0))
		sysfatality("getcwd");

	else if(getcwd(buf, size) == 0)
		sysfatality("getcwd");

	finally : coda;
}

int API xlseek(int fd, off_t offset, int whence, off_t * res)
{
	if(res && ((*res) = lseek(fd, offset, whence)) == (off_t)-1)
		sysfatality("lseek");

	else if(lseek(fd, offset, whence) == (off_t)-1)
		sysfatality("lseek");

	finally : coda;
}

int API xclose(int fd)
{
	sysfatalize(close, fd);

	finally : coda;
}

int API ixclose(int * fd)
{
	if(*fd != -1)
	{
		sysfatalize(close, *fd);
		*fd = -1;
	}

	finally : coda;
}

int API xsymlink(const char * target, const char * linkpath)
{
	sysfatalize(symlink, target, linkpath);

	finally : coda;
}

int API uxsymlink(const char * target, const char * linkpath)
{
	if(symlink(target, linkpath) != 0 && errno != EEXIST)
		sysfatality("symlink");

	finally : coda;
}

int API xunlink(const char * pathname)
{
	sysfatalize(unlink, pathname);

	finally : coda;
}

int API uxunlink(const char * pathname)
{
	if(unlink(pathname) != 0 && errno != ENOENT)
		sysfatality("unlink");

	finally : coda;
}

int API xfork(pid_t * r)
{
	if(r && (((*r) = fork()) == -1))
		sysfatality("fork");
	
	else if(fork() == -1)
		sysfatality("fork");

	finally : coda;
}

int API xdup(int oldfd)
{
	if(dup(oldfd) == -1)
		sysfatality("dup");

	finally : coda;
}

int API xdup2(int oldfd, int newfd)
{
	if(dup2(oldfd, newfd) == -1)
		sysfatality("dup2");

	finally : coda;
}

int API xsetresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	sysfatalize(setresuid, ruid, euid, suid);

	finally : coda;
}

int API xsetresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
	sysfatalize(setresgid, rgid, egid, sgid);

	finally : coda;
}

int API xeuidaccess(const char * pathname, int mode, int * const r)
{
	if(r && ((*r) = euidaccess(pathname, mode)) == -1 && errno != ENOENT)
		sysfatality("euidaccess");
		
	else if(euidaccess(pathname, mode) == -1)
		sysfatality("euidaccess");

	finally : coda;
}	
