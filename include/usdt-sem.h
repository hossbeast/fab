/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#if CONFIG_PROBES
#define _SDT_HAS_SEMAPHORES
#include <sys/sdt.h>
#endif

#define PROBE_SEM(provider, probe)                                            \
  __extension__ /* dont warn on gcc extensions */                             \
  unsigned short provider##_##probe##_semaphore                               \
  __attribute__((unused))                                                     \
  __attribute__((section (".probes"))) __attribute__((visibility ("hidden")))

#include "usdt.h"
