#ifndef _BP_H
#define _BP_H

#include <listwise.h>

#include "gn.h"
#include "fml.h"
#include "ts.h"

#include "map.h"

#define restrict __restrict

// actions to be performed in a single build stage
typedef struct bp_stage
{
	fmleval **	evals;				// evaluations to be carried out which result in 1 or more
	int					evals_l;			//  SECONDARY/GENERATED files

	gn **				nofmls;				// placeholders for SECONDARY nodes which are needed but which have no fml
	int					nofmls_l;			//  i.e. these are errors being aggregated

	gn **				primary;			// source files
	int					primary_l;
} bp_stage;

// build plan
typedef struct
{
	bp_stage	* stages;
	int					stages_l;
} bp;

/// bp_create
//
// create a bp for building the specified goals
//
int bp_create(gn ** goals, int goals_len, bp ** bp);

/// bp_eval
//
// SUMMARY
//  evaluate the buildplan, removing nodes that can be determined to not require fabrication
//  then consolidate stages in the buildplan
//
// PARAMETERS
//  bp     - buildplan
//  poison - set to 1 if the bp cannot be executed (ex: missing source files)
//
// RETURNS
//  0 in the case of failure (memory, io), and 1 otherwise
//  check poison to determine whether the buildplan can be executed
//
int bp_eval(bp * const restrict, int * const restrict)
	__attribute__((nonnull));

/// bp_dump
//
// print a bp to stdout
//
void bp_dump(bp *);

/// bp_flatten
//
//
//
int bp_flatten(bp * bp, gn *** gns, int * gnl, int * gna)
	__attribute__((nonnull));

/// bp_exec
//
// execute the build plan one stage at a time
//
int bp_exec(bp * bp, map * vmap, lstack *** stax, int * staxa, int staxp, ts *** ts, int * tsa, int * tsw)
	__attribute__((nonnull));

/// bp_free
//
// SUMMARY
//  free a bp with free semantics
//
void bp_free(bp * const restrict bp);

/// bp_xfree
//
// SUMMARY
//  free a bp with xfree semantics
//
void bp_xfree(bp ** const restrict bp)
	__attribute__((nonnull));

#undef restrict
#endif
