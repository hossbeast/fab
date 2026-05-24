/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/map.h"
#include "bacon/builder.h"
#include "narrator/units.h"
#include "narrator/fixed.h"
#include "bam/ipc.h"

#include "notify.h"
#include "fsent.h"
#include "request_handler.h"
#include "shadow.h"
#include "identity.h"
#include "subsys.h"
#include "sweeper_thread.h"


llist notify_watched_files;
union stats_notify notify_stats;
subsys sys_notify;

void notify_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  notify_stats.count = notify_thread_fsent_by_in_wd->size;
  notify_stats.file_count = llist_count(&notify_watched_files);

  /* not currently computable */
  notify_stats.dir_count = 0;
  notify_stats.symlink_count = 0;

  stats_build(&notify_stats.record, &descriptor_stats_notify, builder_writer);
}

void notify_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  char buf[256];
  narrator_fixed fixed;

  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING STRING("sweeper-interval")
        interval_say_ns(sweeper_thread_interval, narrator_fixed_init(&fixed, buf, sizeof(buf)));
        BYTES(fixed.s, fixed.l)
      }
      { MAPPING STRING("sweeper-clock")
        UINT(sweeper_thread_clock);
      }
      { MAPPING
        STRING("stats")
        { SET
          notify_sys_stats_build(v, false, builder_writer);
        }
      }

      vertex_relations_describe(v, builder_writer);
    }
  }
}

void notify_sys_show(void)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  notify_sys_describe(&sys_notify.vertex, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void notify_setup(void)
{
  llist_init(&notify_watched_files);

  subsys_init(&sys_notify, VERTEX_SYS_NOTIFY);
  shadow_mkref(g_shadow_bm, sys_notify.vertex.label, sys_notify.vertex.label_len, &sys_notify.vertex);
}

void notify_cleanup(void)
{

}

void notify_add_watch(fsent * fse)
{
  if(fse->in_wd != -1) {
    return;
  }

  notify_thread_add_watch(fse);

  if(fse->in_wd == -1) {
    return;
  }
  if(vertex_type_get(&fse->vertex) != VERTEX_FILE) {
    return;
  }

  llist_push(&notify_watched_files, &fse->notify_watched_files_lln);
}

void notify_rm_watch(fsent * fse)
{
  if(fse->in_wd == -1) {
    return;
  }

  notify_thread_rm_watch(fse);

  if(vertex_type_get(&fse->vertex) != VERTEX_FILE) {
    return;
  }

  llist_delete(&fse->notify_watched_files_lln);
}

void notify_sys_tell(const moria_vertex * restrict v, struct request_handler * restrict handler)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  narrator *N;
  bacon_writer writer, *builder_writer;
  const fsent *fse;

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { MAPPING STRING("dirs")
    UINT(notify_thread_fsent_by_in_wd->size)
  }

  { MAPPING STRING("files")
    UINT(llist_count(&notify_watched_files))
  }

  llist_foreach(&notify_watched_files, fse, notify_watched_files_lln) {
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&fse->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  msg->len = fixed.l;
  request_handler_post(handler, msg);
}
