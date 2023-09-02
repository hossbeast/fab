/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "xinet.h"

//
// api
//

void API xinet_pton(int af, const char * restrict src, void * restrict dst)
{
  int r;
  r = inet_pton(af, src, dst);
  RUNTIME_ASSERT(r == 1);
}
