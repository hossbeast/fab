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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "lorien/path_normalize.h"
#include "value.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"

#include "config.h"
#include "config_cursor.h"
#include "extern.h"
#include "node.h"
#include "reconfigure.h"
#include "walker.h"

static uint64_t extern_config_hash[2];

xapi extern_reconfigure(struct reconfigure_context * restrict ctx, const value * restrict config, uint32_t dry)
{
  enter;

  char space[512];

  value * list;
  value ** val;
  config_cursor cursor = {};

  if(!dry && !ctx->extern_changed)
    goto XAPI_FINALIZE;

  if(dry)
    extern_config_hash[1] = 0;

  fatal(config_cursor_init, &cursor);
  fatal(config_cursor_sets, &cursor, "extern");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_SET & dry, &list);
  if(list)
  {
    fatal(config_cursor_mark, &cursor);

    int x;
    for(x = 0; x < list->els->table_size; x++)
    {
      if(!(val = hashtable_table_entry(list->els, x)))
        continue;

      if(dry)
      {
        extern_config_hash[1] = value_hash(extern_config_hash[1], *val);
      }
      else
      {
        path_normalize(space, sizeof(space), (*val)->s->s);

        node * base;
        fatal(node_graft, space, &base);
        fatal(walker_walk, 0, base, 0, space, 0);
      }
    }
  }

  // no extern config
  else
  {
    extern_config_hash[1] = 0xdeadbeef;
  }

  if(dry)
  {
    ctx->extern_changed = extern_config_hash[0] != extern_config_hash[1];
  }
  else
  {
    extern_config_hash[0] = extern_config_hash[1];
  }

finally:
  config_cursor_destroy(&cursor);
coda;
}

xapi extern_setup()
{
  enter;

  finally : coda;
}

xapi extern_cleanup()
{
  enter;

  memset(extern_config_hash, 0, sizeof(extern_config_hash));

  finally : coda;
}
