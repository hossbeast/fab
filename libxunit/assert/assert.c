/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "common/color.h"
#include "xlinux/xunistd.h"

#include "xunit.h"
#include "assert.h"
#include "type.internal.h"
#include "info.internal.h"

#include "zcat.h"

uint32_t API xunit_assertions_passed;
uint32_t API xunit_assertions_failed;

//
// API
//

static int sayf(void * restrict cbuf, size_t clen, const char * restrict fmt, ...)
{
  va_list va;
  int r;

  if(clen)
  {
    axwrite(2, cbuf, clen);
  }

  va_start(va, fmt);
  r = vdprintf(2, fmt, va);
  va_end(va);

  if(clen)
  {
    axwrite(2, NOCOLOR);
  }

  return r;
}

static void infos_trace(void)
{
  info * nfo = 0;
  int x;
  int y;
  size_t z = 0;

  for(x = 0; x < xunit_infos_stagingl; x++)
  {
    nfo = &xunit_infos_staging[x];

    for(y = 0; y < x; y++)
    {
      // determine whether an info by this name was already used
      if(memncmp(xunit_infos_staging[y].ks, xunit_infos_staging[y].kl, nfo->ks, nfo->kl) == 0) {
        break;
      }
    }
    if(y != x) {
      continue;
    }

    if(z == 0)
    {
      z += sayf(DIM_CYAN, " > ");
    }
    else
    {
      z += sayf(DIM_CYAN, " ");
    }

    z += sayf(LIGHT_YELLOW, "%.*s", (int)nfo->kl, nfo->ks);
    z += sayf(LIGHT_GRAY, " %.*s", (int)nfo->vl, nfo->vs);
  }

  z += sayf(NOCOLOR, "\n");
}

void API xunit_assertion_evaluate(const xunit_type * const restrict type, uint8_t op, const char * const restrict value, ...)
{
  va_list va;
  xunit_arg expected;
  xunit_arg actual;
  int r;
  const char *expected_label;

  va_start(va, value);

  if(op == XU_NULL || op == XU_NOTNULL)
  {
    type->xu_unpack(va, &actual);
    r = actual.d;
  }
  else
  {
    type->xu_unpack(va, &expected);
    type->xu_unpack(va, &actual);
    r = type->xu_compare(&expected, &actual);
  }

       if(op == XU_EQ && r) { }
  else if(op == XU_NE && r == 0) { }
  else if(op == XU_GT && r > 0) { }
  else if(op == XU_GE && r >= 0) { }
  else if(op == XU_LT && r < 0) { }
  else if(op == XU_LE && r <= 0) { }
  else if(op == XU_NULL && r) { }
  else if(op == XU_NOTNULL && !r) { }
  else
  {
    xunit_assertions_passed++;
    return;
  }

  xunit_assertions_failed++;
  xunit_info_pushs("value", value);
  if(op == XU_NE)
  {
    type->xu_info_push("expected not", &expected);
    type->xu_info_push("actual", &actual);
  }
  else if(op == XU_NULL || op == XU_NOTNULL)
  {
    xunit_info_pushs("expected", op == XU_NULL ? "(null)" : "(notnull)");
    type->xu_info_push("actual", &actual);
  }
  else
  {
    expected_label = "expected";
    if(op == XU_GT) { expected_label = "expected >"; }
    if(op == XU_GE) { expected_label = "expected >="; }
    if(op == XU_LT) { expected_label = "expected <"; }
    if(op == XU_LE) { expected_label = "expected <="; }

    type->xu_info_push(expected_label, &expected);
    type->xu_info_push("actual", &actual);
  }

  infos_trace();
  RTA;
}
