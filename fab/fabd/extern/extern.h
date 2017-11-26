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

#ifndef FABD_EXTERN_H
#define FABD_EXTERN_H

/*

SUMMARY
 "use" and "require" directives are resolved against the node graph, or against the extern graph.

 the extern graph built from directory trees specified in config under the "extern" key.

*/

#include "xapi.h"
#include "types.h"

struct ff_node_pattern;
struct reconfigure_context;
struct value;
struct vertex;

xapi extern_setup(void);
xapi extern_cleanup(void);
xapi extern_report(void);

/// extern_reconfigure
//
// SUMMARY
//  reapply configuration to the extern module
//
// PARAMETERS
/// ctx    - reconfiguration context
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi extern_reconfigure(struct reconfigure_context * restrict ctx, const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

/// extern_reference_resolve
//
// SUMMARY
//  lookup a vertex which was indexed for extern lookup
//
// PARAMETERS
//  ref  - string node for the reference
//  node - (returns) first matching vertex, either belonging to the node graph, or the extern graph
//
xapi extern_reference_resolve(const struct ff_node_pattern * restrict ref, const struct vertex ** restrict node)
  __attribute__((nonnull));

/// extern_index_vertex
//
// SUMMARY
//  index a single vertex for extern lookup
//
// PARAMETERS
//  name  - name of the node (last component of its file path)
//  namel - length of name
//  v     - vertex, either belonging to the node graph, or the extern graph
//
xapi extern_index_vertex(const char * restrict name, size_t namel, struct vertex * restrict v)
  __attribute__((nonnull));

#endif
