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
#include <pwd.h>
#include <grp.h>

#include "identity.h"

#include "log.h"

#define fail(fmt, ...)				\
	do {												\
		fprintf(stderr, fmt "\n" 	\
			, ##__VA_ARGS__					\
		);												\
		return 0;									\
	} while(0)

int identity_init()
{
	struct passwd *	pwd;
	struct group *	grp;

	// get user identity of this process
	uid_t suid;
	getresuid(&g_args.ruid, &g_args.euid, &suid);

	errno = 0;
	if((pwd = getpwuid(g_args.ruid)) == 0)
		fail("cannot get ruid : [%d][%s]", errno, strerror(errno));

	g_args.ruid_name = strdup(pwd->pw_name);

	errno = 0;
	if((pwd = getpwuid(g_args.euid)) == 0)
		fail("cannot get euid : [%d][%s]", errno, strerror(errno));

	g_args.euid_name = strdup(pwd->pw_name);

	// get group identity of this process
	gid_t sgid;
	getresgid(&g_args.rgid, &g_args.egid, &sgid);

	errno = 0;
	if((grp = getgrgid(g_args.rgid)) == 0)
		fail("cannot get rgid : [%d][%s]", errno, strerror(errno));

	g_args.rgid_name = strdup(grp->gr_name);

	errno = 0;
	if((grp = getgrgid(g_args.egid)) == 0)
		fail("cannot get egid : [%d][%s]", errno, strerror(errno));

	g_args.egid_name = strdup(grp->gr_name);

	// this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions !!
	if(strcmp(g_args.euid_name, "fabsys") || strcmp(g_args.egid_name, "fabsys"))
	{
/*
		fail(
			"fab executable must be owned by fabsys:fabsys and have u+s and g+s permissions\n"
			" -> r:%s/%d:%s/%d\n"
			" -> e:%s/%d:%s/%d"
			, g_args.ruid_name, g_args.ruid, g_args.rgid_name, g_args.rgid
			, g_args.euid_name, g_args.euid, g_args.egid_name, g_args.egid
		);
*/
	}

	// assume identity of the user - default identity
	if(seteuid(g_args.ruid) == -1)
		fail("cannot seteuid(ruid) : [%d][%s]", errno, strerror(errno));

	if(setegid(g_args.rgid) == -1)
		fail("cannot setegid(rgid) : [%d][%s]", errno, strerror(errno));

	return 1;
}

/*

int identity_init()
{
	struct passwd	_pwd;
	char					ubuf[512];
	struct group	_grp;
	char					gbuf[512];

	int e;

	// get user identity of this process
	uid_t suid;
	getresuid(&g_args.ruid, &g_args.euid, &suid);

	struct passwd * pwd = 0;
	if((e = getpwuid_r(g_args.ruid, &_pwd, ubuf, sizeof(ubuf), &pwd)))
	{
		fail("cannot get ruid : [%d][%s]", e, strerror(e));
	}
	g_args.ruid_name = strdup(pwd->pw_name);

	if((e = getpwuid_r(g_args.euid, &_pwd, ubuf, sizeof(ubuf), &pwd)))
	{
		fail("cannot get euid : [%d][%s]", e, strerror(e));
	}
	g_args.euid_name = strdup(pwd->pw_name);

	// get group identity of this process
	gid_t sgid;
	getresgid(&g_args.rgid, &g_args.egid, &sgid);

	struct group * grp = 0;
	if((e = getgrgid_r(g_args.rgid, &_grp, gbuf, sizeof(gbuf), &grp)))
	{
		fail("cannot get rgid : [%d][%s]", e, strerror(e));
	}
	g_args.rgid_name = strdup(grp->gr_name);

	if((e = getgrgid_r(g_args.egid, &_grp, gbuf, sizeof(gbuf), &grp)))
	{
		fail("cannot get egid : [%d][%s]", e, strerror(e));
	}
	g_args.egid_name = strdup(grp->gr_name);

	// this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions !!
	if(strcmp(g_args.euid_name, "fabsys") || strcmp(g_args.egid_name, "fabsys"))
	{
		fail(
			"fab executable must be owned by fabsys:fabsys and have u+s and g+s permissions\n"
			" -> r:%s/%d:%s/%d\n"
			" -> e:%s/%d:%s/%d"
			, g_args.ruid_name, g_args.ruid, g_args.rgid_name, g_args.rgid
			, g_args.euid_name, g_args.euid, g_args.egid_name, g_args.egid
		);
	}

	// assume identity of the user - default identity
	if(seteuid(g_args.ruid) == -1)
		fail("cannot seteuid(ruid) : [%d][%s]", errno, strerror(errno));

	if(setegid(g_args.rgid) == -1)
		fail("cannot setegid(rgid) : [%d][%s]", errno, strerror(errno));

	return 1;
}

*/

#undef fail
#include "control.h"

int identity_assume_user()
{
	fatal_os(seteuid, g_args.ruid);
	fatal_os(setegid, g_args.rgid);

	finally : coda;
}

int identity_assume_fabsys()
{
	fatal_os(seteuid, g_args.euid);
	fatal_os(setegid, g_args.egid);

	finally : coda;
}

