#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "ff.h"
#include "log.h"
#include "control.h"

// signal handling
static int o_signum;
void signal_handler(int signum)
{
	log(L_INFO, "shutdown signal: %d", signum);
	exit(0);
}

int main(int argc, char** argv)
{
	ff_parser *		ffp = 0;
	ff_node *			ffn = 0;

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// ignore most signals
	int x;
	for(x = 0; x < NSIG; x++)
		signal(x, SIG_DFL);

	// handle these signals by terminating gracefully.
	signal(SIGINT		, signal_handler);	// terminate gracefully
	signal(SIGQUIT	, signal_handler);
	signal(SIGTERM	, signal_handler);

	// initialize logger
	fatal(log_init, "+ERROR +WARN +INFO +FFTOKN +FFTREE +FFSTAT");

	// parse the faffile
	fatal(ff_mkparser, &ffp);
	fatal(ff_parse, ffp, "testfile", &ffn);

extern void ff_dump(ff_node * const __restrict);
	ff_dump(ffn);

	return 0;
}
