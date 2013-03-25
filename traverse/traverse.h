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
