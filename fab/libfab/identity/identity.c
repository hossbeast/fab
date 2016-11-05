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

#include "internal.h"
#include "identity.internal.h"
#include "errtab/FAB.errtab.h"

static uid_t  ruid;          // real-user-id
static char * ruid_name;
static int    ruid_namel;
static uid_t  euid;          // effective-user-id   (fabsys in the final build)
static char * euid_name;
static int    euid_namel;
static gid_t  rgid;          // real-group-id
static char * rgid_name;
static int    rgid_namel;
static gid_t  egid;          // effective-group-id  (fabsys in the final build)
static char * egid_name;
static int    egid_namel;

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
  fatal(xgetresuid, &ruid, &euid, &suid);

  // get group identity
  gid_t __attribute__((unused)) sgid;
  fatal(xgetresgid, &rgid, &egid, &sgid);

  // get real-user-id name for this process
  fatal(xgetpwuid, ruid, &pwd);
  fatal(ixstrdup, &ruid_name, pwd->pw_name);
  ruid_namel = strlen(ruid_name);

  // get effective-user-id name for this process
  fatal(xgetpwuid, euid, &pwd);
  fatal(ixstrdup, &euid_name, pwd->pw_name);
  euid_namel = strlen(euid_name);

  // get real-group-id name for this process
  fatal(xgetgrgid, rgid, &grp);
  fatal(ixstrdup, &rgid_name, grp->gr_name);
  rgid_namel = strlen(rgid_name);

  // get effective-group-id name for this process
  fatal(xgetgrgid, egid, &grp);
  fatal(ixstrdup, &egid_name, grp->gr_name);
  egid_namel = strlen(egid_name);

#if DEBUG || DEVEL
  // this check is omitted in DEBUG/DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
  // this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
  if(strcmp(euid_name, "fabsys") || strcmp(egid_name, "fabsys"))
  {
    xapi_fail_intent();
    xapi_info_addf("real", "r:%s/%d:%s/%d", ruid_name, ruid, rgid_name, rgid);
    xapi_info_addf("effective", "e:%s/%d:%s/%d", euid_name, euid, egid_name, egid);
    fail(FAB_FABPERMS);
  }
#endif

  finally : coda;
}

void identity_teardown()
{
  iwfree(&ruid_name);
  iwfree(&rgid_name);
  iwfree(&euid_name);
  iwfree(&egid_name);
}

//
// api
//

API xapi identity_assume_user()
{
  enter;

  fatal(xseteuid, ruid);
  fatal(xsetegid, rgid);

  finally : coda;
}

API xapi identity_assume_fabsys()
{
  enter;

  fatal(xseteuid, euid);
  fatal(xsetegid, egid);

  finally : coda;
}
