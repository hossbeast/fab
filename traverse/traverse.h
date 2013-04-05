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

#ifndef _TRAVERSE_H
#define _TRAVERSE_H

#define restrict __restrict

int traverse_init();
void traverse_teardown();

struct gn;

/// traverse_depth_bynodes_needsward_useweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - needs-wise
//   - follow weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

int traverse_depth_bynodes_needsward_useweak_nobridge(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_needsward_noweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - needs-wise
//   - dont follow weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

int traverse_depth_bynodes_needsward_noweak_nobridge(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_needsward_skipweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - needs-wise
//   - dont follow weak dependencies of the root node
//   - follow all other weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_skipweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_feedsward_useweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - feeds-wise
//   - follow weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_feedsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

int traverse_depth_bynodes_feedsward_useweak_nobridge(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_feedsward_noweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - feeds-wise
//   - dont follow weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_feedsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

int traverse_depth_bynodes_feedsward_noweak_nobridge(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_feedsward_skipweak
//
// SUMMARY
//  traverse the graph starting at and including gn, detecting cycles
//   - depth first
//   - feeds-wise
//   - dont follow weak dependencies
//  apply logic at each visited node
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_feedsward_skipweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

#undef restrict
#endif

