#ifndef _BP_H
#define _BP_H

#include <listwise.h>

/* bp - build plan */
#include "gn.h"
#include "fml.h"
#include "ts.h"

#include "map.h"

// actions to be performed in a single build stage
typedef struct 
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

/// bp_prune
//
// remove nodes that do not require updating
//
int bp_prune(bp * bp);

/// bp_dump
//
// print a bp to stdout
//
void bp_dump(bp *);

/// bp_exec
//
// execute the build plan one stage at a time
//
int bp_exec(bp * bp, map * vmap, lstack *** stax, int * lstax_l, int * stax_a, int p, ts *** ts, int * tsa, int * tsw)
	__attribute__((nonnull));

#endif
