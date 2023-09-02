/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/hash.h"

#include "keywords.h"

#include "macros.h"

static uint32_t keyword_hash(uint32_t h, const void *ent)
{
  const keyword *k = ent;
  return hash32(h, k->s, k->len);
}

static int keyword_cmp(const void *A, const void *B)
{
  const keyword *a, *b;
  a = A;
  b = B;
  return memncmp(a->s, a->len, b->s, b->len);
}

static keyword keyword_true = { s : "true", len : 4 };
static keyword keyword_false = { s : "false", len : 5 };
hashtable keywords;

//
//
//

void keywords_setup()
{
  hashtable_initx(&keywords, 0, keyword_hash, keyword_cmp, 0);

  // keywords
  hashtable_put(&keywords, &keyword_true);
  hashtable_put(&keywords, &keyword_false);
}

void keywords_cleanup()
{
  hashtable_destroy(&keywords);
}
