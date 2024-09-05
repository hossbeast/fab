/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

/* bottom 32 bits of the pid shifted into the top 32 bits */
#define PID_AS_MSGID(pid) ((((uint64_t)pid & UINT32_MAX) << 32) | 1)

/* top 32 bits shifted down */
#define PID_FROM_MSGID(id) (uint32_t)(id >> 32)

// non-configurable execution parameters
extern struct g_params
{
  pid_t    pid;            // pid of this process
  bool     shutdown;       // whether to terminate the main loop
  bool     waserror;

  char     ipcdir[PATH_MAX];
  uint16_t ipcdir_len;

  char     cwd[PATH_MAX];  // working directory
  uint16_t cwd_len;
  char     cwd_hash[40];
  uint16_t cwd_hash_len;

#if CONFIG_DEVEL
  char    *exedir;         // path to the directory containing the executable
#endif
} g_params;

/// params_parse
//
// SUMMARY
//  parses command-line options, populating g_params
//
void params_setup(void);

/// params_teardown
//
// free g_params
//
void params_teardown(void);

size_t params_path_hash(char *dst, size_t dst_size, const char * restrict path, uint16_t path_len)
  __attribute__((nonnull));
