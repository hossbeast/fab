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

#ifndef FAB_TRAVERSE_H
#define FAB_TRAVERSE_H

#define restrict __restrict

int traverse_init();
void traverse_teardown();

struct gn;

/// traverse
//
// _depth     - depth first (visit a nodes children before itself)
// _breadth   - breadth first (visit a node before its children)
//
// _bynodes   - apply logic at nodes
// _byrels    - apply logic at relations
//
// _needsward - needs-wise
// _feedsward - feeds-wise
//
// _useweak   - traverse weak relations
// _noweak    - ignore weak relations
// _skipweak  - traverse weak relations, but do not invoke logic on consequent nodes
//
// _usebridge - traverse bridge relations
// _nobridge  - ignore bridge relations
//
// _usenofile - traverse nofile boundaries
// _nonofile  - ignore nofile boundaries
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_noweak_nobridge_usenofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_noweak_nobridge_nonofile     (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_noweak_usebridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_noweak_usebridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_useweak_nobridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_useweak_nobridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_useweak_usebridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_useweak_usebridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_skipweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_skipweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_skipweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_needsward_skipweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_noweak_nobridge_usenofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_noweak_nobridge_nonofile     (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_noweak_usebridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_noweak_usebridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_useweak_nobridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_useweak_nobridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_useweak_usebridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_useweak_usebridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_skipweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_skipweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_skipweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_depth_bynodes_feedsward_skipweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));

int traverse_breadth_bynodes_needsward_noweak_nobridge_usenofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_noweak_nobridge_nonofile     (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_noweak_usebridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_noweak_usebridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_useweak_nobridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_useweak_nobridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_useweak_usebridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_useweak_usebridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_skipweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_skipweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_skipweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_needsward_skipweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_noweak_nobridge_usenofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_noweak_nobridge_nonofile     (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_noweak_usebridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_noweak_usebridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_useweak_nobridge_usenofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_useweak_nobridge_nonofile    (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_useweak_usebridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_useweak_usebridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_skipweak_nobridge_usenofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_skipweak_nobridge_nonofile   (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_skipweak_usebridge_usenofile (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));
int traverse_breadth_bynodes_feedsward_skipweak_usebridge_nonofile  (struct gn * const restrict r, int (* const logic)(struct gn *, int)) __attribute__((nonnull));

#undef restrict
#endif
