#ifndef _TS_H
#define _TS_H

#include <stdint.h>
#include <sys/types.h>

#include "gn.h"
#include "ff.h"

#include "pstring.h"

struct fml;

/// fmleval
//
// an instance of a formula evaluation with a list of products
//
typedef struct fmleval
{
	struct fml *	fml;					// the formula

	struct											// products expected when executing the formula
	{
		gn **					products;
		int						products_l;
	};
} fmleval;

// thread workspace
typedef struct 
{
	ff_parser *				ffp;					// ff parser
	ff_node *					ffn;					// ff node

	struct fmleval *	fmlv;					// fml evaluation context

	pid_t							pid;					// child pid

	int								cmd_fd;				// fd for cmd file
	pstring *					cmd_path;			// full path to cmd file
	pstring *					cmd_txt;			// txt in cmd file

	int								stdo_fd;			// fd for stdout file
	pstring *					stdo_path;		// full path to stdout file
	pstring *					stdo_txt;			// txt in stdout file

	int								stde_fd;			// fd for stderr file
	pstring *					stde_path;		// full path to stderr file
	pstring *					stde_txt;			// txt in stderr file

	int								r_status;			// exit status
	int								r_signal;			// exit signal

	int y;
} ts;

/// ts_ensure
//
// SUMMARY
//  ensure that at least n threadspaces are allocated
//  then call ts_reset on the first n threadspaces
//
int ts_ensure(ts *** ts, int * tsa, int n)
	__attribute__((nonnull));

/// ts_reset
//
// reset all threadspace properties without freeing underlying resources
// so that it can be used again
//
void ts_reset(ts * ts)
	__attribute__((nonnull));

/// ts_execwave
//
// SUMMARY
//  execute a set of formulas each associated with a threadspace
//
// PARAMETERS
//  ts  - ptr to threadspaces
//  n   - number of threadspaces
//  id  - process-unique identifier for this wave of threadspace executions
//  no  - wave number for logging (buildplan stage, etc)
//  hi  - log tags to associate with high-level logging of this execution wave
//  lo  - log tags to associate with low-level logging of this execution wave
//  res - set to 1 if all formulas executed successfully
//
int ts_execwave(ts ** ts, int n, int * waveid, int waveno, uint64_t hi, uint64_t lo, int * res)
	__attribute__((nonnull));

void ts_free(ts * ts);

#endif
