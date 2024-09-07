/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "plugin.h"
#include "plugin_process.h"
#include "plugin/probes.h"
#include "exec.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(plugin, exec) = 1;
PROBE_SEM(plugin, quiesce) = 1;
PROBE_SEM(plugin, unready) = 1;
PROBE_SEM(plugin, ready) = 1;
PROBE_SEM(plugin, reap) = 1;

void plugin_exec_probe(const struct plugin * restrict plugin, const struct exec * restrict exec)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;
  int x;

  if(!plugin_exec_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s pid %d\n"
    , "exec"
    , (int)plugin->id_len
    , plugin->id
    , (int)plugin->self_node->vertex.label_len
    , plugin->self_node->vertex.label
    , plugin->process->pid
  );
  for(x = 0; x < exec->args_size; x++) {
    z += zcatf(s + z, sz - z, " [%2d] %s\n", x, exec->args[x]);
  }
  for(x = 0; x < exec->envs_size; x++) {
    z += zcatf(s + z, sz - z, " [%2d] %s\n", x, exec->envs[x]);
  }
  s[z] = 0;

(void)!write(1, buf, z);
  PROBE2(plugin, exec, buf, v);
}

void plugin_unready_probe(const struct plugin * restrict plugin)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;

  if(!plugin_unready_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s\n"
    , "unready"
    , (int)plugin->id_len
    , plugin->id
    , (int)plugin->self_node->vertex.label_len
    , plugin->self_node->vertex.label
  );

(void)!write(1, buf, z);
  PROBE2(plugin, unready, buf, v);
}

void plugin_ready_probe(const struct plugin * restrict plugin)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;

  if(!plugin_ready_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s\n", "ready"
    , (int)plugin->id_len
    , plugin->id
    , (int)plugin->self_node->vertex.label_len
    , plugin->self_node->vertex.label
  );

(void)!write(1, buf, z);
  PROBE2(plugin, ready, buf, v);
}

void plugin_quiesce_probe(const struct plugin * restrict plugin)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;

  if(!plugin_quiesce_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s\n", "quiesce", (int)plugin->id_len, plugin->id, (int)plugin->self_node->vertex.label_len, plugin->self_node->vertex.label);

(void)!write(1, buf, z);
  PROBE2(plugin, quiesce, buf, v);
}

void plugin_reap_probe(const struct plugin * restrict plugin)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;

  if(!plugin_reap_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s exit %d\n"
    , "reap"
    , (int)plugin->id_len, plugin->id
    , (int)plugin->self_node->vertex.label_len, plugin->self_node->vertex.label
    , plugin->process->status
  );

(void)!write(1, buf, z);
  PROBE2(plugin, reap, buf, v);
}
