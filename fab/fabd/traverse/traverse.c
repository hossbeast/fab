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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "traverse.h"

#include "gn.h"
#include "logs.h"
#include "global.h"
#include "args.h"

#include "xlinux.h"
#include "map.h"
#include "cksum.h"

#define restrict __restrict

// map from hash of detected cycle -> (int) number of times that particular cycle was encountered
static map * o_cycles;

/// raise_cycle
//
// SUMMARY
//  handle the fact that a cycle was detected in the dependency graph
//
// REMARKS
//  weak arcs do not participate in cycles
//
// PARAMETERS
//  stack   - stack of nodes comprising the cycle in reverse traversal order
//            i.e. [1] is the last node visited, [n] is the first
//            also the incident node is stored at [0]
//  num     - number of elements in stack
//
// RETURNS
//  1 - success; program execution may continue
//  0 - failure; program execution must cease
//
static int raise_cycle(gn * (* const stack)[64], int stackl, int * proceed)
{
	void logcycle()
	{
		int x;
		for(x = stackl - 1; x >= 0; x--)
		{
			if(x != stackl)
				logs(0, " -> ");

			logf(0, "%s", (*stack)[x]->idstring);
		}

		if(stackl == (sizeof((*stack)) / sizeof((*stack)[0])))
			logs(0, " -> ...");

		log_finish();
	};

	(*proceed) = 1;
	if(g_args->mode_cycles == MODE_CYCLES_FAIL || g_args->mode_cycles == MODE_CYCLES_WARN)
	{
		// skip the beginning of the traversal, to the start of the cycle
		if(g_args->mode_cycles == MODE_CYCLES_FAIL)
		{
			if(log_start(L_ERROR))
			{
				logf(L_ERROR, "detected cycle(%d) : ", stackl - 1);
				logcycle();
			}
			(*proceed) = 0;
		}
		else if(g_args->mode_cycles == MODE_CYCLES_WARN)
		{
			/*
			** hash this cycle using an order-invariant hash
			** the cost of collisions is low : cycles are reported on more than once
			*/
			uint64_t cyc = 0;
			int x;
			for(x = 1; x < stackl; x++)
				cyc += (uint64_t)(uintptr_t)(*stack)[x];

			// get or set the hash into the map
			int * c = 0;
			if((c = map_get(o_cycles, MM(cyc))) == 0)
			{
				int cc = 0;
				fatal(map_set, o_cycles, MM(cyc), MM(cc), &c);
			}

			if((*c)++ == 0)
			{
				if(log_start(L_WARN))
				{
					logf(L_WARN, "%lu detected cycle(%d) : ", cyc, stackl - 1);
					logcycle();
				}
			}
		}
	}

	finally : coda;
}

/// enter
//
// SUMMARY
//  traversal
//
// PARAMETERS
//  n           - node
//  isweak			- whether this node was reached via a weak relation
//  rel_off     - offset to collection of relations
//  rel_mem_off - offset to relation member
//  d           - depth
//  weak        - weak traversal mode (0=no, 1=yes, 2=skip)
//  bridge      - traverse bridges
//  nofile      - traverse nofile boundaries
//  stack       - cycle stack
//  num         - len of stack
//  logic       - callback
//  invoke      - whether to invoke the callback on this node
//  before      - breadth-first traversal, depth-first otherwise
//  t           - traversal id
//
// RETURNS
//  2 - failure down the stack (user callback or map_set)
//  1 - fatal cycle
//  0 - success
// -1 - cycle detection (internal)
//
static int enter(
	  gn * const restrict n
	, const int isweak
	, const size_t rel_off
	, const size_t rel_mem_off
	, int (* const logic)(struct gn *, int)
	, const int weak
	, const int bridge
	, const int nofile
	, const int before
	, const int invoke
	, const int d
	, gn * (* const restrict stack)[64]
	, int * restrict num
	, const int t
)
{
	if(n->guard)	// cycle detected
	{
		// always include the incident node
		(*num) = 0;
		if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
			(*stack)[(*num)++] = n;

		// add each node not reached via a weak dependency
		if(!isweak)
		{
			if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
				(*stack)[(*num)++] = n;
		}

		return -1;
	}

	union relations * rels = (void*)(((char*)n) + rel_off);

	if(before && invoke)
	{
		prologue;

		fatal(logic, n, d);

		int R = 0;
		finally : conclude(&R);

		if(R)
			return 2;
	}

	// each node may be travelled at most twice, once where it is not being invoked upon
	// and once where it is being invoked upon
	if(invoke)
		n->travel0 = t;
	else
		n->travel1 = t;

	n->guard = 1;	// descend

	int x;
	for(x = 0; x < rels->l; x++)
	{
		if(weak || !rels->e[x]->weak)
		{
			if(bridge || !rels->e[x]->bridge)
			{
				gn * next = *(gn**)(((char*)rels->e[x]) + rel_mem_off);

				if(nofile || ((n->type & GN_TYPE_HASFILE) ^ (next->type & GN_TYPE_HASFILE)) == 0)
				{
					// ninvoke : whether the next node is to be invoked upon
					int ninvoke = weak == 2 && rels->e[x]->weak ? 0 : 1;

					// check the travel guard
					if((ninvoke && next->travel0 != t) || (!ninvoke && next->travel1 != t))
					{
						int e = enter(
								next
							, rels->e[x]->weak
							, rel_off
							, rel_mem_off
							, logic
							, weak
							, bridge
							, nofile
							, before
							, ninvoke
							, d + (rels->e[x]->weak ? 0 : 1)		// depth only increases for a non-weak arc
							, stack
							, num
							, t
						);
						if(e == -1)
						{
							if(!isweak)
							{
								if((*num) < sizeof((*stack)) / sizeof((*stack)[0]))
									(*stack)[(*num)++] = n;
							}

							if((*stack)[0] == n)
							{
								int proceed = 1;

								// raise the cycle if there is at least one non-incident participating node
								if((*num) > 2)
								{
									if(g_args->mode_cycles == MODE_CYCLES_FAIL || g_args->mode_cycles == MODE_CYCLES_WARN)
									{
										prologue;
										fatal(raise_cycle, stack, (*num), &proceed);

										int R = 0;
										finally : conclude(&R);

										if(R)
										{
											n->guard = 0;
											return 2;		// return other error
										}
									}
								}

								if(proceed == 0)
								{
									n->guard = 0;
									return 1;			// return fatal cycle
								}
							}
							else
							{
								n->guard = 0;
								return -1;			// continue unwinding
							}
						}
						else if(e)
						{
							n->guard = 0;
							return e;					// propagate
						}
					}
				}
			}
		}
	}

	n->guard = 0;	// ascend

	if(!before && invoke)
	{
		prologue;

		fatal(logic, n, d);

		int R = 0;
		finally : conclude(&R);

		if(R)
			return 2;
	}

	return 0;
}

///
/// public
///

static int traverse(
	  gn * const restrict n
	, const size_t rel_off
	, const size_t rel_mem_off
	, int (* const logic)(struct gn *, int)
	, const int weak
	, const int bridge
	, const int nofile
	, const int before
)
{
	static int o_t;

	gn * stack[64] = {};
	int num = 0;

	int r = enter(
	  	n
		, 0							// isweak
		, rel_off
		, rel_mem_off
		, logic
		, weak
		, bridge
		, nofile
		, before
		, 1							// invoke
		, 0							// depth
		, &stack
		, &num
		, ++o_t
	);

	if(r == 2)
		fail(0);					// user callback raised an error
	if(r == 1)
		fail(FAB_CYCLE);	// fatal cycle detected

	finally : coda;
}

int traverse_depth_bynodes_needsward_noweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 0, 0, 0);
}
int traverse_depth_bynodes_needsward_noweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 0, 1, 0);
}
int traverse_depth_bynodes_needsward_noweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 1, 0, 0);
}
int traverse_depth_bynodes_needsward_noweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 1, 1, 0);
}
int traverse_depth_bynodes_needsward_useweak_nobridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 0, 0, 0);
}
int traverse_depth_bynodes_needsward_useweak_nobridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 0, 1, 0);
}
int traverse_depth_bynodes_needsward_useweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 1, 0, 0);
}
int traverse_depth_bynodes_needsward_useweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 1, 1, 0);
}
int traverse_depth_bynodes_needsward_skipweak_nobridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 0, 0, 0);
}
int traverse_depth_bynodes_needsward_skipweak_nobridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 0, 1, 0);
}
int traverse_depth_bynodes_needsward_skipweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 1, 0, 0);
}
int traverse_depth_bynodes_needsward_skipweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 1, 1, 0);
}
int traverse_depth_bynodes_feedsward_noweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 0, 0, 0);
}
int traverse_depth_bynodes_feedsward_noweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 0, 1, 0);
}
int traverse_depth_bynodes_feedsward_noweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 1, 0, 0);
}
int traverse_depth_bynodes_feedsward_noweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 1, 1, 0);
}
int traverse_depth_bynodes_feedsward_useweak_nobridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 0, 0, 0);
}
int traverse_depth_bynodes_feedsward_useweak_nobridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 0, 1, 0);
}
int traverse_depth_bynodes_feedsward_useweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 1, 0, 0);
}
int traverse_depth_bynodes_feedsward_useweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 1, 1, 0);
}
int traverse_depth_bynodes_feedsward_skipweak_nobridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 0, 0, 0);
}
int traverse_depth_bynodes_feedsward_skipweak_nobridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 0, 1, 0);
}
int traverse_depth_bynodes_feedsward_skipweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 1, 0, 0);
}
int traverse_depth_bynodes_feedsward_skipweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 1, 1, 0);
}

int traverse_breadth_bynodes_needsward_noweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 0, 0, 1);
}
int traverse_breadth_bynodes_needsward_noweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 0, 1, 1);
}
int traverse_breadth_bynodes_needsward_noweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 1, 0, 1);
}
int traverse_breadth_bynodes_needsward_noweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 0, 1, 1, 1);
}
int traverse_breadth_bynodes_needsward_useweak_nobridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 0, 0, 1);
}
int traverse_breadth_bynodes_needsward_useweak_nobridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 0, 1, 1);
}
int traverse_breadth_bynodes_needsward_useweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 1, 0, 1);
}
int traverse_breadth_bynodes_needsward_useweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 1, 1, 1, 1);
}
int traverse_breadth_bynodes_needsward_skipweak_nobridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 0, 0, 1);
}
int traverse_breadth_bynodes_needsward_skipweak_nobridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 0, 1, 1);
}
int traverse_breadth_bynodes_needsward_skipweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 1, 0, 1);
}
int traverse_breadth_bynodes_needsward_skipweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), needs), offsetof(typeof(*r->needs.e[0]), B), logic, 2, 1, 1, 1);
}
int traverse_breadth_bynodes_feedsward_noweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 0, 0, 1);
}
int traverse_breadth_bynodes_feedsward_noweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 0, 1, 1);
}
int traverse_breadth_bynodes_feedsward_noweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 1, 0, 1);
}
int traverse_breadth_bynodes_feedsward_noweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 0, 1, 1, 1);
}
int traverse_breadth_bynodes_feedsward_useweak_nobridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 0, 0, 1);
}
int traverse_breadth_bynodes_feedsward_useweak_nobridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 0, 1, 1);
}
int traverse_breadth_bynodes_feedsward_useweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 1, 0, 1);
}
int traverse_breadth_bynodes_feedsward_useweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 1, 1, 1, 1);
}
int traverse_breadth_bynodes_feedsward_skipweak_nobridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 0, 0, 1);
}
int traverse_breadth_bynodes_feedsward_skipweak_nobridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 0, 1, 1);
}
int traverse_breadth_bynodes_feedsward_skipweak_usebridge_nonofile (struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 1, 0, 1);
}
int traverse_breadth_bynodes_feedsward_skipweak_usebridge_usenofile(struct gn * const restrict r, int (* const logic)(struct gn *, int))
{
	xproxy(traverse, r, offsetof(typeof(*r), feeds), offsetof(typeof(*r->feeds.e[0]), A), logic, 2, 1, 1, 1);
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
