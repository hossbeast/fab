/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "valyria/llist.h"
#include "valyria/map.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "fstree.h"
#include "fsent.h"
#include "common/hash.h"
#include "stats.h"
#include "events.h"
#include "vertex.h"
#include "filesystem.h"
#include "system.h"

#include "zcat.h"

union stats_filesystem filesystem_stats;
moria_vertex *g_filesystem = (moria_vertex[]){{ }};
char g_filesystem_id[40];
uint16_t g_filesystem_id_len;

llist filesystem_reconcile_list = LLIST_INITIALIZER(filesystem_reconcile_list);

//
// public
//

static void vertex_dir_mkdir(fsent *fse)
{
  fstree *fst;
  char path[PATH_MAX];
  events_context ectx;
  event_bacon *msg;
  const char *errstr;

  if(vertex_state_get(&fse->vertex) == VERTEX_OK) {
    return;
  }

  fst = vertex_fstree_get(&fse->vertex);
  if(!fst->mkdir) {
    vertex_state_set(&fse->vertex, VERTEX_OK);
    return;
  }

  filesystem_stats.mkdir++;
  fst->fstree_stats.mkdir++;
  fse->dirstats.mkdir++;

  fsent_absolute_path_cat(path, sizeof(path), fse);
  if(mkdir(path, 0755) != 0 && errno != EEXIST) {
    errstr = strerror(errno);
    goto err;
  }
  vertex_state_set(&fse->vertex, VERTEX_OK);
  return;

err:
  msg = system_error_event(&ectx);
  msg->code = EINVAL;
  msg->size += zcats(msg->text + msg->size, sizeof(msg->text) - msg->size, errstr);
  msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, " with path %s", path);
  events_publish(&ectx);
}

void filesystem_reconcile()
{
  fsent *fse;

  /* this algorithm for mkdirs relies on the fact that, because pattern-generate is left-to-right, directories
   * that need to be created will have been added to this list in the proper order */

  while((fse = llist_shift(&filesystem_reconcile_list, typeof(*fse), fs_reconcile_lln))) {
    switch(vertex_type_get(&fse->vertex)) {
      case VERTEX_MODULE_DIR:
      case VERTEX_DIR:
        vertex_dir_mkdir(fse);
        break;
/*
      case VERTEX_SYMLINK:
        fsent_symlink_resolve(fse);
        break;
*/
      default:
        vertex_shape_show(&fse->vertex);
        RTA;
    }

    if(system_error) {
      llist_unshift(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
      break;
    }
  }
}

void filesystem_mkdir_push(struct fsent * restrict fse)
{
  llist_delete(&fse->fs_reconcile_lln);
  llist_push(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
}

void filesystem_mkdir_unshift(struct fsent * restrict fse)
{
  llist_delete(&fse->fs_reconcile_lln);
  llist_unshift(&filesystem_reconcile_list, &fse->fs_reconcile_lln);
}

void filesystem_setup(void)
{
  moria_vertex_initw(g_filesystem, &g_graph, VERTEX_FILESYSTEM | VERTEX_OK, MMS("(filesystem)"));
  g_filesystem_id_len = zcatf(g_filesystem_id, sizeof(g_filesystem_id), "%016"PRIx64, hash64(0x1234, MMS("(filesystem)")));
}

void filesystem_stats_build(bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&filesystem_stats.record, &descriptor_stats_filesystem, builder_writer);
}

void filesystem_describe(struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(g_filesystem, builder_writer);
    { SET
      vertex_mappings(g_filesystem, builder_writer);
      { MAPPING
        STRING("stats")
        { SET
          filesystem_stats_build(false, builder_writer);
        }
      }
    }
  }
}

size_t filesystem_id_cat(char * dst, size_t sz)
{
  return zcatw(dst, sz, g_filesystem_id, g_filesystem_id_len);
}
