#ifndef _TRAVERSE_H
#define _TRAVERSE_H

#define restrict __restrict

struct gn;

/// traverse_depth_bynodes_needsward_useweak
//
// SUMMARY
//  traverse the graph starting at gn, detecting cycles
//   - depth first
//   - needs-wise
//   - include weak dependencies
//  apply logic at each node along the way
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_needsward_noweak
//
// SUMMARY
//  traverse the graph starting at gn, detecting cycles
//   - depth first
//   - needs-wise
//   - ignore weak dependencies
//  apply logic at each node along the way
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_needsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_feedsward_useweak
//
// SUMMARY
//  traverse the graph starting at gn, detecting cycles
//   - depth first
//   - feeds-wise
//   - include weak dependencies
//  apply logic at each node along the way
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_feedsward_useweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

/// traverse_depth_bynodes_feedsward_noweak
//
// SUMMARY
//  traverse the graph starting at gn, detecting cycles
//   - depth first
//   - feeds-wise
//   - ignore weak dependencies
//  apply logic at each node along the way
//
// LOGIC
//  returns 0 to stop the traversal with failure
//
// RETURNS
//  returns 0 if an error was encountered (including a cycle or failed invocation of the logic callback)
//  returns 1 on success
//
int traverse_depth_bynodes_feedsward_noweak(struct gn * const restrict r, int (* const logic)(struct gn *, int));

#undef restrict
#endif
