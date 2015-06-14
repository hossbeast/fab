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

#ifndef _PARAMS_H
#define _PARAMS_H

#include <sys/types.h>
#include <stdint.h>

extern struct g_params_t
{
	//
	// non-configurable execution parameters
	//

	uid_t           ruid;                       // real-user-id
	char *          ruid_name;
	int             ruid_namel;
	uid_t           euid;                       // effective-user-id   (fabsys in the final build)
	char *          euid_name;
	int             euid_namel;
	gid_t           rgid;                       // real-group-id
	char *          rgid_name;
	int             rgid_namel;
	gid_t           egid;                       // effective-group-id  (fabsys in the final build)
	char *          egid_name;
	int             egid_namel;

	pid_t						fab_pid;										// pid of the fab process
	pid_t						fabd_pgid;									// pgid of the fabd process
	pid_t						pid;												// pid of this process
	pid_t						ppid;												// pid of parent process
	char *					cwd;												// cwd
	int							cwdl;
	char *					exedir;											// location of the fab executable when the program started
	int							exedirl;
	long						procs;											// detected number of processors (0 means unknown)
	long						pagesize;										// page size

	time_t					starttime;									// time last action of fabd began
	uint32_t				canhash;										// for ipc dir
	char						ipcdir[2048];							  // ipc dir
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

//
// signals used for communication between fab processes start at FABSIG_BASE
//
#define FABSIG_BASE 40

/// FABSIG_READY
//
// fabd -> fab
//  ready to accept commands
//
#define FABSIG_READY FABSIG_BASE + 0

/// FABSIG_START
//
// fab  -> fabd
//  new command is ready to be executed
//
#define FABSIG_START FABSIG_BASE + 1

/// FABSIG_DONE
//
// fabd -> fab
//  the command is complete
//
#define FABSIG_DONE FABSIG_BASE + 2

/// FABSIG_BPSTART
//
// fabd -> faba
//  buildplan stage is ready to be executed
//
#define FABSIG_BPSTART FABSIG_BASE + 3

/// FABSIG_BPGOOD
//
// faba -> fabd 
//  buildplan was executed successfully
//
#define FABSIG_BPGOOD FABSIG_BASE + 4

/// FABSIG_BPBAD
//
// faba -> fabd
//  buildplan was not successfully executed to completion
//
#define FABSIG_BPBAD FABSIG_BASE + 5

/// FABSIG_SIZE
//
// number of fab ipc signals, used for enumerating
//
#define FABSIG_SIZE 6

/// FABIPC_DATA
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*   files which need not be executable
//
// DESCRIPTION
//  ugo+rw   world read/write
//
// RATIONALE
//
#define FABIPC_DATA	0666

/// FABIPC_CODE
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*    files which need to be executable
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//  
#define FABIPC_CODE	0777

/// FABIPC_DIR
//
// LOCATION
//  {FABIPCDIR,FABTCMPDIR}/*    directories
//
// DESCRIPTION
//  ugo+rwx   world read/write/exec
//
// RATIONALE
//  
#define FABIPC_DIR	0777

#endif
