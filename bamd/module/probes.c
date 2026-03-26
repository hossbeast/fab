/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "module.h"
#include "ruleset.h"
#include "probes.h"

#include "usdt-sem.h"
#include "zcat.h"

PROBE_SEM(module, rebuild) = 1;
PROBE_SEM(module, reparse) = 1;
PROBE_SEM(module, remake) = 1;
PROBE_SEM(module, reimport) = 1;
PROBE_SEM(module, reload) = 1;

void ruleset_rebuild_probe(const ruleset * restrict rules)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_rebuild_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "ruleset rebuild %p %s", rules, rules->id);
  z += zcatf(s + z, sz - z, " %.*s", rules->self_node_abspath_len, rules->self_node_abspath);

  printf("%.*s\n", (int)z, buf);
}

void module_rebuild_probe(const module * restrict mod)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_rebuild_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "module rebuild %p %s", mod, mod->id);
  z += zcatf(s + z, sz - z, " %.*s", mod->build_dir_abspath_len, mod->build_dir_abspath);

  printf("%.*s\n", (int)z, buf);
}

void module_reparse_probe(const module * restrict mod)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_reparse_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "module reparse %p %s", mod, mod->id);
  z += zcatf(s + z, sz - z, " %.*s", mod->build_dir_abspath_len, mod->build_dir_abspath);

  printf("%.*s\n", (int)z, buf);
}

void module_remake_probe(const module * restrict mod)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_remake_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "module remake %p %s", mod, mod->id);
  z += zcatf(s + z, sz - z, " %.*s", mod->build_dir_abspath_len, mod->build_dir_abspath);

  printf("%.*s\n", (int)z, buf);
}

void module_reimport_probe(const module * restrict mod)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_reimport_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "module reimport %p %s", mod, mod->id);
  z += zcatf(s + z, sz - z, " %.*s", mod->build_dir_abspath_len, mod->build_dir_abspath);

  printf("%.*s\n", (int)z, buf);
}

void module_reload_probe(const module * restrict mod)
{
  char buf[PATH_MAX];
  char *s;
  size_t sz;
  size_t z;

  if(!module_reload_semaphore) {
    return;
  }

  s = buf;
  sz = sizeof(buf);
  z = 0;

  z += zcatf(s + z, sz - z, "module reload %p %s", mod, mod->id);
  z += zcatf(s + z, sz - z, " %.*s", mod->build_dir_abspath_len, mod->build_dir_abspath);

  printf("%.*s\n", (int)z, buf);
}
