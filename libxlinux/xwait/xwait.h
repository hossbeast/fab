/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/wait.h>


/// xwait
//
// SUMMARY
//  xapi proxy for wait
//
void xwait(int * restrict status)
  __attribute__((nonnull));

/// xwaitpid
//
// SUMMARY
//  xapi proxy for waitpid
//
void xwaitpid(pid_t pid, int * restrict status, int options)
  __attribute__((nonnull(2)));

void xwaitid(idtype_t idtype, id_t id, siginfo_t * restrict infop, int options)
  __attribute__((nonnull));

void uxwaitid(idtype_t idtype, id_t id, siginfo_t * restrict infop, int options)
  __attribute__((nonnull));
