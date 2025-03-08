/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "grow.h"
#include "assure.h"

void grow(void * target, size_t es, size_t len, size_t ec, size_t * const restrict ac)
{
  assure(target, es, ec + len, ac);
}
