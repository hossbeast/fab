/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "types.h"
#include "bam/stats.h"
#include "filesystem.h"

struct bacon_value;
struct bacon_writer;
struct narrator;
struct fsent;
struct config;
struct graph_invalidation_context;
struct config_parser;

typedef struct configuration_filesystems_entry {
  struct {
    struct bacon_value *v;
    invalidate_type e;
  } invalidate;
  struct {
    struct bacon_value *v;
    bool b;
  } mkdirs;
  struct {
    struct bacon_value *v;
    bool b;
  } readlinks;
  struct {
    struct bacon_value *v;
    bool b;
  } indexed;
} configuration_filesystems_entry;

typedef struct configuration_filesystems_mapping {
  llist lln;
  rbnode rbn;

  struct {
    struct bacon_value *v;

    /* normalized */
    char path[PATH_MAX];
    uint16_t path_len;
  } key;

  configuration_filesystems_entry val;
} configuration_filesystems_mapping;

typedef struct configuration_walker_special_entry {
  llist lln;
  rbnode rbn;
  struct bacon_value *v;
  char *s;
  uint16_t len;
} configuration_walker_special_entry;

typedef struct configuration_formula_path_dirs_entry {
  llist lln;
  rbnode rbn;
  struct bacon_value *v;
  char *s;
  uint16_t len;
} configuration_formula_path_dirs_entry;

typedef struct configuration {
  llist owner;

  struct {
    struct {
      struct bacon_value *v;
      uint16_t u16;
    } concurrency;
  } build;

  struct config_walker {
    struct {
      rbtree set;     // struct pattern
    } include;

    struct {
      rbtree set;     // struct pattern
    } exclude;

    struct {
      rbtree set;     // configuration_walker_special_entry
    } special;
  } walker;

  struct config_filesystems {
    rbtree set;               // configuration_filesystems_mapping
  } filesystems;

  struct {
    struct {
      struct {
        struct bacon_value *v;
        bool b;
      } copy_from_env;
      struct {
        rbtree set;           // configuration_formula_path_dirs_entry
      } dirs;
    } path;
  } formula;
} configuration;

extern struct configuration *configuration_active;

void configuration_alloc(configuration ** restrict rv)
  __attribute__((nonnull));

void configuration_free(configuration * restrict cfg, struct config_parser * restrict parser);

void configuration_ifree(configuration ** restrict cfg, struct config_parser * restrict parser)
  __attribute__((nonnull));

void configuration_say(const configuration * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

void configuration_describe(const configuration * restrict cfg, struct bacon_writer * const restrict builder_writer)
  __attribute__((nonnull));

void configuration_show(const configuration * restrict cfg)
  __attribute__((nonnull));

void configuration_compare(const configuration * restrict prev, configuration * restrict next)
  __attribute__((nonnull(2)));

void configuration_setup(void);
void configuration_cleanup(void);
