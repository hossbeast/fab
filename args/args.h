#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

#include "path.h"

#define FAB_VERSION		0x01

/*
** PER-SID : delete if no extant process in this session  (pertains to the last build in this session)
** ------------
** /var/cache/fab/sid/<sid>/gn/<gn-id-hash>/needs/strong/<link>
** /var/cache/fab/sid/<sid>/gn/<gn-id-hash>/needs/weak/<link>
**
** PER-GN : delete if newest file is older than <policy>  (pertains to a given PRIMARY file)
** ------------
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/fab/stat						stat hash			// -- updated after successful fabrication
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/fab/content					content hash
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/fab/vrs							version hash
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/dscv/stat						stat hash			// -- updated after successful ddisc
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/dscv/content				content hash
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/dscv/vrs						version hash
** /var/cache/fab/gn/PRIMARY/<gn-id-hash>/dscv/block					cached ddisc results
**
** PER-FF : delete if newest file is older than <policy>  (pertains to a given fabfile)
** ------------
** /var/cache/fab/ff/REGULAR/<ff-id-hash>/stat						stat hash
** /var/cache/fab/ff/REGULAR/<ff-id-hash>/content					content hash
** /var/cache/fab/ff/REGULAR/<ff-id-hash>/vrs							version hash
**
** PER-PID : delete if pid is not presently executing     (pertains to a given fab process)
** ------------
** /var/tmp/fab/pid/<pid>/fml/<fml-id-hash>/cmd
** /var/tmp/fab/pid/<pid>/fml/<fml-id-hash>/stdo
** /var/tmp/fab/pid/<pid>/fml/<fml-id-hash>/stde
*/

#define SID_DIR_BASE							"/var/cache/fab/sid"
#define GN_DIR_BASE								"/var/cache/fab/gn"
#define FF_DIR_BASE								"/var/cache/fab/ff"
#define PID_DIR_BASE							"/var/tmp/fab/pid"
#define FABLW_DIRS								(char*[]){ "/usr/lib/fab/listwise" }

#define DEFAULT_INIT_FABFILE 			"./fabfile"
#define DEFAULT_INVALIDATE_ALL		0
#define DEFAULT_DUMPNODE_ALL			0
#define DEFAULT_MODE_EXEC					MODE_EXEC_FABRICATE
#define DEFAULT_MODE_GNID					MODE_GNID_RELATIVE
#define DEFAULT_MODE_DDSC					MODE_DDSC_DEFERRED
#define DEFAULT_CONCURRENCY_LIMIT	0
#define DEFAULT_BAKE_PATH					"./bakescript"
#define DEFAULT_INVOKEDIR					"/usr/lib/fab/lib"

#define EXPIRATION_POLICY					(60 * 60 * 24 * 7)

/* modes */

#define MODE_TABLE(x)																																					\
/* execution modes */																																					\
	_MODE(MODE_EXEC_FABRICATE	, 0x00	, x)		/* fabricate targets */														\
	_MODE(MODE_EXEC_BUILDPLAN	, 0x01	, x)		/* generate buildplan only */											\
	_MODE(MODE_EXEC_DDSC			, 0x02	, x)		/* perform dependency discovery */								\
	_MODE(MODE_EXEC_DUMP			, 0x03	, x)		/* dump graph nodes */														\
	_MODE(MODE_EXEC_BAKE			, 0x04	, x)		/* bake the buildplan */													\
/* path display modes */																																			\
	_MODE(MODE_GNID_RELATIVE	, 0x05	, x)		/* path relative to the initial fabfile */				\
	_MODE(MODE_GNID_CANON			, 0x06	, x)		/* canonical path */															\
/* dependency discovery modes */																															\
	_MODE(MODE_DDSC_DEFERRED	, 0x07	, x)		/* defer dependency discovery until bp prune */		\
	_MODE(MODE_DDSC_UPFRONT		, 0x08	, x)		/* comprehensive dependency discovery upfront */	\

enum {
#define _MODE(a, b, c) a = b,
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a "[" #b "]" :
#define MODE_STR(x) MODE_TABLE(x) "unknown"

extern struct g_args_t
{
//
// execution parameters
//

	pid_t				pid;									// pid of this process
	pid_t				sid;									// session-id
	char *			cwd;									// cwd

	uid_t				ruid;									// real-user-id
	char *			ruid_name;
	uid_t				euid;									// effective-user-id   (must be fabsys)
	char *			euid_name;
	gid_t				rgid;									// real-group-id
	char *			rgid_name;
	gid_t				egid;									// effective-group-id  (must be fabsys)
	char *			egid_name;

//
// arguments
//

	int					mode_exec;						// execution mode
	int					mode_gnid;						// mode for gn identification string
	int					mode_ddsc;						// dependency discovery mode

	int					concurrency;					// concurrently limiting factor

	path *			init_fabfile_path;		// path to initial fabfile
	char *			bakescript_path;			// path to bakescript

	char **			varkeys;							// var keys
	int					varkeysl;

	char **			varvals;							// var values
	int					varvalsl;

	char **			invokedirs;						// root directories for locating invocations
	int					invokedirsl;

	char **			targets;							// targets
	int					targetsl;

	int					invalidationsz;				// invalidate all graph nodes
	char **			invalidations;				// graph nodes to invalidate
	int					invalidationsl;

	int					dumpnodesz;						// invalidate all graph nodes
	char **			dumpnodes;						// graph nodes to dump
	int					dumpnodesl;
} g_args;

//// parse_args
//
// parses command-line options, populating g_args.
//
// Prints the usage statement and terminates the program if options are
// invalid, for example required options are not present, or invalid
// parameters are given to an option
//
// returns zero on failure (malloc failure, for example)
//
int parse_args(int argc, char** argv);

/// args_teardown
//
// free g_args
//
void args_teardown();

#endif
