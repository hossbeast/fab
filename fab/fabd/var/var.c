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

#include "value.h"
#include "value/parser.h"
#include "value/merge.h"
#include "value/make.h"
#include "value/clone.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "valyria/pstring.h"
#include "valyria/map.h"
#include "narrator/fixed.h"
#include "valyria/set.h"

#include "var.h"
#include "node.h"
#include "params.h"
#include "logging.h"
#include "config.internal.h"
#include "variant.h"

#include "common/snarf.h"

static value_parser * parser;
value * g_var;

//
// public
//

xapi var_setup()
{
  enter;

  fatal(value_parser_create, &parser);

  finally : coda;
}

xapi var_cleanup()
{
  enter;

  fatal(value_parser_xfree, parser);

  finally : coda;
}

xapi var_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  if(dry)
  {

  }
  else if(cfg->var.attrs & CONFIG_CHANGED)
  {
    g_var = cfg->var.value;
  }

  finally : coda;
}

xapi var_node_parse(node * restrict var_node)
{
  enter;

  char * text = 0;
  size_t text_len;
  value * val;
  char path[512];

  if(!var_node->not_parsed)
    goto XAPI_FINALLY;

  node_kind_set(var_node, VERTEX_VAR_BAM);

  // absolute path, for exec
  node_get_project_relative_path(var_node, path, sizeof(path));

  fatal(snarfats, &text, &text_len, g_params.proj_dirfd, path);
  if(text)
  {
    fatal(value_parser_parse, parser, text, text_len, path, VALUE_TYPE_SET, &val);
  }

  var_node->self_var = val;

  logf(L_MODULE, "parsed var @ %s", path);
  var_node->not_parsed = 0;

finally:
  wfree(text);
coda;
}

/*
 * write an environment variable name
 *
 * N   narrator to write to
 * val name to render
 */
static xapi __attribute__((nonnull)) get_mapping_key(narrator * restrict N, value * restrict val)
{
  enter;

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

      fatal(narrator_xsayc, N, val->s->s[x]);
    }

    fatal(narrator_flush, N);
  }
  else
  {
    fatal(value_say, val, N);
  }

  finally : coda;
}

xapi var_denormalize(value_parser * restrict parser, variant * restrict var, value * restrict valset, value ** restrict varsp)
{
  enter;

  int x;
  narrator * Nkey = 0;
  char Nstor_key[NARRATOR_STATIC_SIZE];
  char keytext[128];
  const char *name;
  uint16_t name_len;
  value * v;
  value * vars;
  value * realkey;
  value key = { 0 };
  pstring ps = { 0 };

  key.type = VALUE_TYPE_STRING;
  key.s = &ps;

  Nkey = narrator_fixed_init(Nstor_key, keytext, sizeof(keytext));

  fatal(value_set_mkv, parser, 0, 0, &vars, 0);

  for(x = 0; x < valset->els->table_size; x++)
  {
    if((v = set_table_get(valset->els, x)) == 0)
      continue;

    RUNTIME_ASSERT(v->type == VALUE_TYPE_MAPPING);

    fatal(narrator_xseek, Nkey, 0, NARRATOR_SEEK_SET, 0);
    fatal(get_mapping_key, Nkey, v->key);

    // as a side-effect, returns the name
    if(!variant_key_compatible(var, keytext, &name, &name_len))
      continue;

    /* SICK HAXX */
    realkey = v->key;
    v->key = &key;
    ps.s = (void*)name;
    ps.size = name_len;

    // merge this mapping in
    fatal(value_merge, parser, &vars, v, 0);

    v->key = realkey;
  }

  *varsp = vars;

  finally : coda;
}
