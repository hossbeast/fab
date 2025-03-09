/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "moria.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "descriptor.h"
#include "stats.h"
#include "vertex.h"

extern descriptor_type descriptor_stats_client;

typedef union stats_client {
  struct {
    /* event counters */
    uint32_t stathash;
  };

  stats_record record;
} stats_client;

/* VERTEX_CLIENT */
typedef struct client {
  moria_vertex vertex;

  stats_vertex stats;
  stats_client client_stats;
  char id[40];
  uint16_t id_len;

  struct timespec create_time;  // time when created, monotonic clock

  struct channel *channel;      // channel-based client
} client;

extern llist client_list;

void client_setup(void);
void client_cleanup(void);

client *client_alloc(void);
void client_release(client *);

void client_advertise(struct client * restrict client)
  __attribute__((nonnull));

void client_excise(struct client * restrict client)
  __attribute__((nonnull));

void client_describe(const client *fst, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void client_identity_describe(const client * restrict client, struct bacon_writer * restrict writer)
  __attribute__((nonnull));

void client_show(const client *fst)
  __attribute__((nonnull));

void client_stats_build(client * restrict client, bool reset, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t client_path_cat(void * restrict dst, size_t sz, const client * restrict client)
  __attribute__((nonnull));

void client_hash(const client * restrict v, const void ** restrict hp, uint16_t * restrict hl)
  __attribute__((nonnull));

void client_id(const client * restrict v, const char ** restrict idp, uint16_t * restrict idl)
  __attribute__((nonnull));
