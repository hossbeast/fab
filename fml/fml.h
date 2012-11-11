#ifndef _FML_H
#define _FML_H

/* fml - formula for fabricating target files */

#include <listwise.h>

#include "ff.h"
#include "gn.h"
#include "ts.h"

#include "coll.h"
#include "map.h"

typedef struct fml
{
	ff_node *		ffn;		// ff_node for this formula
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
int fml_add(ff_node *, fml **);

/// fml_render
//
// render cmd to ts->cmd_txt
//
int fml_render(ts * ts, map * vmap, lstack *** stax, int * stax_l, int * stax_a)
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
