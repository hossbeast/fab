/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "exec_builder.h"
#include "exec_render.h"
#include "request.h"

struct plugin;
struct bacon_parser;

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

  int stdin_pipe[2];
  int stdout_pipe[2];
  int stderr_pipe[2];
  int auxout_pipe[2];

  /* stdin ; plugins receive results on stdin */
  /* circular buffer ; trailing portion is retained for visibility */
  char *in;
  uint32_t in_size;
  uint32_t in_tail;
  uint32_t in_head;
  uint32_t in_total;

  /* stdout ; plugins can write to stdout for tracing */
  /* circular buffer ; trailing portion is retained for visibility */
  char *out;
  uint32_t out_size;
  uint32_t out_tail;
  uint32_t out_head;
  uint32_t out_total;

  /* stderr */
  /* circular buffer ; trailing portion is retained for visibility */
  char *err;
  uint32_t err_size;
  uint32_t err_tail;
  uint32_t err_head;
  uint32_t err_total;

  /* auxout ; plugins write requests on the auxout fd */
  /* circular buffer ; trailing portion is retained for visibility */
  char *auxout;
  uint32_t auxout_size;
  uint32_t auxout_tail;
  uint32_t auxout_head;
  uint32_t auxout_total;

  /* most recent request copied into contiguous space for parsing */
  char last_buf[8192];
  uint32_t last_buf_len;

  /* most recent request, parsed */
  request last_req;
} plugin_process;

plugin_process *plugin_process_alloc(void);
void plugin_process_free(plugin_process *process);

void plugin_process_cleanup(void);
