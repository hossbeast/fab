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
struct exec_value;
struct request_handler;
struct include_pattern;

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
  } unlink;
  struct {
    struct bacon_value *v;
    readlink_type e;
  } readlink;
  struct {
    struct bacon_value *v;
    bool b;
  } mount;
  struct {
    struct bacon_value *list;   // list of namespaces
  } namespaces;
} config_fstree_value;

/* fstree entry */
typedef struct config_fstree_mapping {
  llist lln;
  rbnode rbn;

  struct {
    struct include_pattern *v;
  } key;

  config_fstree_value val;
} config_fstree_mapping;

typedef struct config_namespace_entry {
  llist lln;
  rbnode rbn;

  struct bacon_value *name;
} config_namespace_entry;

typedef struct config_plugin_entry {
  llist lln;

  /* optional */
  struct bacon_value *name;

  struct {
    struct bacon_value *lookup;
  } file;

  struct bacon_value *args;
  struct bacon_value *envs;
} config_plugin_entry;

typedef struct config_specials_entry {
  llist lln;
  rbnode rbn;

  struct bacon_value *name;
} config_specials_entry;

typedef struct config {
  llist owner;

  struct {
    struct {
      struct bacon_value *v;
      uint16_t u16;
    } concurrency;
    struct {
      struct bacon_value *v;
      uint32_t u32;
    } stdout_buffer_size;
    struct {
      struct bacon_value *v;
      uint32_t u32;
    } stderr_buffer_size;
    struct {
      struct bacon_value *v;
      uint32_t u32;
    } auxout_buffer_size;
    struct {
      struct {
        struct bacon_value *v;
        uint32_t u32;
      } max_entries;
    } retention;
  } build;

  struct {
    struct {
      rbtree set;     // config_fstree_mapping
    } fstrees;

    struct {
      rbtree set;     // struct pattern
    } exclude;

    struct {
      rbtree set;     // config_specials_entry
    } special;
  } filesystem;

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
      struct bacon_value *v;
      uint32_t u32;           // milliseconds
    } interval;
  } notify;

  struct {
    rbtree set;               // config_namespace_entry
  } namespaces;

  struct {
    llist list;               // config_plugin_entry

    struct {
      struct bacon_value *v;
      uint32_t u32;
    } stdin_buffer_size;

    struct {
      struct bacon_value *v;
      uint32_t u32;
    } stdout_buffer_size;

    struct {
      struct bacon_value *v;
      uint32_t u32;
    } stderr_buffer_size;

    struct {
      struct bacon_value *v;
      uint32_t u32;
    } auxout_buffer_size;
  } plugins;

  struct {
    struct {
      rbtree set;     // config_specials_entry
    } varsets;

    struct {
      rbtree set;     // config_specials_entry
    } rulesets;
  } modules;
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

typedef struct config_param {
  const struct bacon_value *val;
  uint64_t min;
  uint64_t max;
  const char *config_path;
} config_param;

void config_validate(const config_param * restrict param)
  __attribute__((nonnull));
