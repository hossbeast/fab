/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <sys/file.h>

#include "types.h"
#include "macros.h"

#include "xfile.h"

void API xflock(int fd, int operation)
{
  RUNTIME_ASSERT(flock(fd, operation) == 0);
}
