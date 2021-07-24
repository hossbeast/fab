#ifndef _PROBES_H
#define _PROBES_H

#include <signal.h>

extern unsigned short sigutil_sigrx_semaphore;
extern unsigned short sigutil_sigtx_semaphore;

void sigrx_probe(const siginfo_t * restrict info)
  __attribute__((nonnull));

void sigtx_probe(int signo, pid_t pid, pid_t tid);

#endif
