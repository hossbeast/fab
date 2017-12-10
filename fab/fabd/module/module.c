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
#include "valyria/array.h"
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
#include "ff_node_use.h"
#include "ff_node_require.h"
#include "logging.h"
#include "node.h"
#include "node_operations.h"
#include "params.h"
#include "rule.h"
#include "walker.h"

#include "zbuffer.h"

#define LANGUAGE_FAB      "language.fab"
#define MODULE_FAB        "module.fab"
#define COMMON_FAB        "common.fab"

array * g_modules;

//
// static
//

static xapi __attribute__((nonnull)) module_initialize(module * restrict mod, node * restrict base)
{
  enter;

  mod->base = base;
  fatal(list_createx, &mod->artifacts, wfree, 0, 0);
  fatal(list_create, &mod->rules);
  fatal(list_create, &mod->required);

  finally : coda;
}

static xapi module_xdestroy(module * restrict mod)
{
  enter;

  if(mod)
  {
    fatal(list_xfree, mod->artifacts);
    fatal(list_xfree, mod->rules);
    fatal(list_xfree, mod->required);
  }

  finally : coda;
}

static xapi __attribute__((nonnull)) node_index_visitor(vertex * restrict v, int distance, void * restrict ctx)
{
  xproxy(extern_index_vertex, v->label, v->label_len, v);
}

static xapi __attribute__((nonnull(1))) fabfile_module_build(const ff_node * restrict ffn, module * restrict mod)
{
  enter;

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

      // resolve the reference
      const vertex * node;
      fatal(extern_reference_resolve, use->pattern, &node);

      // apply those rules and formulas to this module
    }
    else if(ffn->type == FFN_REQUIRE)
    {
      const ff_node_require * require = (ff_node_require*)ffn;

      // resolve the reference
      const vertex * node;
      fatal(extern_reference_resolve, require->pattern, &node);

      // save with this module
    }
    else if(ffn->type == FFN_ARTIFACT)
    {
      const ff_node_artifact * artifact = (ff_node_artifact*)ffn;

      // resolve to list of nodes
      fatal(artifact_pattern_generate, artifact->pattern, mod->base, mod->artifacts);
    }

    ffn = ffn->next;
  }

  finally : coda;
}

static xapi module_load_visitor(vertex * v, int distance, void * ctx)
{
  enter;

  char path[512];
  ff_node * ffn = 0;
  module * mod = 0;
  vertex * common = 0;
  vertex * module = 0;

  common = vertex_travel_vertexs(v, COMMON_FAB, NODE_FSTYPE_FILE, NODE_RELATION_FS, MORIA_TRAVERSE_DOWN);
  module = vertex_travel_vertexs(v, MODULE_FAB, NODE_FSTYPE_FILE, NODE_RELATION_FS, MORIA_TRAVERSE_DOWN);
  if(common || module)
  {
    node_get_absolute_path(vertex_value(v), path, sizeof(path));
  }

  if(common)
  {
    fatal(ff_load_pathf, &ffn, "%s/%s", path, COMMON_FAB);

    if(ffn)
    {
      fatal(fabfile_module_build, ffn, 0);
      logf(L_MODULE, "loaded %s/%s", path, COMMON_FAB);
      ffn = 0;
    }
  }

  if(module)
  {
    fatal(ff_load_pathf, &ffn, "%s/%s", path, MODULE_FAB);
  }

  if((module || distance == 0) && !ffn)
  {
    xapi_info_pushs("directory", path);
    fail(FF_NOMODULE);
  }

  if(module)
  {
    fatal(array_push, g_modules, &mod);
    fatal(module_initialize, mod, vertex_value(v));
    fatal(fabfile_module_build, ffn, mod);
    logf(L_MODULE, "loaded %s", path);
  }

  finally : coda;
}

//
// public
//

xapi module_load_project(node * restrict project_root, const char * restrict project_dir)
{
  enter;

  ff_node * ffn;

  // index the filesystem rooted at the module dir for extern lookups
  fatal(graph_traverse_vertices
    , g_node_graph 
    , vertex_containerof(project_root)
    , node_index_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel : NODE_FSTYPE_DIR
        , edge_visit : NODE_RELATION_FS
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );

  // traverse ancestor directories for common fabfiles, in reverse order
  const char * end = project_dir;
  const char * term = project_dir + strlen(project_dir);
  end++;
  while(end != term && *end != '/')
    end++;

  while(end != term)
  {
    fatal(ff_load_pathf, &ffn, "%.*s/%s", (int)(end - project_dir), project_dir, COMMON_FAB);

    if(ffn)
    {
      fatal(fabfile_module_build, ffn, 0);
      logf(L_MODULE, "loaded %.*s/%s", (int)(end - project_dir), project_dir, COMMON_FAB);
    }

    end++;
    while(end != term && *end != '/')
      end++;
  }

  // load this module and its descendants
  fatal(graph_traverse_vertices
    , g_node_graph 
    , vertex_containerof(project_root)
    , module_load_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel : NODE_FSTYPE_DIR
        , edge_visit : NODE_RELATION_FS
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );

  // build the dependency graph, radiating outward from all known artifacts
  int traversal_id = node_traversal_begin();

  int x;
  for(x = 0; x < g_modules->l; x++)
  {
    module * mod = array_get(g_modules, x);

    int y;
    for(y = 0; y < mod->artifacts->l; y++)
    {
      artifact * af = list_get(mod->artifacts, y);
      fatal(rules_apply, mod->rules, mod->base, af, traversal_id);
    }
  }

  fatal(module_report);

  finally : coda;
}

xapi module_setup()
{
  enter;

  fatal(array_createx, &g_modules, sizeof(module), 0, module_xdestroy, 0);

  finally : coda;
}

xapi module_cleanup()
{
  enter;

  fatal(array_xfree, g_modules);

  finally : coda;
}

xapi module_report()
{
  enter;

  logs(L_INFO, "loaded modules");

  int x;
  for(x = 0; x < g_modules->l; x++)
  {
    module * mod = array_get(g_modules, x);

    narrator * nar;
    fatal(log_start, L_INFO, &nar);
    fatal(node_path_say, mod->base, nar);
    fatal(log_finish);
  }

  finally : coda;
}
