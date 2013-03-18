#ifndef _FML_H
#define _FML_H

/* fml - formula for executing an external process */

#include <listwise.h>

#include "ffn.h"
#include "ts.h"

#include "coll.h"
#include "strstack.h"
#include "map.h"

#define restrict __restrict

struct fmleval;	// in ts.h

typedef struct fml
{
	ff_node *					ffn;					// ff_node for this formula

	struct fmleval **	evals;				// evaluation instances
	int								evalsa;
	int								evalsl;

	// the number of bags associated with an fml is the number of different times that
	// the ff_file it is in is invoked
	map ** 						bags;
	int								bagsa;
	int								bagsl;

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
int fml_attach(ff_node * const restrict ffn, strstack * const restrict sstk, map * const restrict vmap, lstack *** const restrict stax, int * const restrict staxa, int staxp)
	__attribute__((nonnull));

/// fml_render
//
// SUMMARY
//  ts->fmlv has been set; render the cmd for that evaluation to ts->cmd_txt
//
int fml_render(ts * const restrict ts, lstack *** const restrict stax, int * const restrict staxa, int staxp, int standalone)
	__attribute__((nonnull));

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
