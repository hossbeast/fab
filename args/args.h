#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

#define DEFAULT_FABFILE 				"fabfile"
#define DEFAULT_EXECDIR_BASE		"/tmp/fab/exec"
#define DEFAULT_HASHDIR					"/tmp/fab/hash"
#define DEFAULT_INVALIDATE_ALL	0
#define DEFAULT_DUMPNODE_ALL		0
#define DEFAULT_MODE_EXEC				MODE_EXEC_FABRICATE
#define DEFAULT_MODE_GNID				MODE_GNID_RELATIVE
#define DEFAULT_MODE_DDSC				MODE_DDSC_DEFERRED

/* modes */

#define MODE_TABLE(x)																																					\
/* execution modes */																																					\
	_MODE(MODE_EXEC_FABRICATE	, 0x00	, x)		/* fabricate targets*/														\
	_MODE(MODE_EXEC_BUILDPLAN	, 0x01	, x)		/* generate buildplan only */											\
/* path display modes */																																			\
	_MODE(MODE_GNID_RELATIVE	, 0x02	, x)		/* path relative to the initial fabfile */				\
	_MODE(MODE_GNID_CANON			, 0x03	, x)		/* canonical path */															\
/* dependency discovery modes */																															\
	_MODE(MODE_DDSC_DEFERRED	, 0x04	, x)		/* defer dependency discovery until bp prune */		\
	_MODE(MODE_DDSC_UPFRONT		, 0x05	, x)		/* complete dependency discovery upfront */				\

enum {
#define _MODE(a, b, c) a = b,
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a "[" #b "]" :
#define MODE_STR(x) MODE_TABLE(x) "unknown"

extern struct g_args_t
{
	pid_t				pid;									// pid of this process
	char				cwd[512];							// current working directory
	int					cwdl;									// length

	int					mode_exec;						// execution mode
	int					mode_gnid;						// mode for gn identification string
	int					mode_ddsc;						// dependency discovery mode

	char **			targets;							// targets
	int					targets_len;

	char *			fabfile;							// user-supplied path to initial fabfile
	char *			fabfile_canon;				// canonical path to initial fabfile
	char *			fabfile_canon_dir;		// canonical path to directory of initial fabfile

	char *			execdir_base;
	char *			execdir;
	char *			hashdir;

	char **			invalidate;						// graph nodes to invalidate
	int					invalidate_len;
	int					invalidate_all;

	char **			dumpnode;							// graph nodes to dump
	int					dumpnode_len;
	int					dumpnode_all;
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
#define parse_args_onfail "argc: '%d', argv: '0x%08x'"
int parse_args(int argc, char** argv);

/// args_teardown
//
// free g_args
//
void args_teardown();

#endif
