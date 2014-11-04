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

#ifndef _PARAMS_H
#define _PARAMS_H

#include <sys/types.h>

#include "path.h"

extern struct g_params_t
{
//
// execution parameters
//

	pid_t								pid;												// pid of this process
	pid_t								ppid;												// pid of parent process
	pid_t								sid;												// session-id
	char *							cwd;												// cwd
	int									cwdl;
	char *							exedir;											// location of the fab executable when the program started
	int									exedirl;

	uid_t								ruid;												// real-user-id
	char *							ruid_name;
	int									ruid_namel;
	uid_t								euid;												// effective-user-id   (must be fabsys)
	char *							euid_name;
	int									euid_namel;
	gid_t								rgid;												// real-group-id
	char *							rgid_name;
	int									rgid_namel;
	gid_t								egid;												// effective-group-id  (must be fabsys)
	char *							egid_name;
	int									egid_namel;

	long								procs;											// detected number of processors (0 means unknown)

	path *							init_fabfile_path;					// path to initial fabfile
} g_params;

/// params_parse
//
// parses command-line options, populating g_params.
//
// Prints the usage statement and terminates the program if options are
// invalid, for example required options are not present, or invalid
// parameters are given to an option
//
int params_setup();

/// params_teardown
//
// free g_params
//
void params_teardown();

#endif
