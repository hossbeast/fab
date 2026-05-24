/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/resource.h>

#include "types.h"

#include "xresource.h"

void xsetrlimit(int resource, const struct rlimit * restrict rlim)
{
  RUNTIME_ASSERT(setrlimit(resource, rlim) == 0);
}
