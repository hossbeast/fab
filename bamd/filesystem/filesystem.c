/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "valyria/llist.h"
#include "valyria/map.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "fstree.h"
#include "fsent.h"
#include "stats.h"
#include "events/events.h"
#include "vertex.h"
#include "filesystem.h"
#include "system.h"
#include "shadow.h"
#include "subsys.h"
#include "ref_edge.h"
#include "symlink.h"

#include "zcat.h"

union stats_filesystem filesystem_stats;
subsys sys_filesystem;

void filesystem_setup(void)
{
  subsys_init(&sys_filesystem, VERTEX_SYS_FILESYSTEM);

  shadow_mkref(g_shadow_bm, sys_filesystem.vertex.label, sys_filesystem.vertex.label_len, &sys_filesystem.vertex);
}

void filesystem_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&filesystem_stats.record, &descriptor_stats_filesystem, builder_writer);
}

void filesystem_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          filesystem_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}
