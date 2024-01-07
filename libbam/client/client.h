/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "macros.h"

#if CONFIG_DEVEL
/* null-terminated strings */
extern const char *g_bam_client_bamd_path;
extern const char *g_bam_client_system_config_path;
extern const char *g_bam_client_user_config_path;
extern const char *g_bam_client_default_filesystem_invalidate;
extern const char *g_bam_client_sweeper_period_nsec;
#endif

struct bamipc_channel;
struct bamipc_message;

typedef struct bam_client {
  int       lockfd;
  char *    basedir;      // canonicalized
  char *    ipcdir;
  char      hash[16 + 1];
  pid_t     bamd_pid;     // bamd process group id
  int32_t   futex;

  /* autoinc */
  uint64_t  msgid;

  struct bamipc_channel *shm;
} bam_client;

/*
 * create a bam client
 *
 * client   - (returns) client instance
 * basedir  - base directory path (contains module.bam)
 * ipcdir   - base ipc dir
 */
bam_client *bam_client_create(
    const char * restrict basedir
  , uint16_t base_len
  , const char * restrict ipcdir
  , uint16_t ipcdir_len
)
  __attribute__((nonnull));

/* free bam client */
void bam_client_free(bam_client * restrict client);

/* free bam client, zero its reference */
void bam_client_ifree(bam_client ** restrict client)
  __attribute__((nonnull));

void bam_client_prepare(bam_client * restrict client)
  __attribute__((nonnull));

void bam_client_kill(bam_client * restrict client)
  __attribute__((nonnull));

/*
 * launch bamd if its not already running
 */

void bam_client_solicit(bam_client * restrict client)
  __attribute__((nonnull));

void bam_client_attach(bam_client * restrict client, int channel_id)
  __attribute__((nonnull));

void bam_client_disconnect(bam_client * restrict client)
  __attribute__((nonnull));

struct bamipc_message * bam_client_produce(bam_client * restrict client)
  __attribute__((nonnull));

void bam_client_post(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

struct bamipc_message * bam_client_acquire(bam_client * restrict client)
  __attribute__((nonnull));

void bam_client_consume(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void bam_client_release(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
