/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/map.h"
#include "bacon/builder.h"
#include "moria.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "common/hash.h"

#include "notify.h"
#include "vertex.h"
#include "notify_thread.h"
#include "fsent.h"

#include "zcat.h"

llist notify_watched_files;
union stats_notify notify_stats;
moria_vertex *g_notify = (moria_vertex[]){{ }};
char g_notify_id[40];
uint16_t g_notify_id_len;

void notify_stats_build(bool reset, bacon_writer * restrict builder_writer)
{
  notify_stats.count = notify_thread_fsent_by_in_wd->size;

  stats_build(&notify_stats.record, &descriptor_stats_notify, builder_writer);
}

void notify_describe(struct bacon_writer * restrict builder_writer)
{
  fsent *fse;
  int x;
  int *key;
  uint16_t key_len;
  size_t val_len;

  { MAPPING
    vertex_path_describe(g_notify, builder_writer);
    { SET
      vertex_mappings(g_notify, builder_writer);
      { MAPPING
        STRING("watches")
        { LIST
          for(x = 0; x < notify_thread_fsent_by_in_wd->table_size; x++) {
            if(!map_table_mapping(notify_thread_fsent_by_in_wd, x, (void*)&key, &key_len, &fse, &val_len)) {
              continue;
            }
            { MAPPING
              INT(*key);
              vertex_path_describe(&fse->vertex, builder_writer);
            }
          }
        }
      }
      { MAPPING
        STRING("stats")
        { SET
          notify_stats_build(false, builder_writer);
        }
      }
    }
  }
}

void notify_show(void)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  notify_describe(builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

void notify_setup(void)
{
  llist_init(&notify_watched_files);

  moria_vertex_initw(g_notify, &g_graph, VERTEX_NOTIFY | VERTEX_OK, MMS("(notify)"));
  g_notify_id_len = zcatf(g_notify_id, sizeof(g_notify_id), "%016"PRIx64, hash64(0x3b37, MMS("(notify)")));
}

void notify_cleanup(void)
{

}

size_t notify_id_cat(char *dst, size_t sz)
{
  return zcatw(dst, sz, g_notify_id, g_notify_id_len);
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
