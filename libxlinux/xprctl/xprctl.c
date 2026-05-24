/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/prctl.h>

#include "types.h"

#include "xprctl.h"

void xprctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
  RUNTIME_ASSERT(prctl(option, arg2, arg3, arg4, arg5) == 0);
}
