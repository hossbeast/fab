/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "common/attrs.h"

#include "subsys.h"
#include "vertex.h"
#include "identity.h"

void subsys_init(subsys * restrict v, vertex_type type)
{
  identity_ctx id;
  const char *name;
  uint16_t name_len;

  name = attrs32_name_byvalue(graph_vertex_type_attrs, type);
  RUNTIME_ASSERT(name);
  name_len = strlen(name);

  moria_vertex_initw(&v->vertex, &g_graph, type | VERTEX_OK, name, name_len);

  identity_init(&id);
  subsys_compute_id(v, &id);
  v->id_len = identity_mk(&id, v->id, sizeof(v->id));
}

void subsys_id(const subsys * restrict v, const char ** restrict id, uint16_t * restrict idl)
{
  *id = v->id;
  *idl = v->id_len;
}

void subsys_compute_id(const struct subsys * restrict subsys, struct identity_ctx * restrict id)
{
  identity_type(id, vertex_type_get(&subsys->vertex));
}
