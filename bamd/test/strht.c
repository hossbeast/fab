/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <stdarg.h>

#include "types.h"

#include "valyria/hashtable.h"
#include "xunit/type.h"
#include "xunit/info.h"
#include "common/stresc.h"
#include "common/hash.h"

#include "strht.h"

#include "zcat.h"

static void strht_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int strht_compare(xunit_arg * A, xunit_arg * B)
{
  return !hashtable_equal(A->p, B->p);
}

static void strht_info_push(const char * const restrict name, xunit_arg * a)
{
  hashtable * ht;
  char buf[256];
  size_t bufsz = sizeof(buf);
  size_t bufz = 0;
  int x;
  const void * e;

  if(a->p == 0)
  {
    xunit_info_pushs(name, "(null)");
    return;
  }

  ht = a->p;
  bufz += zcats(buf + bufz, bufsz - bufz, "{");

  for(x = 0; x < ht->table_size; x++)
  {
    if(!(e = hashtable_table_entry(ht, x))) {
      continue;
    }

    if(bufz > 1) {
      bufz += zcats(buf + bufz, bufsz - bufz, " ");
    }

    bufz += strescw(buf + bufz, bufsz - bufz, e, strlen(e));
  }

  bufz += zcats(buf + bufz, bufsz - bufz, "}");

  xunit_info_pushw(name, buf, bufz);
}

uint32_t strht_hash(uint32_t h, const void *ent)
{
  return hash32(h, ent, strlen(ent));
}

int strht_cmp(const void *A, const void *B)
{
  return strcmp(A, B);
}

xunit_type * xunit_strht = (xunit_type[]) {{
    xu_unpack : strht_unpack
  , xu_compare : strht_compare
  , xu_info_push : strht_info_push
}};
