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

#include "xlinux.h"
#include "xapi.h"

#include "identity.h"
#include "FAB.errtab.h"

#include "args.h"
#include "params.h"

int identity_init()
{
	struct passwd *	pwd;
	struct group *	grp;

	// get user identity
	uid_t __attribute__((unused)) suid;
	fatal(xgetresuid, &g_params.ruid, &g_params.euid, &suid);

	// get group identity
	gid_t __attribute__((unused)) sgid;
	fatal(xgetresgid, &g_params.rgid, &g_params.egid, &sgid);

	// get real-user-id name for this process
	fatal(xgetpwuid, g_params.ruid, &pwd);
	g_params.ruid_name = strdup(pwd->pw_name);
	g_params.ruid_namel = strlen(g_params.ruid_name);

	// get effective-user-id name for this process
	fatal(xgetpwuid, g_params.euid, &pwd);
	g_params.euid_name = strdup(pwd->pw_name);
	g_params.euid_namel = strlen(g_params.euid_name);

	// get real-group-id name for this process
	fatal(xgetgrgid, g_params.rgid, &grp);
	g_params.rgid_name = strdup(grp->gr_name);
	g_params.rgid_namel = strlen(g_params.rgid_name);

	// get effective-group-id name for this process
	fatal(xgetgrgid, g_params.egid, &grp);
	g_params.egid_name = strdup(grp->gr_name);
	g_params.egid_namel = strlen(g_params.egid_name);

#if DEBUG || DEVEL
	// this check is omitted in DEBUG/DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
	// this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
	if(strcmp(g_params.euid_name, "fabsys") || strcmp(g_params.egid_name, "fabsys"))
	{
		failf(FAB_EXESUID,
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
	** they can be unlinked) but the files are user:user (so they can be executed)
	*/
	umask(0);

	finally : coda;
}

int identity_assume_user()
{
	fatal(xseteuid, g_params.ruid);
	fatal(xsetegid, g_params.rgid);

	finally : coda;
}

int identity_assume_fabsys()
{
	fatal(xseteuid, g_params.euid);
	fatal(xsetegid, g_params.egid);

	finally : coda;
}
