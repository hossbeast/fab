/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "common/sha1.h"
#include "common/attrs.h"
#include "common/bech32.h"

#include "identity.h"
#include "zcat.h"
#include "graph.h"

void identity_init(identity_ctx * restrict id)
{
  sha1_init(&id->sha1);
}

void identity_add(identity_ctx * restrict id, const void * restrict data, uint16_t len)
{
  sha1_update(&id->sha1, data, len);
}

void identity_type(identity_ctx * restrict id, enum vertex_type type)
{
  const char *name;

  name = attrs32_name_byvalue(graph_vertex_type_attrs, type);
  RUNTIME_ASSERT(name);

  identity_add(id, name, strlen(name));
}

void identity_name(identity_ctx * restrict id, enum vertex_type type, const char * restrict name, uint16_t name_len)
{
  identity_type(id, type);
  if(name_len) {
    identity_add(id, " ", 1);
  }
  identity_add(id, name, name_len);
}

uint16_t identity_mk(identity_ctx * restrict ctx, char * restrict id, size_t id_size)
{
  uint8_t u8[20];
  int z;

  sha1_final(u8, &ctx->sha1);

  z = bech32_encode(id, id_size, "bm", 2, u8, 12);

  return z;
}
