/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"
#include "moria.h"
#include "descriptor.h"
#include "stats.h"
#include "types.h"

#include "filesystem.h"
#include "vertex.h"

struct bacon_writer;
struct narrator;
struct bacon_value;
struct config;
struct configset_parser;
struct graph_invalidation_context;
struct fsedge;

/* global VERTEX_CONFIGSET_FILE nodes */
extern struct fsent *system_configset_node;
extern struct fsent *user_configset_node;

/* active config list */
extern llist configset_list;

extern descriptor_type descriptor_stats_configset;

/* VERTEX_CONFIGSET - config.bam file */
typedef union stats_configset {
  struct {
    /* event counters */
    uint16_t parse_ok;       /* parsed successfully */
    uint16_t parse_err;      /* parse failed */
  };

  stats_record record;
} stats_configset;

/* VERTEX_CONFIGSET */
typedef struct configset {
  moria_vertex vertex;

  stats_vertex stats;
  stats_configset configset_stats;
  struct bacon_value *val;
  char id[40];
  uint16_t id_len;

  /* regarding the corresponding VERTEX_CONFIGSET_FILE */
  struct fsent *self_node;
  char self_node_abspath[PATH_MAX];
  uint16_t self_node_abspath_len;
} configset;

void configset_parse(configset * restrict, struct configset_parser * restrict parser, struct config * restrict)
  __attribute__((nonnull));

void configset_create(configset ** restrict cfg)
  __attribute__((nonnull));

/* free a config struct with free semantics */
void configset_release(configset * restrict cfg);

/* mark sections of a new config struct as different from those of another config struct */
bool configset_compare(configset * restrict new, configset * restrict old)
  __attribute__((nonnull));

void configset_merge(configset * restrict dst, configset * restrict src)
  __attribute__((nonnull));

void configset_setup(void);
void configset_cleanup(void);

void configset_alloc(configset ** restrict cfg)
  __attribute__((nonnull));

/*
 * Write a config stats to a buffer
 *
 * reset - true to reset the stats while reading them
 * zp    - (returns) number of bytes written to dst
 */
void configset_stats_collate(configset * restrict config, bool reset)
  __attribute__((nonnull));

size_t configset_path_cat(void * restrict dst, size_t sz, const configset * restrict config)
  __attribute__((nonnull));

void configset_describe(const struct configset * restrict configset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void configset_file_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void configset_identity_describe(const struct configset * restrict configset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void configset_stats_build(configset * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void configset_file_stats_build(struct fsent * restrict, bool reset, struct bacon_writer * restrict)
  __attribute__((nonnull));

void configset_hash(const configset * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void configset_id(const configset * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));

void configset_compute_id(const struct configset * restrict v, struct identity_ctx * restrict id)
  __attribute__((nonnull));

void configsets_show(void);
