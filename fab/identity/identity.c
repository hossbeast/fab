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

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#include "identity.h"

#include "args.h"
#include "params.h"
#include "log.h"

#define fail(fmt, ...)				\
	do {												\
		fprintf(stderr, fmt "\n" 	\
			, ##__VA_ARGS__					\
		);												\
		if(UNWIND_ERRORS)					\
			return 1;								\
		else											\
			return -1;							\
	} while(0)

int identity_init()
{
	struct passwd *	pwd;
	struct group *	grp;

	// get user identity of this process
	uid_t suid;
	getresuid(&g_params.ruid, &g_params.euid, &suid);

	// get group identity of this process
	gid_t sgid;
	getresgid(&g_params.rgid, &g_params.egid, &sgid);

	errno = 0;
	if((pwd = getpwuid(g_params.ruid)) == 0)
		fail("cannot get ruid name : [%d][%s] (ruid=%d rgid=%d euid=%d egid=%d)", errno, strerror(errno), g_params.ruid, g_params.rgid, g_params.euid, g_params.egid);

	g_params.ruid_name = strdup(pwd->pw_name);

	errno = 0;
	if((pwd = getpwuid(g_params.euid)) == 0)
		fail("cannot get euid name : [%d][%s] (ruid=%d rgid=%d euid=%d egid=%d)", errno, strerror(errno), g_params.ruid, g_params.rgid, g_params.euid, g_params.egid);

	g_params.euid_name = strdup(pwd->pw_name);

	errno = 0;
	if((grp = getgrgid(g_params.rgid)) == 0)
		fail("cannot get rgid name : [%d][%s] (ruid=%d rgid=%d euid=%d egid=%d)", errno, strerror(errno), g_params.ruid, g_params.rgid, g_params.euid, g_params.egid);

	g_params.rgid_name = strdup(grp->gr_name);

	errno = 0;
	if((grp = getgrgid(g_params.egid)) == 0)
		fail("cannot get egid name : [%d][%s] (ruid=%d rgid=%d euid=%d egid=%d)", errno, strerror(errno), g_params.ruid, g_params.rgid, g_params.euid, g_params.egid);

	g_params.egid_name = strdup(grp->gr_name);

#if DEVEL
	// this check is omitted in DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
	// this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
	if(strcmp(g_params.euid_name, "fabsys") || strcmp(g_params.egid_name, "fabsys"))
	{
		fail(
			"fab executable must be owned by fabsys:fabsys and have u+s and g+s permissions\n"
			" -> r:%s/%d:%s/%d\n"
			" -> e:%s/%d:%s/%d"
			, g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid
			, g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid
		);
	}
#endif
	
	/*
	** In addition, this program requires explicit control over file and directory permissions for
	** created files. In particular, the directories in /var/tmp/fab are owned by fabsys:fabsys (so
	** they can be deleted) but the files are user:user (so they can be executed)
	*/
	umask(0);

	return 0;
}

#undef fail
#include "fab_control.h"

int identity_assume_user()
{
	fatal_os(seteuid, g_params.ruid);
	fatal_os(setegid, g_params.rgid);

	finally : coda;
}

int identity_assume_fabsys()
{
	fatal_os(seteuid, g_params.euid);
	fatal_os(setegid, g_params.egid);

	finally : coda;
}
