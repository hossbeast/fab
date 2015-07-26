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

struct lwx;               // listwise.h
struct gn;                // gn.h
struct fmleval;           // ts.h
struct ts;                // ts.h
struct transform_parser;  // listwise/transform.h

// build plan
typedef struct bp
{
  // actions to be performed in a single build stage
  struct bp_stage
  {
    struct fmleval ** evals;				// evaluations to be carried out which result in 1 or more
    int					      evalsl;		  	//  SECONDARY/GENERATED files

    struct gn **	    nofmls;				// placeholders for SECONDARY nodes which are needed but which have no fml
    int					      nofmlsl;			//  i.e. these are errors being aggregated

    struct gn **	    primary;			// source files
    int					      primaryl;
  } *   stages;
	int		stagesl;
  int   evalsl;       // sum stages[-].evalsl
} bp;

#define restrict __restrict

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
	  struct gn ** const restrict fabrications
	, int fabricationsl
	, struct gn ** const restrict fabricationxs
	, int fabricationxsl
	, struct gn ** const restrict fabricationns
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
int bp_prepare_stage(
  /*  1 */   bp * bp
  /*  2 */ , int stage
  /*  3 */ , struct transform_parser * const gp
  /*  4 */ , struct lwx *** stax
  /*  5 */ , size_t * staxa
  /*  6 */ , size_t staxp
  /*  7 */ , struct ts *** ts
  /*  8 */ , size_t * tsl
  /*  9 */ , size_t * tsa
)
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
