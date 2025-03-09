/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "valyria/llist.h"

#include "filesystem.h"
#include "rcu_list.h"
#include "vertex.h"
#include "types.h"
#include "descriptor.h"
#include "stats.h"
#include "lookup.h"
#include "exec_builder.h"
#include "exec_render.h"

struct exec;
struct bacon_writer;
struct narrator;
struct bacon_value;
struct pluginuration;
struct plugin_parser;
struct graph_invalidation_context;
struct plugin_thread;
struct plugin_process;
struct fsedge;
struct graph_disintegrate_context;

/* active plugin list */
extern llist plugin_list;
extern struct bacon_value *plugin_sysvars;

extern descriptor_type descriptor_stats_plugin;

typedef union stats_plugin {
  struct {
    /* event counters */
    uint16_t fork;           /* plugin process launched */
    uint16_t ready;          /* plugin process became ready */
    uint16_t reap;           /* plugin process reaped */
    uint16_t crashed;        /* plugin process exited with errors */

    uint32_t in_bytes;       /* bytes written to stdin */
    uint32_t out_bytes;      /* bytes read from stdout */
    uint32_t err_bytes;      /* bytes read from stderr */
    uint32_t aux_bytes;      /* bytes read from auxout */

    uint32_t tx_messages;    /* messages received from plugin process via auxout */
    uint16_t tx_parse_ok;    /* message from plugin parsed successfully */
    uint16_t tx_parse_err;   /* message from plugin parse attempted */
    uint32_t commands;       /* commands from the plugin performed */

    uint32_t rx_messages;    /* messages sent to plugin process via stdin */
    uint32_t events;         /* events delivered to plugin process */
  };

  stats_record record;
} stats_plugin;

#define PLUGIN_THREAD_STATE_TABLE                       \
  DEF(PLUGIN_THREAD_STARTING        , "starting"  , 0)  \
  DEF(PLUGIN_THREAD_UNREADY         , "unready"   , 1)  \
  DEF(PLUGIN_THREAD_READY_REQUESTED , "requested" , 2)  \
  DEF(PLUGIN_THREAD_READY_RECEIVED  , "received"  , 3)  \
  DEF(PLUGIN_THREAD_READY_REPORTED  , "reported"  , 4)  \
  DEF(PLUGIN_THREAD_EXITED          , "exited"    , 5)  \

typedef enum plugin_thread_state {
#undef DEF
#define DEF(x, s, v) x = v,
PLUGIN_THREAD_STATE_TABLE
} plugin_thread_state;

extern struct attrs32 plugin_thread_state_attrs;

/* VERTEX_PLUGIN */
typedef struct plugin {
  moria_vertex vertex;

  stats_vertex stats;
  stats_plugin plugin_stats;
  char id[40];
  uint16_t id_len;
  uint32_t epoch;

  /* optional name from config */
  char name[256];
  uint16_t name_len;

  /* shadow tree */
  struct fsent *shadow;                //plugins/id/

  /* regarding the corresponding VERTEX_PLUGIN_FILE */
  struct fsent *self_node;
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;

  plugin_thread_state thread_state;
  struct plugin_thread *thread;

  /* for building/storing the exec args */
  struct exec *exec;

  struct plugin_process *process;
  lookup_store lookups;
} plugin;

/* free a plugin struct with free semantics */
void plugin_release(struct plugin * restrict plugin)
  __attribute__((nonnull));

/* mark sections of a new plugin struct as different from those of another plugin struct */
bool plugin_compare(plugin * restrict new, plugin * restrict old)
  __attribute__((nonnull));

void plugin_say(plugin * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

void plugin_setup(void);
void plugin_cleanup(void);

plugin *plugin_get(
    struct fsent * restrict fse
  , const char * restrict name
  , uint16_t name_len
  , struct graph_invalidation_context * restrict invalidation
)
  __attribute__((nonnull(1, 4)));

size_t plugin_path_cat(void * restrict dst, size_t sz, const plugin * restrict config)
  __attribute__((nonnull));

void plugin_describe(const plugin * restrict plugin, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void plugin_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void plugin_identity_describe(const struct plugin * restrict plugin, struct bacon_writer * restrict)
  __attribute__((nonnull));

void plugin_file_identity_describe(const struct fsent * restrict fse, struct bacon_writer * restrict)
  __attribute__((nonnull));

/*
 * Write a plugin stats to a buffer
 *
 * reset - true to reset the stats while reading them
 */
void plugin_stats_build(plugin * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void plugin_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void plugin_unlink(struct plugin * restrict plugin, struct graph_disintegrate_context * restrict ctx)
  __attribute__((nonnull));

void plugin_hash(const plugin * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void plugin_id(const plugin * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void plugin_compute_id(const struct plugin * restrict v, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void plugins_show(void);
