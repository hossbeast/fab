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
#include <fcntl.h>

#include "types.h"
#include "xapi.h"

#include "lorien/nftwat.h"
#include "lorien/path_normalize.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/multimap.h"
#include "valyria/pstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xstat.h"

#include "extern.h"
#include "FF.errtab.h"
#include "config.h"
#include "config_cursor.h"
#include "ff_node.h"
#include "ff_node_pattern.h"
#include "ff_node_string.h"
#include "fmt.h"
#include "logging.h"
#include "macros.h"
#include "node.h"
#include "params.h"
#include "reconfigure.h"
#include "snarf.h"

#include "macros.h"

static graph * extern_graph;
static multimap * extern_mmap;
static uint64_t extern_config_hash[2];

typedef struct {
  char * name;
} refnode;

typedef struct {
  vertex * root;
} extern_index_context;

static void refnode_destroy(refnode * restrict n)
{
  wfree(n->name);
}

/// index_directory_visit
//
// PARAMETERS
//  method - 
//  info   - 
//  arg    - 
//  stop   -  
//
static xapi __attribute__((nonnull)) index_directory_visit(
    int method
  , ftwinfo * restrict info
  , void * arg
  , int * restrict stop
)
{
  enter;

  vertex * v;
  refnode * n;
  extern_index_context * ctx = arg;

  if(method != FTWAT_PRE && method != 0)
    goto XAPI_FINALIZE;

  // skip dotfiles
  if(strncmp(info->path + info->name_off, ".", 1) == 0)
  {
    *stop = 1;
    goto XAPI_FINALIZE;
  }

  // skip the project dir
  if(strncmp(info->path, g_params.proj_dir, strlen(g_params.proj_dir)) == 0)
  {
    *stop = 1;
    goto XAPI_FINALIZE;
  }

  const char * name = info->path + info->name_off;
  size_t namel = info->pathl - info->name_off;

  fatal(graph_vertex_create, &v, extern_graph, 0);
  n = vertex_value(v);
  fatal(ixstrndup, &n->name, name, namel);
  v->label = n->name;
  v->label_len = namel;

  if(info->parent)
  {
    fatal(graph_connect_edge
      , extern_graph
      , info->parent->udata
      , v
      , NODE_RELATION_FS
    );
  }
  else
  {
    ctx->root = v;
  }

  fatal(extern_index_vertex, name, namel, v);
  info->udata = v;

  finally : coda;
}

/// extern_report_visit
//
// SUMMARY
//  v    - 
//  dist - 
//  arg  - 
//
static xapi extern_report_visit(vertex * v, int distance, void * arg)
{
  enter;

  narrator * N = g_narrator_stdout;
  xsayf("%*s%s\n", distance * 2, "", v->label);

  finally : coda;
}

/// index_directory
//
// SUMMARY
//  index a directory and its contents, recursively, for extern lookup
//
// PARAMETERS
//  abspath - absolute path to the directory
//
static xapi index_directory(const char * restrict abspath)
{
  enter;

  struct stat st;
  int r;

  fatal(uxstats, &r, &st, abspath);

  if(r)
  {
    logf(L_INFO, "no permissions %s", abspath);
  }
  else if(!S_ISDIR(st.st_mode))
  {
    logf(L_INFO, "not a dir %s", abspath);
  }
  else
  {
    extern_index_context ctx = {};
    fatal(nftwat, AT_FDCWD, abspath, index_directory_visit, 64, &ctx);
  }

  finally : coda;
}

//
// public
//

xapi extern_index_vertex(const char * restrict name, size_t namel, vertex * restrict v)
{
  enter;

  fatal(multimap_set, extern_mmap, name, namel, v);

  if(namel >= 4 && strncmp(name + namel - 4, ".fab", 4) == 0)
  {
    namel -= 4;
    fatal(multimap_set, extern_mmap, name, namel, v);
  }

  finally : coda;
}

xapi extern_reference_resolve(const ff_node_pattern * restrict ref, const vertex ** restrict rv)
{
  enter;

  char space[512];
  void * tmp = 0;
  const vertex ** nodes;
  size_t nodesl;
  const char * name;
  ffn_bydir_context base_ctx;

  // traverse the ref string left to right, by directory segments
  fatal(ffn_pattern_bydir_strings_rtl_init, ref->chain, &base_ctx, space, sizeof(space), &name);

  // get all references to the rightmost segment
  fatal(multimap_get, extern_mmap, MMS(name), &tmp, &nodes, &nodesl);

  int x[2] = { [0] = -1 };
  for(x[1] = 0; x[1] < nodesl; x[1]++)
  {
    const vertex * v = nodes[x[1]];

    ffn_bydir_context ctx = base_ctx;
    while(1)
    {
      fatal(ffn_pattern_bydir_strings_rtl, &ctx, space, sizeof(space), &name);
      if(!name)
        break;

      v = vertex_travel_vertexs(v, name, 0, NODE_RELATION_FS, MORIA_TRAVERSE_UP);
      if(v == 0)
        break;
    }

    if(name)
    {
      // ref not fully matched
      continue;
    }
    else if(x[0] != -1)
    {
      // ref matched multiple nodes
      fatal(ffn_semantic_error, FF_EXTERN_AMBIGREF, ref);
    }
    else
    {
      break;
    }

    x[0] = x[1];
  }

  if(x[1] < nodesl)
  {
    *rv = nodes[x[1]];
  }
  else
  {
    fatal(ffn_semantic_error, FF_EXTERN_NOREF, ref);
  }

finally:
  wfree(tmp);
coda;
}

xapi extern_reconfigure(struct reconfigure_context * restrict ctx, const value * restrict config, uint32_t dry)
{
  enter;

  char space[512];

  value * list;
  value * val;
  config_cursor cursor = {};

  if(!dry && !ctx->extern_changed)
    goto XAPI_FINALIZE;

  if(dry)
    extern_config_hash[1] = 0;

  else
  {
    fatal(multimap_recycle, extern_mmap);
    fatal(graph_recycle, extern_graph);
  }

  fatal(config_cursor_init, &cursor);
  fatal(config_cursor_sets, &cursor, "extern");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_LIST & dry, &list);
  if(list)
  {
    fatal(config_cursor_mark, &cursor);

    int x;
    for(x = 0; x < list->els->l; x++)
    {
      val = list_get(list->els, x);

      if(dry)
      {
        extern_config_hash[1] += value_hash(val);
      }
      else
      {
        path_normalize(space, sizeof(space), val->s->s);
        fatal(index_directory, space);
      }
    }
  }

  // no extern config
  else
  {
    extern_config_hash[1] = 0xdeadbeef;
  }

  if(dry)
    ctx->extern_changed = extern_config_hash[0] != extern_config_hash[1];
  else
  {
    fatal(extern_report);
    extern_config_hash[0] = extern_config_hash[1];
  }

finally:
  config_cursor_destroy(&cursor);
coda;
}

xapi extern_setup()
{
  enter;

  fatal(graph_createx, &extern_graph, sizeof(refnode), refnode_destroy, 0);
  fatal(multimap_create, &extern_mmap);

  finally : coda;
}

xapi extern_cleanup()
{
  enter;

  fatal(graph_xfree, extern_graph);
  fatal(multimap_xfree, extern_mmap);
  memset(extern_config_hash, 0, sizeof(extern_config_hash));

  finally : coda;
}

xapi extern_report()
{
  enter;

  logs(L_INFO, "extern graph");

  // dump the extern graph
  fatal(graph_traverse_all_vertices
    , extern_graph
    , extern_report_visit
    , 0
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );

  finally : coda;
}
