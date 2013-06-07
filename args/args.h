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

#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

#include "path.h"
struct selector;

/*
** PER-GN : delete if newest file is older than <policy>  (pertains to a given gn)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/stat						stat hash			// rewritten after successfully
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/content					content hash	// processing a detected change to
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/vrs							version hash	// the underlying source file
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/dscv						cached ddisc
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/PRIMARY/afeed_secondary_skipweak/<gn-id>	link to 'gn/<gn-id>' for a SECONDARY gn
**
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/fab/noforce_ff							// require fabrication of this secondary node due to ff change
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/fab/noforce_gn							// require fabrication of this secondary node due to primary file change
** /var/cache/fab/INIT/<ff-id>/gn/<gn-id>/SECONDARY/aneed_primary_skipweak/<gn-id>		link to 'gn/<gn-id>' for a PRIMARY gn
**
** PER-FF : delete if newest file is older than <policy>  (pertains to a given fabfile)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/stat						     	stat hash			// rewritten after successfully
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/content					    	content hash	// processing a detected change to
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/vrs							    	version hash	// the regular fabfile
** /var/cache/fab/INIT/<ff-id>/ff/<ff-id>/REGULAR/closure_gns/<gn-id>		link to 'gn/<gn-id>' for an enclosed gn
**
** PER-PID : delete if pid is not presently executing     (pertains to a given fab process)
** -------------------------------------------------------------------------------------------------------------------------------
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/cmd							cmd text
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stdo						stdout from cmd
** /var/tmp/fab/pid/<pid>/fml/<fmlvnum>/stde						stderr from cmd
*/

#define CACHEDIR_BASE							"/var/cache/fab"
#define TMPDIR_BASE								"/var/tmp/fab"
#define FABLW_DIRS								(char*[]){ "/usr/lib/fab/listwise" }

#define DEFAULT_INIT_FABFILE 			"./fabfile"
#define DEFAULT_INVALIDATE_ALL		0
#define DEFAULT_MODE_BPLAN				MODE_BPLAN_EXEC
#define DEFAULT_MODE_GNID					MODE_GNID_RELATIVE
#define DEFAULT_MODE_CYCL					MODE_CYCL_WARN
#define DEFAULT_CONCURRENCY_LIMIT	0
#define DEFAULT_INVOKEDIR					"/usr/lib/fab/lib"

#define EXPIRATION_POLICY					(60 * 60 * 24 * 7)		/* 7 days */

#define MODE_TABLE(x)																																							\
/* execution modes */																																							\
	_MODE(MODE_BPLAN_GENERATE	, 0x01	, x)		/* generate the buildplan */													\
	_MODE(MODE_BPLAN_BAKE			, 0x02	, x)		/* bake the buildplan */															\
	_MODE(MODE_BPLAN_EXEC			, 0x03	, x)		/* execute the buildplan */														\
/* path display modes */																																					\
	_MODE(MODE_GNID_RELATIVE	, 0x05	, x)		/* path relative to the initial fabfile */						\
	_MODE(MODE_GNID_CANON			, 0x06	, x)		/* canonical path */																	\
/* cycle handling modes */																																				\
	_MODE(MODE_CYCL_WARN			, 0x09	, x)		/* warn when a cycle is detected */										\
	_MODE(MODE_CYCL_FAIL			, 0x0a	, x)		/* fail when a cycle is detected */										\
	_MODE(MODE_CYCL_DEAL			, 0x0b	, x)		/* deal when a cycle is detected (halt traversal) */	\

enum {
#define _MODE(a, b, c) a = b,
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a :
#define MODE_STR(x) MODE_TABLE(x) "unknown"

extern struct g_args_t
{
//
// execution parameters
//

	pid_t								pid;									// pid of this process
	pid_t								sid;									// session-id
	char *							cwd;									// cwd

	uid_t								ruid;									// real-user-id
	char *							ruid_name;
	uid_t								euid;									// effective-user-id   (must be fabsys)
	char *							euid_name;
	gid_t								rgid;									// real-group-id
	char *							rgid_name;
	gid_t								egid;									// effective-group-id  (must be fabsys)
	char *							egid_name;

//
// arguments
//

	int									mode_bplan;						// buildplan mode
	int									mode_gnid;						// gn identification mode
	int									mode_cycl;						// cycle handling mode

	int									concurrency;					// concurrently limiting factor
	path *							init_fabfile_path;		// path to initial fabfile
	char *							bakescript_path;			// path to bakescript

	char **							rootvars;							// root scope variable expressions
	int									rootvarsl;
	int									rootvarsa;

	char **							invokedirs;						// root directories for locating invocations
	int									invokedirsl;

	struct selector * 	selectors;						// node selectors
	int									selectorsa;
	int									selectorsl;

	int									invalidationsz;				// invalidate all nodes (-B)
} g_args;

/// args_parse
//
// parses command-line options, populating g_args.
//
// Prints the usage statement and terminates the program if options are
// invalid, for example required options are not present, or invalid
// parameters are given to an option
//
// returns zero on failure (malloc failure, for example)
//
int args_parse(int argc, char** argv);

/// args_teardown
//
// free g_args
//
void args_teardown();

#endif

