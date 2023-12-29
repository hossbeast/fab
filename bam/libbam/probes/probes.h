/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <signal.h>

extern unsigned short sigutil_sigrx_semaphore;
extern unsigned short sigutil_sigtx_semaphore;

void sigrx_probe(const siginfo_t * restrict info)
  __attribute__((nonnull));

void sigtx_probe(int signo, pid_t pid, pid_t tid);
