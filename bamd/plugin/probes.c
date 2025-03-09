/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/uio.h>

#include "plugin.h"
#include "plugin_process.h"
#include "plugin/probes.h"
#include "exec.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(plugin, exec) = 0;
PROBE_SEM(plugin, unready) = 0;
PROBE_SEM(plugin, ready) = 0;
PROBE_SEM(plugin, reap) = 0;
PROBE_SEM(plugin, tx) = 0;
PROBE_SEM(plugin, rx) = 0;

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
  for(x = 0; x < exec->args_len; x++) {
    z += zcatf(s + z, sz - z, " [%2d] %s\n", x, exec->args[x]);
  }
  for(x = 0; x < exec->envs_len; x++) {
    z += zcatf(s + z, sz - z, " [%2d] %s\n", x, exec->envs[x]);
  }
  s[z] = 0;

(void)!write(1, buf, z);
//  PROBE2(plugin, exec, buf, v);
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
//  PROBE2(plugin, unready, buf, v);
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
//  PROBE2(plugin, ready, buf, v);
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
//  PROBE2(plugin, reap, buf, v);
}

void plugin_tx_probe(const struct plugin * restrict plugin, const struct iovec * restrict iovs, int iovcnt)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;
  int x;

  if(!plugin_tx_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s data "
    , "tx"
    , (int)plugin->id_len, plugin->id
    , (int)plugin->self_node->vertex.label_len, plugin->self_node->vertex.label
  );
  for(x = 0; x < iovcnt; x++) {
    z += zcatw(s + z, sz - z, iovs[x].iov_base, iovs[x].iov_len);
  }
  z += zcats(s + z, sz - z, "\n");

(void)!write(1, buf, z);
//  PROBE3(plugin, tx, plugin->id, data, len);
}

void plugin_rx_probe(const struct plugin * restrict plugin, const void *data, size_t len)
{
  char buf[2048];
  char *s;
  size_t sz;
  size_t z;

  if(!plugin_rx_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "%10s %.*s %.*s data '%.*s'\n"
    , "rx"
    , (int)plugin->id_len, plugin->id
    , (int)plugin->self_node->vertex.label_len, plugin->self_node->vertex.label
    , (int)len
    , (char*)data
  );

(void)!write(1, buf, z);
//  PROBE3(plugin, rx, plugin->id, data, len);
}
