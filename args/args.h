#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

#define DEFAULT_FABFILE "fabfile"
#define DEFAULT_EXECDIR		"/tmp/fab/exec"
#define DEFAULT_HASHDIR		"/tmp/fab/hash"

/* modes */

#define MODE_DEFAULT			MODE_FABRICATE
#define MODE_TABLE(x)																													\
	_MODE(MODE_FABRICATE			, 0x00	, x)		/* fabricate targets - default */	\
	_MODE(MODE_BUILDPLAN			, 0x01	, x)		/* generate buildplan only */			\

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

	char **			targets;							// targets
	int					targets_len;

	char				mode;									// one of MODE_*

	char *			fabfile;							// path to fabfile

	char *			execdir_base;
	char *			execdir;
	char *			hashdir;
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
