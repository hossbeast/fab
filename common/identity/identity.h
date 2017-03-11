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

#ifndef _IDENTITY_H
#define _IDENTITY_H

#include <unistd.h>

#include "xapi.h"

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

/// identity_setup
//
// populate associated globals
//
xapi identity_setup(void);

/// identity_teardown
//
// release resources
//
void identity_teardown(void);

/// identity_assume_user
//
// assume ruid identity, i.e. the user who executed the program
//
xapi identity_assume_real(void);

/// identity_assume_fabsys
//
// assume euid identity, i.e. fabsys:fabsys
//
xapi identity_assume_effective(void);

#endif
