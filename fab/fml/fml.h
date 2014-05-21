/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _FML_H
#define _FML_H

/* fml - formula for executing an external process */

#include "listwise.h"
#include "listwise/generator.h"

#include "ffn.h"
#include "ts.h"

#include "coll.h"
#include "strstack.h"
#include "map.h"

#define restrict __restrict

struct fmlctx;			// in ts.h
struct fmleval;			// in ts.h

typedef struct fml
{
	ff_node *					ffn;					// ff_node for this formula

	// a new ctx is created for the fml each time the ff_file in which it resides is invoked
	//  i.e. { variable-set }
	struct fmlctx ** 	ctxs;
	int								ctxsa;
	int								ctxsl;

	// a new eval is created for the fml for each set of targets it can be invoked on behalf of
	//  i.e. { variable-set, target-list }
	struct fmleval **	evals;				// evaluation instances
	int								evalsa;
	int								evalsl;

	/*
	** the var closure of an fml is a flat distinct list of all of the variables
	** whose values could affect the rendered text of the fml (FFN_VARREF nodes)
	*/
	struct ff_node **	closure_vars;
	int								closure_varsa;
	int								closure_varsl;
} fml;

extern union g_fmls_t
{
	coll_doubly c;

	struct
	{
		int			l;
		int			a;
		int			z;

		fml **	e;
	};
} g_fmls;

/// fml_attach
//
// SUMMARY
//  attach a formula to graph nodes
//
int fml_attach(ff_node * const restrict ffn, strstack * const restrict sstk, struct ff_loc ** const restrict loc, const int locl, map * const restrict vmap, generator_parser * const gp, lwx *** const restrict stax, int * const restrict staxa, int * const restrict staxp)
	__attribute__((nonnull));

/// fml_render
//
// SUMMARY
//  ts->fmlv has been set; reset ts->cmd_txt and render the cmd for that evaluation to ts->cmd_txt, including an initial SHE-BANG directive
//
int fml_render(ts * const restrict ts, generator_parser * const gp, lwx *** const restrict stax, int * const restrict staxa, int staxp, map * const restrict rawvars, int shebang)
	__attribute__((nonnull(1, 2, 3, 4)));

/// fml_exec
//
// execute the formula
//
// parameters
//     ts - threadspace
//    num - unique-per-pid number for this execution
//
int fml_exec(ts * const restrict, int num)
	__attribute__((nonnull));

/// fml_teardown
//
// free g_fmls
//
void fml_teardown();

#undef restrict
#endif
