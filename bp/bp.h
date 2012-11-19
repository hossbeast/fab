#ifndef _BP_H
#define _BP_H

#include <listwise.h>

/* bp - build plan */
#include "gn.h"
#include "fml.h"

#include "map.h"

// actions to be performed in a single build stage
typedef struct 
{
	fmleval **	evals;
	int					evals_l;
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
int bp_exec(bp * bp, map * vmap, lstack *** stax, int * lstax_l, int * stax_a, int p)
	__attribute__((nonnull));

#endif
