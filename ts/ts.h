#ifndef _TS_H
#define _TS_H

#include <sys/types.h>

#include "gn.h"
#include "fml/fml.h"

#include "pstring.h"

// thread workspace
typedef struct 
{
/*
	gn *			gn;					// graph node being fabricated
*/

	fmleval * fmlv;				// fml evaluation context

	pid_t			pid;				// child pid

	int				cmd_fd;			// fd for cmd file
	pstring *	cmd_path;		// full path to cmd file
	pstring *	cmd_txt;		// txt in cmd file

	int				stdo_fd;		// fd for stdout file
	pstring *	stdo_path;	// full path to stdout file
	pstring *	stdo_txt;		// txt in stdout file

	int				stde_fd;		// fd for stderr file
	pstring *	stde_path;	// full path to stderr file
	pstring *	stde_txt;		// txt in stderr file

	int				r_status;		// exit status
	int				r_signal;		// exit signal
} ts;

int ts_mk(ts ** ts)
	__attribute__((nonnull));

void ts_free(ts * ts);

#endif
