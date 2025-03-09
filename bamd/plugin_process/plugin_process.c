/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "bacon/parser.h"

#include "plugin_system.h"
#include "plugin_process.h"

plugin_process *plugin_process_alloc(void)
{
  plugin_process *process;

  xmalloc(&process, sizeof(*process));

  xmalloc(&process->in, plugin_system_stdin_size);
  process->in_size = plugin_system_stdin_size;

  xmalloc(&process->out, plugin_system_stdout_size);
  process->out_size = plugin_system_stdout_size;

  xmalloc(&process->err, plugin_system_stderr_size);
  process->err_size = plugin_system_stderr_size;

  xmalloc(&process->auxout, plugin_system_auxout_size);
  process->auxout_size = plugin_system_auxout_size;

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

  free(process->in);
  free(process->out);
  free(process->err);
  free(process->auxout);

  free(process);
}

void plugin_process_cleanup(void)
{
}
