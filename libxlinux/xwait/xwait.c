/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "xwait.h"

void xwait(int * restrict status)
{
  RUNTIME_ASSERT(wait(status) == 0);
}

void xwaitpid(pid_t pid, int * restrict status, int options)
{
  int r;

  r = waitpid(pid, status, options);
  RUNTIME_ASSERT(r == pid);
}

void xwaitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
  int r;

  r = waitid(idtype, id, infop, options);
  RUNTIME_ASSERT(r == 0);
}

void uxwaitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
  int r;

  r = waitid(idtype, id, infop, options);
  RUNTIME_ASSERT(r == 0 || errno == ECHILD);
}
