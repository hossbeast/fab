#ifndef _PROBES_SEM_H
#define _PROBES_SEM_H

#define _SDT_HAS_SEMAPHORES
#include <sys/sdt.h>

#define PROBE_SEM(provider, probe)                                            \
  __extension__ /* dont warn on gcc extensions */                             \
  unsigned short provider##_##probe##_semaphore                               \
  __attribute__((unused))                                                     \
  __attribute__((section (".probes"))) __attribute__((visibility ("hidden")))

#include "usdt.h"

#endif
