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
#include "xlinux/xunistd.h"
#include "xlinux/xpwd.h"
#include "xlinux/xgrp.h"
#include "xlinux/xstring.h"

#include "internal.h"
#include "identity.internal.h"
#include "errtab/FAB.errtab.h"
#include "params.internal.h"

//
// public
//

xapi identity_setup()
{
  enter;

  struct passwd * pwd;
  struct group *  grp;

  // get user identity
  uid_t __attribute__((unused)) suid;
  fatal(xgetresuid, &g_params.ruid, &g_params.euid, &suid);

  // get group identity
  gid_t __attribute__((unused)) sgid;
  fatal(xgetresgid, &g_params.rgid, &g_params.egid, &sgid);

  // get real-user-id name for this process
  fatal(xgetpwuid, g_params.ruid, &pwd);
  fatal(ixstrdup, &g_params.ruid_name, pwd->pw_name);
  g_params.ruid_namel = strlen(g_params.ruid_name);

  // get effective-user-id name for this process
  fatal(xgetpwuid, g_params.euid, &pwd);
  fatal(ixstrdup, &g_params.euid_name, pwd->pw_name);
  g_params.euid_namel = strlen(g_params.euid_name);

  // get real-group-id name for this process
  fatal(xgetgrgid, g_params.rgid, &grp);
  fatal(ixstrdup, &g_params.rgid_name, grp->gr_name);
  g_params.rgid_namel = strlen(g_params.rgid_name);

  // get effective-group-id name for this process
  fatal(xgetgrgid, g_params.egid, &grp);
  fatal(ixstrdup, &g_params.egid_name, grp->gr_name);
  g_params.egid_namel = strlen(g_params.egid_name);

#if DEBUG || DEVEL
  // this check is omitted in DEBUG/DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
  // this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
  if(strcmp(g_params.euid_name, "fabsys") || strcmp(g_params.egid_name, "fabsys"))
  {
    xapi_fail_intent();
    xapi_info_addf("real", "r:%s/%d:%s/%d", g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid);
    xapi_info_addf("effective", "e:%s/%d:%s/%d", g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid);
    fail(FAB_FABPERMS);
  }
#endif

  finally : coda;
}

//
// api
//

API xapi identity_assume_user()
{
  enter;

  fatal(xseteuid, g_params.ruid);
  fatal(xsetegid, g_params.rgid);

  finally : coda;
}

API xapi identity_assume_fabsys()
{
  enter;

  fatal(xseteuid, g_params.euid);
  fatal(xsetegid, g_params.egid);

  finally : coda;
}
