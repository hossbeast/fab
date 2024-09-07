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

struct bacon_writer;
struct narrator;
struct bacon_value;
struct pluginuration;
struct plugin_parser;
struct graph_invalidation_context;
struct plugin_thread;
struct plugin_process;

/* active plugin list */
extern llist plugin_list;

extern descriptor_type descriptor_stats_plugin;

typedef union stats_plugin {
  struct {
    /* event counters */
    uint16_t parse;          /* parse attempted */
    uint16_t parse_ok;       /* parsed successfully */
    uint16_t fork;           /* plugin process launched */
    uint16_t ready;          /* plugin process became ready */
    uint16_t reap;           /* plugin process reaped */
    uint16_t crashed;        /* plugin process exited with errors */

    uint32_t in_bytes;       /* bytes written to stdin */
    uint32_t out_bytes;      /* bytes read from stdout */
    uint32_t err_bytes;      /* bytes read from stderr */
    uint32_t aux_bytes;      /* bytes read from auxout */

    uint32_t rx_messages;    /* messages received */
    uint32_t tx_messages;    /* messages send */
  };

  stats_record record;
} stats_plugin;

/* VERTEX_PLUGIN */
typedef struct plugin {
  moria_vertex vertex;

  stats_vertex stats;
  stats_plugin plugin_stats;
  char id[40];
  uint16_t id_len;

  /* shadow link */
  struct fsedge *link;

  /* regarding the corresponding VERTEX_PLUGIN_FILE */
  struct fsent *self_node;
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;

  struct plugin_thread *thread;
  struct plugin_process *process;
  struct llist *args;     /* struct bacon_value */
} plugin;

/* free a plugin struct with free semantics */
void plugin_release(plugin * restrict cfg, struct graph_invalidation_context * restrict invalidation);

/* mark sections of a new plugin struct as different from those of another plugin struct */
bool plugin_compare(plugin * restrict new, plugin * restrict old)
  __attribute__((nonnull));

void plugin_say(plugin * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

void plugin_cleanup(void);

void plugin_alloc(plugin ** restrict cfg)
  __attribute__((nonnull));

size_t plugin_path_cat(void * restrict dst, size_t sz, const plugin * restrict config)
  __attribute__((nonnull));

void plugin_describe(const plugin * restrict plugin, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void plugin_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void plugin_identity_say(const struct plugin * restrict plugin, struct narrator * restrict N)
  __attribute__((nonnull));

void plugin_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
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
