#ifndef _FML_H
#define _FML_H

/* fml - formula for executing an external process */

#include <listwise.h>

#include "ff.h"
#include "gn.h"
#include "ts.h"

#include "coll.h"
#include "strstack.h"
#include "map.h"

struct fmleval;

typedef struct fml
{
	ff_node *					ffn;					// ff_node for this formula

	struct fmleval *	evals;				// evaluation instances
	int								evals_l;
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

/// fml_add
//
// add a formula to the global list from the fabfile node
//
int fml_add(ff_node * ffn, strstack * sstk, map * vmap, lstack *** stax, int * stax_a, int p)
	__attribute__((nonnull));

/// fml_render
//
// render cmd to ts->cmd_txt
//
int fml_render(ts * ts, map * vmap, lstack *** stax, int * stax_a, int p, int standalone)
	__attribute__((nonnull));

/// fml_exec
//
// execute the formula
//
// parameters
//     ts - threadspace
//    num - unique-per-pid number for this execution
//
int fml_exec(ts *, int num)
	__attribute__((nonnull));

/// fml_teardown
//
// free g_fmls
//
void fml_teardown();

#endif
