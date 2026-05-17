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
#include "fsent_dir.h"

#include "zcat.h"

llist filesystem_reconcile_list = LLIST_INITIALIZER(filesystem_reconcile_list);

static void mkdir_no_error(fstree * restrict fst, const char * restrict path)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("mkdir disallowed")
    { SET
      { MAPPING STRING("path")
        STRING(path)
      }
      { MAPPING STRING("fstree")
        { SET
          vertex_identity_describe(&fst->vertex, builder_writer);
        }
      }
    }
  }

  bacon_writer_close(&writer);
  msg->code = EPERM;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void mkdir_error(const char * restrict path)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("mkdir failed")
    { SET
      { MAPPING STRING("path")
        STRING(path)
      }
      { MAPPING STRING("error")
        STRING(strerror(errno))
      }
    }
  }

  bacon_writer_close(&writer);
  msg->code = EPERM;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void vertex_dir_mkdir(fsent *fse)
{
  fstree *fst;
  char path[PATH_MAX];

  RUNTIME_ASSERT(vertex_state_get(&fse->vertex) == VERTEX_UNCREATED);

  fsent_absolute_path_cat(path, sizeof(path), fse);
  fst = vertex_fstree_of(&fse->vertex);
  if(!fst->mkdir) {
    filesystem_stats.mkdir_no++;
    fst->fstree_stats.mkdir_no++;
    fse->dirstats.mkdir_no++;
    mkdir_no_error(fst, path);
    return;
  }

  if(mkdir(path, 0755) == 0) {
    filesystem_stats.mkdir_ok++;
    fst->fstree_stats.mkdir_ok++;
    fse->dirstats.mkdir_ok++;
  } else if(errno == EEXIST) {
    filesystem_stats.mkdir_eexist++;
    fst->fstree_stats.mkdir_eexist++;
    fse->dirstats.mkdir_eexist++;
  } else {
    filesystem_stats.mkdir_err++;
    fst->fstree_stats.mkdir_err++;
    fse->dirstats.mkdir_err++;
    mkdir_error(path);
    return;
  }

  vertex_state_set(&fse->vertex, VERTEX_OK);
}

void fsent_dir_reconcile()
{
  fsent *fse;

  /* this algorithm for mkdirs relies on the fact that, because pattern-generate is left-to-right, directories
   * that need to be created will have been added to this list in the proper order */

  while((fse = llist_shift(&filesystem_reconcile_list, typeof(*fse), fs_mkdir_lln))) {
    switch(vertex_type_get(&fse->vertex)) {
      case VERTEX_DIR:
        vertex_dir_mkdir(fse);
        break;
      default:
        RTA;
    }

    if(system_has_error) {
      llist_unshift(&filesystem_reconcile_list, &fse->fs_mkdir_lln);
      return;
    }
  }

}

void fsent_dir_mkdir_push(struct fsent * restrict fse)
{
  switch(vertex_type_get(&fse->vertex)) {
    case VERTEX_DIR:
      break;
    default:
      RTA;
  }

  if(llist_attached(&fse->fs_mkdir_lln)) {
    return;
  }

  llist_delete(&fse->fs_mkdir_lln);
  llist_push(&filesystem_reconcile_list, &fse->fs_mkdir_lln);
}

void fsent_dir_mkdir_unshift(struct fsent * restrict fse)
{
  switch(vertex_type_get(&fse->vertex)) {
    case VERTEX_DIR:
      break;
    default:
      RTA;
  }

  llist_delete(&fse->fs_mkdir_lln);
  llist_unshift(&filesystem_reconcile_list, &fse->fs_mkdir_lln);
}

void fsent_dir_mkdir_cancel(struct fsent * restrict fse)
{
  if(!llist_attached(&fse->fs_mkdir_lln)) {
    return;
  }

  llist_delete(&fse->fs_mkdir_lln);
}
