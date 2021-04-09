/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "stats.h"
#include "config.h"
#include "formula.h"
#include "fsent.h"
#include "marshal.h"
#include "module.h"
#include "var.h"

fab_global_stats g_stats;

typedef struct stat_def {
  uint32_t offset;
  uint8_t size;
} stat_def;

//
// public
//

static xapi collate_global(void *dst, size_t sz, fab_global_stats *stats, bool reset, size_t *zp)
{
  enter;

  size_t z;
  fab_global_stats lstats;
  descriptor_field *field;
  module *mod;
  statement_block *block;
  int x;
  uint32_t count;

  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_global_stats.id);

  /* rules */
  count = 0;
  llist_foreach(&module_list, mod, vertex.owner) {
    if(mod->unscoped_block) {
      count += llist_count(&mod->unscoped_block->rules);
    }
    llist_foreach(&mod->scoped_blocks, block, lln) {
      count += llist_count(&block->rules);
    }
  }
  z += marshal_u32(dst + z, sz - z, count);

  /* rmas */
  count = 0;
  llist_foreach(&module_list, mod, vertex.owner) {
    count += llist_count(&mod->rmas_owner);
  }
  z += marshal_u32(dst + z, sz - z, count);

  /* models */
  count = 0;
  llist_foreach(&module_list, mod, vertex.owner) {
    if(fsent_kind_get(mod->self_node) == VERTEX_MODEL_FILE) {
      count++;
    }
  }
  z += marshal_u32(dst + z, sz - z, count);

  /* modules */
  count = 0;
  llist_foreach(&module_list, mod, vertex.owner) {
    if(fsent_kind_get(mod->self_node) == VERTEX_MODULE_FILE) {
      count++;
    }
  }
  z += marshal_u32(dst + z, sz - z, count);

  /* event counters */
  for(x = 4; x < descriptor_fab_global_stats.members_len; x++)
  {
    field = descriptor_fab_global_stats.members[x];
    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u16(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  *zp += z;

  finally : coda;
}

static size_t collate_node(void *dst, size_t sz, fsent * restrict n, bool reset)
{
  size_t z;
  fab_fsent_stats *stats;
  fab_fsent_stats lstats;
  descriptor_field *field;
  int x;
  uint16_t len;
  size_t len_off;
  uint32_t val;

  stats = &n->stats;
  if(reset)
  {
    memcpy(&lstats, stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_fsent_stats.id);

  /* abs path */
  len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);
  len = fsent_absolute_path_znload(dst + z, sz - z, n);
  marshal_u16(dst + len_off, sz - len_off, len);
  z += len;

  /* kind */
  val = vertex_kind_remap(fsent_kind_get(n));
  z += marshal_u32(dst + z, sz - z, val);

  /* state */
  val = vertex_state_remap(fsent_state_get(n));
  z += marshal_u32(dst + z, sz - z, val);

  /* event counters */
  for(x = 3; x < descriptor_fab_fsent_stats.members_len; x++)
  {
    field = descriptor_fab_fsent_stats.members[x];
    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u8(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  return z;
}

xapi stats_global_collate(void *dst, size_t sz, bool reset, size_t *zp)
{
  enter;

  fatal(collate_global, dst, sz, &g_stats, reset, zp);

  finally : coda;
}

xapi stats_node_collate(void *dst, size_t sz, fsent * restrict n, bool reset, size_t *zp)
{
  enter;

  size_t z;
  vertex_kind kind;

  z = 0;
  z += collate_node(dst + z, sz - z, n, reset);

  kind = fsent_kind_get(n);
  if(kind == VERTEX_MODULE_DIR)
  {
    fatal(module_collate_stats, dst + z, sz - z, n->self_mod, reset, &z);
  }
  else if(kind == VERTEX_MODULE_FILE)
  {
    fatal(module_file_collate_stats, dst + z, sz - z, n, reset, &z);
  }
  else if(kind == VERTEX_FORMULA_FILE)
  {
    fatal(formula_collate_stats, dst + z, sz - z, n->self_fml, reset, &z);
  }
  else if(kind == VERTEX_VAR_FILE)
  {
    fatal(var_collate_stats, dst + z, sz - z, n->self_var, reset, &z);
  }
  else if(kind == VERTEX_CONFIG_FILE)
  {
    fatal(config_collate_stats, dst + z, sz - z, n->self_config, reset, &z);
  }

  *zp += z;

  finally : coda;
}
