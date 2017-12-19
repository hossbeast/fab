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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "moria/edge.h"
#include "moria/graph.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "xapi/info.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"

#include "module.h"
#include "FF.errtab.h"
#include "artifact.h"
#include "extern.h"
#include "ff.h"
#include "ff_node.h"
#include "ff_node_artifact.h"
#include "ff_node_artifact_patterns.h"
#include "ff_node_use.h"
#include "ff_node_require.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "logging.h"
#include "node.h"
#include "node_operations.h"
#include "params.h"
#include "path.h"
#include "rule.h"
#include "walker.h"
#include "formula.h"

#include "zbuffer.h"
#include "assure.h"

#define LANGUAGE_FAB      "language.fab"
#define MODULE_FAB        "module.fab"
#define COMMON_FAB        "common.fab"

dictionary * g_modules;

//
// static
//

static xapi __attribute__((nonnull)) module_initialize(module * restrict mod, node * restrict base)
{
  enter;

  mod->base = base;
  fatal(list_createx, &mod->artifacts, wfree, 0, 0);
  fatal(list_create, &mod->rules);
  fatal(list_create, &mod->rules_lists);
  fatal(list_create, &mod->required);
  fatal(list_create, &mod->used);
  fatal(map_create, &mod->require_scope);

  fatal(list_push, mod->rules_lists, mod->rules);

  finally : coda;
}

static xapi module_xdestroy(module * restrict mod)
{
  enter;

  if(mod)
  {
    fatal(list_xfree, mod->artifacts);
    fatal(list_xfree, mod->rules);
    fatal(list_xfree, mod->rules_lists);
    fatal(list_xfree, mod->required);
    fatal(list_xfree, mod->used);
    fatal(map_xfree, mod->require_scope);
  }

  finally : coda;
}

static xapi module_vertex_load(vertex * mod_dir_v, int distance)
  __attribute__((nonnull));

static xapi ffn_apply(const ff_node * restrict ffn, module * restrict mod)
{
  enter;

  rendered_patterns * block = 0;
  typeof(*block->items) *item;
  int x;
  node * refnode = 0;

  while(ffn)
  {
    if(ffn->type == FFN_VARDEF)
    {

    }
    else if(ffn->type == FFN_RULE)
    {
      fatal(list_push, mod->rules, ffn);
    }
    else if(ffn->type == FFN_USE)
    {
      const ff_node_use * use = (ff_node_use*)ffn;

      // resolve the references
      iwfree(&block);
      fatal(ffn_patterns_render, use->target_list, &block);

      item = block->items;
      for(x = 0; x < block->num; x++)
      {
        fatal(node_lookup_path, ffn, item->name, &refnode);
        fatal(module_vertex_load, vertex_containerof(refnode), -1);

        if(refnode->mod == 0)
          fatal(ffn_semantic_error, FF_NOMODULE, ffn, 0, refnode, 0);

        fatal(list_push, mod->used, refnode->mod);
        fatal(list_push, mod->rules_lists, refnode->mod->rules);

        item = rendered_item_next(item);
      }
    }
    else if(ffn->type == FFN_REQUIRE)
    {
      const ff_node_require * require = (ff_node_require*)ffn;

      // resolve the references
      iwfree(&block);
      fatal(ffn_patterns_render, require->target_list, &block);

      item = block->items;
      for(x = 0; x < block->num; x++)
      {
        fatal(node_lookup_path, ffn, item->name, &refnode);
        fatal(module_vertex_load, vertex_containerof(refnode), -1);

        if(refnode->mod == 0)
          fatal(ffn_semantic_error, FF_NOMODULE, ffn, item->name, refnode, 0);

        fatal(list_push, mod->required, refnode->mod);

        // TODO add uniqueness check here
        fatal(map_set, mod->require_scope, refnode->mod->base->name->name, refnode->mod->base->name->namel, refnode->mod->base);

        item = rendered_item_next(item);
      }
    }
    else if(ffn->type == FFN_ARTIFACT)
    {
      const ff_node_artifact * artifact = (ff_node_artifact*)ffn;

      // resolve to list of nodes
      fatal(artifact_pattern_generate, artifact->pattern_list->chain, mod->base, mod->artifacts);
    }

    ffn = ffn->next;
  }

finally:
  wfree(block);
coda;
}

static xapi module_vertex_load(vertex * mod_dir_v, int distance)
{
  enter;

  vertex ** ancestry = 0;
  size_t ancestryz = 0;
  size_t ancestryl = 0;

  char path[512];
  size_t pathl;
  int x;

  node * mod_dir_n = vertex_value(mod_dir_v);
  node_get_absolute_path(mod_dir_n, path, sizeof(path));

  if(mod_dir_n->mod)
    goto XAPI_FINALIZE;

  vertex * mod_file_v = vertex_downs(mod_dir_v, MODULE_FAB);
  if(!mod_file_v)
    goto XAPI_FINALIZE;

  node * mod_file_n = vertex_value(mod_file_v);
  pathl = node_get_absolute_path(mod_dir_n, path, sizeof(path));
  module * mod;
  fatal(dictionary_set, g_modules, path, pathl, &mod);
  mod_dir_n->mod = mod;
  fatal(module_initialize, mod, mod_dir_n);

  // list of ancestor nodes, in reverse order
  vertex * v = mod_dir_v;
  while(v)
  {
    fatal(assure, &ancestry, sizeof(*ancestry), ancestryl + 1, &ancestryz);
    ancestry[ancestryl++] = v;
    v = vertex_up(v);
  }

  // apply ancestor common modules
  for(x = ancestryl - 1; x >= 0; x--)
  {
    vertex * common;
    if((common = vertex_downs(ancestry[x], COMMON_FAB)))
    {
      node * n = vertex_value(common);
      fatal(node_get_ffn, n, 0);
      if(n->ffn)
        fatal(ffn_apply, n->ffn, mod);
    }
  }

  // apply the module
  fatal(node_get_ffn, mod_file_n, 0);
  if(mod_file_n->ffn)
    fatal(ffn_apply, mod_file_n->ffn, mod);

  if(log_would(L_MODULE))
  {
    node_get_absolute_path(mod_dir_n, path, sizeof(path));
    logf(L_MODULE, "loaded module %s with %d artifacts", path, mod->artifacts->l);
    for(x = 0; x < mod->artifacts->l; x++)
    {
      artifact * af = list_get(mod->artifacts, x);
      logf(L_MODULE, " %s ? %.*s", af->node->name->path, (int)af->variant_len, af->variant);
    }
  }

finally:
  wfree(ancestry);
coda;
}

static xapi module_load_visitor(vertex * v, int distance, void * ctx)
{
  enter;

  fatal(module_vertex_load, v, distance);

  finally : coda;
}

//
// public
//

xapi module_load_project(node * restrict project_root, const char * restrict project_dir)
{
  enter;

  // load this module, its descendants, and any referenced modules, recursively
  fatal(graph_traverse_vertices
    , g_node_graph 
    , vertex_containerof(project_root)
    , module_load_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: RELATION_TYPE_FS
        , vertex_visit : NODE_FSTYPE_DIR
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );

  // build the dependency graph, radiating outward from known artifacts
  int traversal_id = graph_traversal_begin(g_node_graph);

  int x;
  for(x = 0; x < g_modules->table_size; x++)
  {
    module * mod = dictionary_table_value(g_modules, x);
    if(!mod)
      continue;

    int y;
    for(y = 0; y < mod->artifacts->l; y++)
    {
      artifact * af = list_get(mod->artifacts, y);

      fatal(rules_apply, af, traversal_id);
    }
  }

  finally : coda;
}

xapi module_setup()
{
  enter;

  fatal(dictionary_createx, &g_modules, sizeof(module), 0, 0, module_xdestroy);

  finally : coda;
}

xapi module_cleanup()
{
  enter;

  fatal(dictionary_xfree, g_modules);

  finally : coda;
}

xapi module_report()
{
  enter;

  logf(L_INFO, "loaded %d modules", g_modules->count);

  int x;
  for(x = 0; x < g_modules->table_size; x++)
  {
    module * mod = dictionary_table_value(g_modules, x);
    if(!mod)
      continue;

    narrator * N;
    fatal(log_start, L_INFO, &N);
    xsays("module ");
    fatal(node_relative_path_say, mod->base, N);
    fatal(log_finish);

    int y;
    for(y = 0; y < mod->artifacts->l; y++)
    {
      artifact * af = list_get(mod->artifacts, y);
      fatal(log_start, L_INFO, &N);
      xsays(" artifact ");
      fatal(node_relative_path_say, af->node, N);
      xsays(" ? ");
      xsayw(af->variant, af->variant_len);
      fatal(log_finish);
    }
  }

  finally : coda;
}

module * module_lookup(const char * const restrict path, size_t pathl)
{
  /* path is a normalized absolute path */
  const char * end = path + strlen(path);

  module * mod = 0;
  while(end != path && !mod)
  {
    mod = dictionary_get(g_modules, path, end - path);

    end--;
    while(end != path && *end != '/')
      end--;
  }

  if(!mod)
    mod = dictionary_get(g_modules, "/", 1);

  return mod;
}
