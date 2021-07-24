/* Copyright (c) 2012-2021 Todd Freed <todd.freed@gmail.com>

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

#include "types.h"

#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "rule_module.h"
#include "dependency.h"
#include "module.h"
#include "rule.h"

static llist rme_list = LLIST_INITIALIZER(rme_list);
static llist rme_freelist = LLIST_INITIALIZER(rme_freelist);

//
// static
//

static void rule_module_edge_release(rule_module_edge * restrict rme)
{
  llist_delete_node(&rme->edge.owner);
  llist_append(&rme_freelist, rme, edge.owner);
}

void rule_module_edge_alloc(rule_module_edge ** restrict rmep)
{
  rule_module_edge *rme;

  if((rme = llist_shift(&rme_freelist, typeof(*rme), edge.owner)))
  {
    memset(rme, 0, sizeof(*rme));
  }
  else
  {
    xmalloc(&rme, sizeof(*rme));
  }

  rbnode_init(&rme->nohits_rbn);
  llist_init_node(&rme->changed[0].lln);
  llist_init_node(&rme->changed[1].lln);
  llist_init_node(&rme->dependencies);

  llist_append(&rme_list, rme, edge.owner);

  *rmep = rme;
}

//
// public
//

void rule_module_cleanup(void)
{
  llist *T;
  rule_module_edge *rme;

  llist_foreach_safe(&rme_list, rme, edge.owner, T) {
    rule_module_edge_release(rme);
  }

  llist_foreach_safe(&rme_freelist, rme, edge.owner, T) {
    wfree(rme);
  }
}

void rule_module_disconnect(rule_module_edge * restrict rme, graph_invalidation_context * restrict invalidation)
{
  dependency *dep;

  /* remove dependency edges created by this rma */
  llist_foreach(&rme->dependencies, dep, dependencies_lln) {
    dependency_disconnect(dep, invalidation);
  }

  /*
   * Remove this rma from the run-queue. It might be in the rma run queue because the rule was attached
   * to a directory which was changed while unloading the module
   */
  if(llist_attached(rme, changed[0].lln)) {
    llist_delete(rme, changed[0].lln);
  }

  if(llist_attached(rme, changed[1].lln)) {
    llist_delete(rme, changed[1].lln);
  }

  if(llist_attached(rme, lln_rmas_owner)) {
    llist_delete(rme, lln_rmas_owner);
  }

  graph_disconnect(&rme->edge);
  rule_module_edge_release(rme);
}

void rule_module_connect(
    rule_module_edge ** restrict rmep
  , module * mod
  , module * mod_owner
  , rule * restrict r
  , set * restrict variants
)
{
  moria_edge *e;
  rule_module_edge *rme = 0;
  moria_connect_context ctx;
  int rv;

  rv = moria_preconnect(&ctx, &g_graph, &mod->vertex, &r->vertex, EDGE_MOD_RULE, &e);
  if(rv == MORIA_HASEDGE)
  {
    *rmep = containerof(e, rule_module_edge, edge);
    goto XAPI_FINALLY;
  }
  RUNTIME_ASSERT(rv == MORIA_NOEDGE);
  rule_module_edge_alloc(&rme);
  graph_connect(&ctx, &mod->vertex, &r->vertex, &rme->edge, EDGE_MOD_RULE);

  rme->rule = r;
  rme->mod = mod;
  rme->mod_owner = mod_owner;
  rme->variants = variants;

  /* also store in the owner module */
  llist_append(&mod_owner->rmas_owner, rme, lln_rmas_owner);

  *rmep = rme;
}
