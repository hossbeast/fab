/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct configuration;

extern char *path_cache_env_path;   // $PATH, if specified in config

typedef struct path_cache_entry {
  struct path_cache_entry *dir;     // directory containing this entry
  int fd;                           // file descriptor
  uint16_t len;                     // length of s
  const char *filename;             // pointer to the filename at the end of s
  char s[];                         // path fragment, or name
} path_cache_entry;

void path_cache_setup(void);
void path_cache_cleanup(void);

/*
 * reapply path cache configuration
 *
 * config - effective config tree
 * dry    - whether to perform a dry-run
 */
void path_cache_reconfigure(struct configuration * restrict cfg, bool dryrun)
  __attribute__((nonnull));

/*
 * get an entry from the path cache
 */
void path_cache_search(const path_cache_entry ** restrict entry, const char * restrict file, uint16_t len)
  __attribute__((nonnull));

/*
 * reset the path cache
 */
void path_cache_reset(void);
