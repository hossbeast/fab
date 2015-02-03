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

/// identity_init
//
// populates the identity fields of g_params and asserts that this
// executable has the correct ownership and permissions
//
int identity_init();

/* 
** working off of the assumption that seteuid/setegid are negligible
** performance-wise, a pair of these calls should bracket every code block that needs to access
** fabsys:fabsys files, within the same function
*/

/// identity_assume_user
//
// assume ruid identity, i.e. the user who executed the program
//
int identity_assume_user();

/// identity_assume_fabsys
//
// assume euid identity, i.e. fabsys:fabsys
//
int identity_assume_fabsys();

#endif
