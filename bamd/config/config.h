/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "moria.h"
#include "bam/stats.h"

#include "filesystem.h"

struct bacon_writer;
struct narrator;
struct bacon_value;
struct configuration;
struct config_parser;
struct graph_invalidation_context;

/* global VERTEX_CONFIG_FILE | VERTEX_PROTECT_BIT nodes */
extern struct fsent *system_config_node;
extern struct fsent *user_config_node;

/* active config list */
extern llist config_list;

/* VERTEX_CONFIG */
typedef struct config {
  moria_vertex vertex;

  bam_vertex_stats stats;
  struct bacon_value *val;
  char id[40];
  uint16_t id_len;
  char token[40];
  uint16_t token_len;

  /* regarding the corresponding VERTEX_CONFIG_FILE */
  struct fsent *self_node;
  bam_config_stats config_stats;
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;

  /* parsed from the file */
//  struct config *cfg;
} config;

void config_parse(config * restrict, struct config_parser * restrict parser, struct configuration * restrict)
  __attribute__((nonnull));

void config_create(config ** restrict cfg)
  __attribute__((nonnull));

/* free a config struct with free semantics */
void config_release(config * restrict cfg, struct graph_invalidation_context * restrict invalidation);

/* mark sections of a new config struct as different from those of another config struct */
bool config_compare(config * restrict new, config * restrict old)
  __attribute__((nonnull));

void config_merge(config * restrict dst, config * restrict src)
  __attribute__((nonnull));

void config_describe(config * const restrict cfg, struct bacon_writer * const restrict writer)
  __attribute__((nonnull));

void config_say(config * restrict cfg, struct narrator * restrict N)
  __attribute__((nonnull));

void config_setup(void);
void config_cleanup(void);

void config_alloc(config ** restrict cfg)
  __attribute__((nonnull));

/*
 * Write a config stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
size_t config_stats_collate(void *dst, size_t sz, bool reset, config * restrict cfg)
  __attribute__((nonnull));
