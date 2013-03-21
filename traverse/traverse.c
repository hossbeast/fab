#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "traverse.h"

#include "gn.h"
#include "log.h"
#include "control.h"
#include "args.h"

#include "xmem.h"
#include "map.h"
#include "cksum.h"

#define restrict __restrict

// map from (uint32) hash of detected cycle -> (int) number of times that particular cycle was encountered
static map * o_cycles;

/// raise_cycle
//
// SUMMARY
//  handle the fact that a cycle was detected in the dependency graph
//
// PARAMETERS
//  stack   - stack of nodes representing the cycle
//  num     - number of elements set in stack
//
static int raise_cycle(gn * (* const stack)[64], int num)
{
	void logcycle(int top)
	{
		int x;
		for(x = top; x >= 0; x--)
		{
			if(x != top)
				log_add(" -> ");

			log_add("%s", (*stack)[x]->idstring);
		}

		if(num == (sizeof((*stack)) / sizeof((*stack)[0])))
			log_finish(" -> ...");
		else
			log_finish("");
	};

	if(g_args.mode_cycl == MODE_CYCL_DEAL)
		return 1;

	// skip the beginning of the traversal, to the start of the cycle
	int top;
	for(top = num - 1; top >= 0; top--)
	{
		if((*stack)[top] == (*stack)[0])
			break;
	}

	if(g_args.mode_cycl == MODE_CYCL_FAIL)
	{
		log_start(L_ERROR, "detected cycle : ");
		logcycle(top);
		return 0;
	}
	else if(g_args.mode_cycl == MODE_CYCL_WARN)
	{
		if(!o_cycles)
			fatal(map_create, &o_cycles, 0);

		// hash this cycle
		uint32_t cyc = cksum((*stack), top * sizeof(*(*stack)));

		// get or set the hash into the map
		int * c = 0;
		if((c = map_get(o_cycles, MM(cyc))) == 0)
		{
			int cc = 0;
			c = map_set(o_cycles, MM(cyc), MM(cc));
		}

		if((*c)++ == 0)
		{
			log_start(L_WARN, "detected cycle : ");
			logcycle(top);
		}

		return 1;
	}

	finally : coda;
}

/// enter
//
// SUMMARY
//  traversal
//
// RETURNS
//  0 - success
//  1 - cycle detected
// -1 - callback failure
//
static int enter(gn * const n, size_t rel_off, int d, int weak, gn * (* const stack)[64], int * num, int (*const logic)(struct gn *, int))
{
	if(n->guard)	// cycle
	{
		if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
			(*stack)[(*num)++] = n;

		return 1;
	}

	union relations * rels = (void*)(((char*)n) + rel_off);

	n->guard = 1;	// descend

	int x;
	for(x = 0; x < rels->l; x++)
	{
		if(!rels->e[x]->weak || weak)
		{
			int e = enter(rels->e[x]->B, rel_off, d + 1, weak, stack, num, logic);
			if(e == 1)
			{
				if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
					(*stack)[(*num)++] = n;

				n->guard = 0;
				return 1;
			}
			else if(e == -1)
			{
				n->guard = 0;
				return -1;
			}
		}
	}

	n->guard = 0;	// ascend

	if(logic(n, d) == 0)
		return -1;

	return 0;
}

///
/// public
///

int traverse_depth_bynodes_needsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	int e = enter(r, offsetof(typeof(*r), needs), 0, 1, &stack, &num, logic);

	if(e == 1)
	{
		// cycle detected
		return raise_cycle(&stack, num);
	}
	else if(e == -1)
	{
		// callback failure
		return 0;
	}

	// success
	return 1;
}

int traverse_depth_bynodes_needsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	int e = enter(r, offsetof(typeof(*r), needs), 0, 0, &stack, &num, logic);

	if(e == 1)
	{
		// cycle detected
		return raise_cycle(&stack, num);
	}
	else if(e == -1)
	{
		// callback failure
		return 0;
	}

	// success
	return 1;
}

int traverse_depth_bynodes_feedsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	int e = enter(r, offsetof(typeof(*r), feeds), 0, 1, &stack, &num, logic);

	if(e == 1)
	{
		// cycle detected
		return raise_cycle(&stack, num);
	}
	else if(e == -1)
	{
		// callback failure
		return 0;
	}

	// success
	return 1;
}

int traverse_depth_bynodes_feedsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	int e = enter(r, offsetof(typeof(*r), feeds), 0, 0, &stack, &num, logic);

	if(e == 1)
	{
		// cycle detected
		return raise_cycle(&stack, num);
	}
	else if(e == -1)
	{
		// callback failure
		return 0;
	}

	// success
	return 1;
}
