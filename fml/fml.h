#ifndef _FML_H
#define _FML_H

/* fml - formula for fabricating target files */

#include <listwise.h>

#include "ff.h"
#include "gn.h"
#include "ts.h"

#include "coll.h"
#include "map.h"

#define FMLEVAL_SINGLE		0x00
#define FMLEVAL_MULTI			0x01

struct fml;

/// fmleval
//
// an instance of a formula evaluation with a list of products
//
typedef struct fmleval
{
	struct fml *	fml;				// the formula

	uint8_t				type;				// one of FMLEVAL_*

	struct										// FMLEVAL_MULTI
	{
		gn **				products;
		int					products_l;
	};
} fmleval;

typedef struct fml
{
	ff_node *		ffn;					// ff_node for this formula

	fmleval *		evals;				// evaluation instances
	int					evals_l;
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
int fml_add(ff_node * ffn, lstack * ls)
	__attribute__((nonnull));

/// fml_render
//
// render cmd to ts->cmd_txt
//
int fml_render(ts * ts, map * vmap, lstack *** stax, int * stax_l, int * stax_a, int p)
	__attribute__((nonnull));

/// fml_exec
//
// execute the formula
//
// parameters
//     ts - threadspace
//    num - unique number for this execution
//
int fml_exec(ts *, int num)
	__attribute__((nonnull));

#endif
