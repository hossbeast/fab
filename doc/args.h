#ifndef _ARGS_H
#define _ARGS_H

#define DEFAULT_BUILDFILE "buildfile"

extern struct g_args_t
{
	char**			targets;							// targets
	int					targets_len;

	char*				buildfile;						// path to buildfile
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
