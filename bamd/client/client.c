/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "xlinux/xtime.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "bacon/writer.h"
#include "common/attrs.h"
#include "moria/connect.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/fixed.h"

#include "client.h"
#include "fsedge.h"
#include "shadow.h"
#include "vertex/probes.h"
#include "edges.h"
#include "vertices.h"
#include "types.h"
#include "zcat.h"
#include "channel.h"
#include "params.h"
#include "times.h"
#include "identity.h"
#include "locks.h"
#include "disintegrate.h"

llist client_list = LLIST_INITIALIZER(client_list);
static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;
static uint32_t client_counter;

static void runtime_say(const struct client * restrict client, struct narrator * restrict N)
{
  struct timespec now;
  uint64_t ns;

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &now);
  ns = timespec_delta(&client->create_time, &now);
  interval_say(ns / NSEC_PER_SEC, N);
}

void client_setup()
{
}

void client_cleanup()
{
  struct client *client;

  while((client = llist_shift(&freelist, typeof(*client), vertex.owner))) {
    ASAN_CLEAN(client);
    free(client);
  }
}

void client_stats_build(struct client * restrict client, bool reset, struct bacon_writer * restrict builder_writer)
{
  stats_build(&client->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&client->client_stats.record, &descriptor_stats_client, builder_writer);
}

void client_describe(const struct client *client, bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&client->vertex, builder_writer);
    { SET
      vertex_mappings(&client->vertex, builder_writer);
      { MAPPING STRING("runtime")
        bacon_writer_push_string(builder_writer);
        runtime_say(client, builder_writer->N);
        bacon_writer_pop_string(builder_writer);
      }
      if(client->channel) {
        MAPPING STRING("channel")
        { SET
          { MAPPING STRING("shmid") UINT(client->channel->ipc->shmid) }
          { MAPPING STRING("client")
            { SET
              { MAPPING STRING("pid") UINT(client->channel->ipc->client_pid) }
              { MAPPING STRING("tid") UINT(client->channel->ipc->client_tid) }
            }
          }
          { MAPPING STRING("rx")
            { SET
              { MAPPING STRING("tx-fast") UINT(client->channel->ipc->rx.stats.tx_fast) }
              { MAPPING STRING("tx-waits") UINT(client->channel->ipc->rx.stats.tx_waits) }
              { MAPPING STRING("tx-stalls") UINT(client->channel->ipc->rx.stats.tx_stalls) }
              { MAPPING STRING("publish-fast") UINT(client->channel->ipc->rx.stats.publish_fast) }
              { MAPPING STRING("publish-stalls") UINT(client->channel->ipc->rx.stats.publish_stalls) }
            }
          }
          { MAPPING STRING("tx")
            { SET
              { MAPPING STRING("tx-fast") UINT(client->channel->ipc->tx.stats.tx_fast) }
              { MAPPING STRING("tx-waits") UINT(client->channel->ipc->tx.stats.tx_waits) }
              { MAPPING STRING("tx-stalls") UINT(client->channel->ipc->tx.stats.tx_stalls) }
              { MAPPING STRING("publish-fast") UINT(client->channel->ipc->tx.stats.publish_fast) }
              { MAPPING STRING("publish-stalls") UINT(client->channel->ipc->tx.stats.publish_stalls) }
            }
          }
        }
      }
      { MAPPING STRING("stats")
        { SET
          client_stats_build((struct client *)client, false, builder_writer);
        }
      }
      vertex_relations_describe(&client->vertex, builder_writer);
    }
  }
}

void client_show(const struct client *client)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  client_describe(client, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}

size_t client_path_cat(void * restrict dst, size_t sz, const struct client * restrict client)
{
  return zcatf(dst, sz, "//bm/clients/%.*s", client->id_len, client->id);
}

void client_identity_describe(const struct client * restrict client, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(client->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(client->id, client->id_len);
  }
  { MAPPING
    STRING("runtime")
    bacon_writer_push_string(builder_writer);
    runtime_say(client, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
  }
}

client *client_alloc(void)
{
  struct client *client = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    client = llist_shift(&freelist, typeof(*client), vertex.owner);
    futexlock_release(&freelist_lock);
  }
  if(client) {
    ASAN_CLEAN(client);
  } else {
    xmalloc(&client, sizeof(*client));
  }

  memset(client, 0, sizeof(*client));

  moria_vertex_init(&client->vertex, &g_graph, VERTEX_CLIENT | VERTEX_OK);

  llist_init(&client->vertex.owner);
  llist_append(&client_list, &client->vertex.owner);

  vertex_alloc_probe(&client->vertex);

  return client;
}

void client_advertise(struct client * restrict client)
{
  identity_ctx id;
  char str[64];

  zcatf(str, sizeof(str), "%u", client_counter++);

  identity_init(&id);
  identity_type(&id, VERTEX_CLIENT);
  identity_add(&id, MM(" "));
  identity_add(&id, MM(str));
  client->id_len = identity_mk(&id, client->id, sizeof(client->id));

  client->vertex.label = client->id;
  client->vertex.label_len = client->id_len;

  xclock_gettime(CLOCK_MONOTONIC_COARSE, &client->create_time);

  shadow_mkref(g_shadow_clients, client->id, client->id_len, &client->vertex);
}

void client_excise(struct client * restrict client)
{
  graph_demolish(&client->vertex);
}

void client_release(struct client * restrict client)
{
  moria_vertex *v;

  /* must have been fully excised from the graph previously */
  v = &client->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  llist_delete(&client->vertex.owner);
  llist_append(&freelist, &client->vertex.owner);
  ASAN_TAINT(client);
}

void client_id(const struct client * restrict client, const char ** restrict id, uint16_t * restrict idl)
{
  *id = client->id;
  *idl = client->id_len;
}
