/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "plugin_process.h"

plugin_process *plugin_process_alloc(void)
{
  plugin_process *process;

  xmalloc(&process, sizeof(*process));

  exec_builder_init(&process->eb);
  process->pid = -1;

  return process;
}

void plugin_process_free(plugin_process *process)
{
  if(!process) {
    return;
  }

  xclose(process->stdin_pipe[0]);
  xclose(process->stdin_pipe[1]);
  xclose(process->stdout_pipe[0]);
  xclose(process->stdout_pipe[1]);
  xclose(process->stderr_pipe[0]);
  xclose(process->stderr_pipe[1]);
  xclose(process->auxout_pipe[0]);
  xclose(process->auxout_pipe[1]);

  exec_builder_destroy(&process->eb);

  free(process);
}

void plugin_process_reset(plugin_process *process)
{
  process->pid = -1;
  exec_builder_reset(&process->eb);

  process->status = 0;
  process->ready = 0;
  process->running = 0;
  process->killed_term = 0;
  process->killed_waited = 0;
  process->killed_kill = 0;
  memset(&process->started_at, 0, sizeof(process->started_at));
  memset(&process->stopped_at, 0, sizeof(process->stopped_at));
  process->out_len = 0;
  process->out_total = 0;
  process->err_len = 0;
  process->err_total = 0;
  process->aux_len = 0;
  process->aux_total = 0;
}
