/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _BP_H
#define _BP_H

#include "listwise.h"
#include "listwise/transform.h"

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
// SUMMARY
//  create a buildplan culminating in the specified targets
//
// PARAMETERS
//  fabrications   - build plan targets
//  fabricationsl  - len
//  fabricationxs  - buildplan targets (exact)
//  fabricationxsl - len
//  fabricationns  - buildplan targets (nofile-non-traversing)
//  fabricationnsl - len
//
// RETURNS
//  0 in the case of failure, and 1 otherwise
//
int bp_create(
	  gn *** const restrict fabrications
	, int fabricationsl
	, gn *** const restrict fabricationxs
	, int fabricationxsl
	, gn *** const restrict fabricationns
	, int fabricationnsl
	, bp ** const restrict bp
)
	__attribute__((nonnull));

/// bp_eval
//
// SUMMARY
//  evaluate the buildplan, removing nodes that can be determined to not require fabrication
//  then consolidate stages in the buildplan
//
// PARAMETERS
//  bp     - buildplan
//
// RETURNS
//  0 in the case of failure (memory, io, poisoned buildplan), and 1 otherwise
//
int bp_eval(bp * const restrict)
	__attribute__((nonnull));

/// bp_dump
//
// print a bp to stdout
//
void bp_dump(bp *)
	__attribute__((nonnull));

/// bp_exec
//
// executing the build plan, one stage at a time
//
int bp_exec();

/// bp_prepare_stage
//
// SUMMARY
//  prepare the buildplan for execution by writing all of the formulas to a set of temp files
//
int bp_prepare_stage(bp * bp, int stage, transform_parser * const gp, lwx *** stax, int * staxa, int staxp, ts *** ts, int * tsl, int * tsa)
	__attribute__((nonnull));

/// bp_harvest
//
// SUMMARY
//  
//
int bp_harvest_stage(bp * const restrict bp, int stage)
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
