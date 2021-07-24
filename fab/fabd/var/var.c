/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include <errno.h>

#include "common/snarf.h"
#include "descriptor.h"
#include "fab/stats.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value.h"
#include "value/make.h"
#include "value/merge.h"
#include "value/parser.h"
#include "valyria/pstring.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "var.h"
#include "fsent.h"
#include "reconcile.h"
#include "marshal.h"
#include "stats.h"
#include "variant.h"
#include "handler.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"

#include "zbuffer.h"

static llist var_list = LLIST_INITIALIZER(var_list);          // active
static llist var_freelist = LLIST_INITIALIZER(var_freelist);  // free
static value_parser * parser;

//
// static
//

static void var_parse(var * restrict vp)
{
  char * text = 0;
  size_t text_len;
  value * val;
  int exit;
  fabipc_message *msg;
  channel *chan;

  vp->val = 0;

  STATS_INC(vp->stats.parsed_try);
  STATS_INC(g_stats.var_parsed_try);

  snarfs(&text, &text_len, vp->self_node_abspath);

  if(text)
  {
    if((exit = value_parser_parse(parser, text, text_len, vp->self_node_abspath, VALUE_TYPE_SET, &val)))
    {
      system_error = true;
      if(!events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
        goto end;
      }

      msg->code = EINVAL;
      msg->size = znloadw(msg->text, sizeof(msg->text), parser->yyu.error_str, parser->yyu.error_len);
      events_publish(chan, msg);
      goto end;
    }

    vp->val = val;
  }

  STATS_INC(vp->stats.parsed);
  STATS_INC(g_stats.var_parsed);

  //logf(L_MODULE, "parsed var @ %s", vp->self_node_abspath);
end:
  wfree(text);
  void_infos("path", vp->self_node_abspath);
}

/*
 * write an environment variable name
 *
 * N   narrator to write to
 * val name to render
 */
static void __attribute__((nonnull)) get_mapping_key(narrator * restrict N, value * restrict val)
{
  int x;

  RUNTIME_ASSERT(val->type & VALUE_TYPE_SCALAR);

  if(val->type == VALUE_TYPE_STRING)
  {
    for(x = 0; x < val->s->size; x++)
    {
      if(val->s->s[x] == '=')
      {
        logf(L_MODULE | L_WARN, "ignored character '=' in environment variable name");
        continue;
      }

      narrator_xsayc(N, val->s->s[x]);
    }

    narrator_flush(N);
  }
  else
  {
    value_say(val, N);
  }
}

static void var_dispose(var * restrict vp)
{
  llist_delete_node(&vp->vertex.owner);
  llist_append(&var_freelist, vp, vertex.owner);
}

static void var_release(var * restrict vp)
{
  moria_vertex __attribute__((unused)) *v;

  /* should be fully excised from the graph previously */
  v = &vp->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  var_dispose(vp);
}

//
// public
//

void var_alloc(var ** restrict vp, moria_graph * restrict g)
{
  var *v;

  if((v = llist_shift(&var_freelist, typeof(*v), vertex.owner)) == 0)
  {
    xmalloc(&v, sizeof(*v));
  }

  moria_vertex_init(&v->vertex, g, VERTEX_VAR);

  llist_append(&var_list, v, vertex.owner);
  *vp = v;
}

void var_reconcile(var * restrict vp)
{
  vp->reconciliation_id = reconciliation_id;
  if(!fsent_invalid_get(vp->self_node)) {
    goto XAPI_FINALLY;
  }

  var_parse(vp);

  if(!system_error) {
    fsent_ok(vp->self_node);
  }
}

void var_system_reconcile()
{
  var *v;
  llist *T;

  llist_foreach_safe(&var_list, v, vertex.owner, T) {
    if(v->reconciliation_id == reconciliation_id) {
      continue;
    }

    var_release(v);
  }
}

void var_setup()
{
  value_parser_create(&parser);
}

void var_cleanup()
{
  var *v;
  llist *T;

  value_parser_xfree(parser);

  llist_foreach_safe(&var_list, v, vertex.owner, T) {
    var_dispose(v);
  }

  llist_foreach_safe(&var_freelist, v, vertex.owner, T) {
    free(v);
  }
}

void var_denormalize(value_parser * restrict parser, variant * restrict var, value * restrict valset, value ** restrict varsp)
{
  int x;
  narrator * Nkey = 0;
  narrator_fixed Nstor_key;
  char keytext[128];
  const char *name;
  uint16_t name_len;
  value * v;
  value * vars;
  value * realkey;
  value key = { };
  pstring ps = { };

  key.type = VALUE_TYPE_STRING;
  key.s = &ps;

  Nkey = narrator_fixed_init(&Nstor_key, keytext, sizeof(keytext));

  value_set_mkv(parser, 0, 0, &vars, 0);

  for(x = 0; x < valset->els->table_size; x++)
  {
    if((v = set_table_get(valset->els, x)) == 0) {
      continue;
    }

    RUNTIME_ASSERT(v->type == VALUE_TYPE_MAPPING);

    narrator_xseek(Nkey, 0, NARRATOR_SEEK_SET, 0);
    get_mapping_key(Nkey, v->key);

    // as a side-effect, returns the name
    if(!variant_key_compatible(var, keytext, &name, &name_len))
      continue;

    /* SICK HAXX */
    realkey = v->key;
    v->key = &key;
    ps.s = (void*)name;
    ps.size = name_len;

    // merge this mapping in
    value_merge(parser, &vars, v, 0);

    v->key = realkey;
  }

  *varsp = vars;
}

void var_collate_stats(void *dst, size_t sz, var *vp, bool reset, size_t *zp)
{
  size_t z;
  fab_var_stats *stats;
  fab_var_stats lstats;
  descriptor_field *field;
  int x;

  stats = &vp->stats;
  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_var_stats.id);

  /* event counters */
  for(x = 0; x < descriptor_fab_var_stats.members_len; x++)
  {
    field = descriptor_fab_var_stats.members[x];

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
}
