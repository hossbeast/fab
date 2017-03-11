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

#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpwd.h"
#include "xlinux/xgrp.h"
#include "xlinux/xstring.h"

#include "identity.h"

uid_t  g_ruid;          // real-user-id
char * g_ruid_name;
int    g_ruid_namel;
uid_t  g_euid;          // effective-user-id   (fabsys in the final build)
char * g_euid_name;
int    g_euid_namel;
gid_t  g_rgid;          // real-group-id
char * g_rgid_name;
int    g_rgid_namel;
gid_t  g_egid;          // effective-group-id  (fabsys in the final build)
char * g_egid_name;
int    g_egid_namel;

//
// public
//

xapi identity_setup()
{
  enter;

  struct passwd * pwd;
  struct group *  grp;

  // get user identity
  uid_t __attribute__((unused)) g_suid;
  fatal(xgetresuid, &g_ruid, &g_euid, &g_suid);

  // get group identity
  gid_t __attribute__((unused)) g_sgid;
  fatal(xgetresgid, &g_rgid, &g_egid, &g_sgid);

  // get real-user-id name for this process
  fatal(xgetpwuid, g_ruid, &pwd);
  fatal(ixstrdup, &g_ruid_name, pwd->pw_name);
  g_ruid_namel = strlen(g_ruid_name);

  // get effective-user-id name for this process
  fatal(xgetpwuid, g_euid, &pwd);
  fatal(ixstrdup, &g_euid_name, pwd->pw_name);
  g_euid_namel = strlen(g_euid_name);

  // get real-group-id name for this process
  fatal(xgetgrgid, g_rgid, &grp);
  fatal(ixstrdup, &g_rgid_name, grp->gr_name);
  g_rgid_namel = strlen(g_rgid_name);

  // get effective-group-id name for this process
  fatal(xgetgrgid, g_egid, &grp);
  fatal(ixstrdup, &g_egid_name, grp->gr_name);
  g_egid_namel = strlen(g_egid_name);

  finally : coda;
}

void identity_teardown()
{
  iwfree(&g_ruid_name);
  iwfree(&g_rgid_name);
  iwfree(&g_euid_name);
  iwfree(&g_egid_name);
}

//
// api
//

xapi identity_assume_real()
{
  enter;

  fatal(xseteuid, g_ruid);
  fatal(xsetegid, g_rgid);

  finally : coda;
}

xapi identity_assume_effective()
{
  enter;

  fatal(xseteuid, g_euid);
  fatal(xsetegid, g_egid);

  finally : coda;
}
