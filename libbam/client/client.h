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
extern const char *g_bam_client_default_filesystem_invalidation;
extern const char *g_bam_client_sweeper_period_nsec;
#endif

struct bamipc_channel;
struct bamipc_message;
struct __dirstream;

typedef struct bam_client {
  /* configuration */
  char *basedir;              /* /home/todd/bam-build */
  uint16_t basedir_len;
  char *ipcdir_base;          /* /run/user/1000/bam */
  uint16_t ipcdir_base_len;
  char *ipcdir;               /* /run/user/1000/bam/627e770a319a6cda */
  uint16_t ipcdir_len;
  uint64_t hash;
  char hash_str[16 + 1];      /* 627e770a319a6cda */

  /* runtime state */
  int64_t bamd_pid;           /* bamd process group id */
  int lockfd;                 /* */
  int32_t futex;
  uint64_t  msgid;            /* autoinc */
  struct bamipc_channel *shm;
} bam_client;

/*
 * create a bam client
 *
 * client   - (returns) client instance
 * basedir  - base directory path (contains module.bam)
 * ipcdir   - base ipc dir
 */
void bam_client_init(
    bam_client * restrict client
  , const char * restrict basedir
  , uint16_t base_len
  , const char * restrict ipcdir
  , uint16_t ipcdir_len
)
  __attribute__((nonnull));

/* attempt to acquire the lockfile */
void bam_client_lockfile_query(bam_client * restrict client)
  __attribute__((nonnull));

/* attempt to acquire the lockfile, cleanup the lockfile if its in an invalid state */
void bam_client_lockfile_validate(bam_client * restrict client)
  __attribute__((nonnull));

/* launch bamd */
void bam_client_launch(bam_client * restrict client)
  __attribute__((nonnull));

/* release bam client */
void bam_client_destroy(bam_client * restrict client);

/* initiate connection to bamd - bamd_pid must be set */
int bam_client_solicit(bam_client * restrict client)
  __attribute__((nonnull));

/* attach the shm channel - channel_id was received from bamd via signal in response to solicit */
void bam_client_attach(bam_client * restrict client, int channel_id)
  __attribute__((nonnull));

/* graceful close of the shm channel */
void bam_client_disconnect(bam_client * restrict client)
  __attribute__((nonnull));

/* prepare a message in the shm channel for post */
struct bamipc_message *bam_client_produce(bam_client * restrict client)
  __attribute__((nonnull));

/* post a message on the shm channel - the peer can now receive it */
void bam_client_post(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

/* attempt to receive a message posted on the shm channel by the peer */
struct bamipc_message *bam_client_acquire(bam_client * restrict client)
  __attribute__((nonnull));

/* release a previously acquired message */
void bam_client_consume(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

/* release acquired messages up to and including the specified message */
void bam_client_release(bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));
