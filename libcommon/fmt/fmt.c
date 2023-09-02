/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "fmt.h"

//
// public
//

void API fmt_apply(char * const restrict dst, size_t dst_size, const char * const restrict fmt, va_list va)
{
  int rv;

  rv = vsnprintf(dst, dst_size, fmt, va);
  if(rv < 0 || rv >= dst_size) {
    RTA;
  }
}
