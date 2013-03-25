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
//  stack   - stack of nodes comprising the cycle in reverse traversal order
//            i.e. [0] is the last node visited, [n] is the first
//  num     - number of elements in stack
//
// RETURNS
//  1 - success; program execution may continue
//  0 - failure; program execution must cease
//
static int raise_cycle(gn * (* const stack)[64], int stackl)
{
	void logcycle()
	{
		int x;
		for(x = stackl - 1; x >= 0; x--)
		{
			if(x != stackl)
				log_add(" -> ");

			log_add("%s", (*stack)[x]->idstring);
		}

		if(stackl == (sizeof((*stack)) / sizeof((*stack)[0])))
			log_finish(" -> ...");
		else
			log_finish("");
	};

	if(g_args.mode_cycl == MODE_CYCL_FAIL || g_args.mode_cycl == MODE_CYCL_WARN)
	{
		// skip the beginning of the traversal, to the start of the cycle
		if(g_args.mode_cycl == MODE_CYCL_FAIL)
		{
			log_start(L_ERROR, "detected cycle(%d) : ", stackl);
			logcycle();
			return 0;
		}
		else if(g_args.mode_cycl == MODE_CYCL_WARN)
		{
			// hash this cycle
			uint32_t cyc = cksum((*stack), stackl * sizeof(*(*stack)));

			// get or set the hash into the map
			int * c = 0;
			if((c = map_get(o_cycles, MM(cyc))) == 0)
			{
				int cc = 0;
				c = map_set(o_cycles, MM(cyc), MM(cc));
			}

			if((*c)++ == 0)
			{
				log_start(L_WARN, "detected cycle(%d) : ", stackl);
				logcycle();
			}
		}
	}

	return 1;
}

/// enter
//
// SUMMARY
//  traversal
//
// RETURNS
//  1 - success
//  0 - callback failure, or fatal cycle
// -1 - cycle detection (internal)
//
static int enter(gn * const n, size_t rel_off, int d, int weak, int nextweak, gn * (* const stack)[64], int * num, int (*const logic)(struct gn *, int), int t)
{
	if(n->guard)	// cycle
	{
		(*num) = 0;
		if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
			(*stack)[(*num)++] = n;

		return -1;
	}

	union relations * rels = (void*)(((char*)n) + rel_off);

	n->travel = t;	// descend
	n->guard = 1;

	int x;
	for(x = 0; x < rels->l; x++)
	{
		if(!rels->e[x]->weak || weak)
		{
			if(rels->e[x]->B->travel != t)
			{
				int e = enter(rels->e[x]->B, rel_off, d + 1, nextweak, nextweak, stack, num, logic, t);
				if(e == -1)
				{
					if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
						(*stack)[(*num)++] = n;

					if((*stack)[0] == n)
					{
						if(raise_cycle(stack, (*num)) == 0)
						{
							n->guard = 0;
							return 0;
						}
					}
					else
					{
						// continue unwinding
						n->guard = 0;
						return -1;
					}
				}
				else if(e == 0)
				{
					n->guard = 0;
					return 0;
				}
			}
		}
	}

	n->guard = 0;	// ascend

	if(logic(n, d) == 0)
		return 0;

	return 1;
}

///
/// public
///

static int o_t;

int traverse_depth_bynodes_needsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), needs), 0, 1, 1, &stack, &num, logic, ++o_t);
}

int traverse_depth_bynodes_needsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), needs), 0, 0, 0, &stack, &num, logic, ++o_t);
}

int traverse_depth_bynodes_needsward_skipweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), needs), 0, 0, 1, &stack, &num, logic, ++o_t);
}

int traverse_depth_bynodes_feedsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), feeds), 0, 1, 1, &stack, &num, logic, ++o_t);
}

int traverse_depth_bynodes_feedsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), feeds), 0, 0, 0, &stack, &num, logic, ++o_t);
}

int traverse_depth_bynodes_feedsward_skipweak(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	gn * stack[64] = {};
	int num = 0;

	return enter(r, offsetof(typeof(*r), feeds), 0, 0, 1, &stack, &num, logic, ++o_t);
}

int traverse_init()
{
	fatal(map_create, &o_cycles, 0);

	finally : coda;
}

void traverse_teardown()
{
	map_free(o_cycles);
}
