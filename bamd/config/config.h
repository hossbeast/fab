/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "types.h"
#include "fstree.h"

struct bacon_value;
struct bacon_writer;
struct narrator;
struct fsent;
struct config;
struct graph_invalidation_context;
struct configset_parser;
struct moria_vertex;
struct channel;
struct pattern;

/* fstree properties */
typedef struct config_fstree_value {
  struct {
    struct bacon_value *v;
    invalidation_type e;
  } invalidation;
  struct {
    struct bacon_value *v;
    bool b;
  } mkdir;
  struct {
    struct bacon_value *v;
    bool b;
  } readlink;
  struct {
    struct bacon_value *v;
    bool b;
  } index;
  struct {
    struct bacon_value *v;
    bool b;
  } mount;
} config_fstree_value;

/* fstree entry */
typedef struct config_fstree_mapping {
  llist lln;
  rbnode rbn;

  struct {
    struct pattern *v;
  } key;

  config_fstree_value val;
} config_fstree_mapping;

typedef struct config_special_entry {
  llist lln;
  rbnode rbn;
  struct bacon_value *v;
  char *s;
  uint16_t len;
} config_special_entry;

typedef struct config_formula_path_dirs_entry {
  llist lln;
  rbnode rbn;
  struct bacon_value *v;
  char *s;
  uint16_t len;
} config_formula_path_dirs_entry;

typedef struct config_plugin_path_dirs_entry {
  llist lln;
  rbnode rbn;
  struct bacon_value *v;
  char *s;
  uint16_t len;
} config_plugin_path_dirs_entry;

typedef struct config_plugin_entry {
  llist lln;

  struct {
    struct bacon_value *v;
    char *s;
    uint16_t len;
  } path;

  struct {
    struct bacon_value *list;
  } args;
} config_plugin_entry;

typedef struct config {
  llist owner;

  struct {
    struct {
      struct bacon_value *v;
      uint16_t u16;
    } concurrency;
  } build;

  struct config_filesystem {
    struct {
      rbtree set;     // config_fstree_mapping
    } fstrees;

    struct {
      rbtree set;     // struct pattern
    } exclude;

    struct {
      rbtree set;     // config_special_entry
    } special;
  } filesystem;

  struct {
    struct {
      struct {
        struct bacon_value *v;
        bool b;
      } copy_from_env;
      struct {
        rbtree set;           // config_formula_path_dirs_entry
      } dirs;
    } path;
  } formula;

  struct {
    struct {
      struct bacon_value *v;
      int64_t i64;            // seconds
    } interval;
    struct {
      struct bacon_value *v;
      bool b;
    } refresh_on_request;
    struct {
      struct bacon_value *v;
      bool b;
    } refresh_on_autobuild;
  } timeout;

  struct {
    struct {
      struct {
        rbtree set;           // config_plugin_path_dirs_entry
      } dirs;
    } path;

    llist list;               // config_plugin_entry
  } plugins;
} config;

extern struct config *config_active;
extern struct moria_vertex *g_config;

void config_alloc(config ** restrict rv)
  __attribute__((nonnull));

void config_free(config * restrict cfg, struct configset_parser * restrict parser);

void config_ifree(config ** restrict cfg, struct configset_parser * restrict parser)
  __attribute__((nonnull));

void config_say(const config * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

void config_describe(const config * restrict cfg, struct bacon_writer * const restrict builder_writer)
  __attribute__((nonnull));

void config_show(const config * restrict cfg)
  __attribute__((nonnull));

void config_compare(const config * restrict prev, config * restrict next)
  __attribute__((nonnull(2)));

void config_setup(void);
void config_cleanup(void);

void config_vertex_describe(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void config_vertex_tell(struct channel * restrict chan)
  __attribute__((nonnull));

void config_vertex_stats_build(bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));
