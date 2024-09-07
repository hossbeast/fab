/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "exec_builder.h"

struct plugin;

typedef struct plugin_process {
  struct plugin *plugin;
  pid_t pid;
  int status;
  bool ready;
  bool running;
  bool killed_term;
  bool killed_waited;
  bool killed_kill;
  struct timespec started_at;
  struct timespec stopped_at;

  /* for building/storing the exec args */
  exec_builder eb;

  int stdin_pipe[2];
  int stdout_pipe[2];
  int stderr_pipe[2];
  int auxout_pipe[2];

  /* stored here for reporting only */
  char out[4196];
  uint32_t out_len;
  uint32_t out_total;

  /* stored here for reporting only */
  char err[4096];
  uint32_t err_len;
  uint32_t err_total;

  /* plugins write requests on the auxout fd */
  char aux[8192];
  uint32_t aux_len;
  uint32_t aux_total;
} plugin_process;

plugin_process *plugin_process_alloc(void);
void plugin_process_reset(plugin_process *process);
void plugin_process_free(plugin_process *process);
