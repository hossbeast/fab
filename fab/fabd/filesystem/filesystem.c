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

#include <stdlib.h>
#include <string.h>

#include "xapi.h"

#include "value.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/pstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "filesystem.h"
#include "config.h"
#include "config_cursor.h"
#include "errtab/CONFIG.errtab.h"

#include "zbuffer.h"

#define restrict __restrict

static map * filesystems;

// sorted by name
static struct {
  const char * name;
  int opt;
} invalidate_opts[] = {
#define FILESYSTEM(a, b, c, x, y) { opt : (b), name : QUOTE(c) },
FILESYSTEM_TABLE(0, 0)
#undef FILESYSTEM
};

static char invalidate_opts_list[64];

static int invalidate_opts_compare(const void * A, const void * B)
{
  return strcmp(((typeof(invalidate_opts[0])*)A)->name, ((typeof(invalidate_opts[0])*)B)->name);
}

static int invalidate_opts_lookup(const char * name)
{
  typeof(*invalidate_opts) key = { .name = name };
  typeof(invalidate_opts[0])* opt = bsearch(&key, invalidate_opts, sizeof(invalidate_opts) / sizeof(*invalidate_opts), sizeof(*invalidate_opts), invalidate_opts_compare);
  if(opt)
    return opt->opt;

  return 0;
}

//
// static
//

static xapi filesystem_create(const char * const restrict path, uint32_t attrs, filesystem ** const restrict fs)
{
  enter;

  fatal(xmalloc, fs, sizeof(**fs));
  fatal(ixstrdup, &(*fs)->path, path);
  (*fs)->attrs = attrs;

  finally : coda;
}

static void filesystem_free(filesystem * const restrict fs)
{
  if(fs)
  {
    wfree(fs->path);
  }

  wfree(fs);
}

//
// public
//

xapi filesystem_setup()
{
  enter;

  char * s;
  size_t sz;
  size_t z;

  fatal(map_createx, &filesystems, filesystem_free, 0, 0);
  qsort(invalidate_opts, sizeof(invalidate_opts) / sizeof(*invalidate_opts), sizeof(*invalidate_opts), invalidate_opts_compare);

  s = invalidate_opts_list;
  sz = sizeof(invalidate_opts_list);
  z = 0;

  int x;
  for(x = 0; x < sizeof(invalidate_opts) / sizeof(*invalidate_opts); x++)
  {
    if(x)
      z += znloads(s + z, sz - z, ", ");
    z += znloads(s + z, sz - z, invalidate_opts[x].name);
  }

  finally : coda;
}

xapi filesystem_cleanup()
{
  enter;

  fatal(map_xfree, filesystems);

  finally : coda;
}

xapi filesystem_lookup(char * const restrict path, filesystem ** const restrict rv)
{
  enter;

  const char * end = path + strlen(path);

  filesystem * fs = 0;
  while(end != path && !fs)
  {
    fs = map_get(filesystems, path, end - path);

    end--;
    while(end != path && *end != '/')
      end--;
  }

  *rv = fs;

  finally : coda;
}

xapi filesystem_reconfigure(const value * restrict config, uint32_t dry)
{
  enter;

  value * map;
  value * val;
  value * key;
  filesystem * fs = 0;
  config_cursor cursor = { 0 };
  int opt = 0;

  fatal(config_cursor_init, &cursor);

  if(!dry)
    fatal(map_recycle, filesystems);

  fatal(config_cursor_sets, &cursor, "filesystems");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_MAP & dry, &map);
  if(map)
  {
    fatal(config_cursor_mark, &cursor);

    int x;
    for(x = 0; x < map->keys->l; x++)
    {
      key = list_get(map->keys, x);
      fatal(config_cursor_pushf, &cursor, "%s.invalidate", key->s->s);
      fatal(config_query, map, config_cursor_path(&cursor), config_cursor_query(&cursor), (VALUE_TYPE_STRING | CONFIG_QUERY_NOTNULL) & dry, &val);

      if(val)
        opt = invalidate_opts_lookup(val->s->s);

      if(dry && !opt)
      {
        xapi_fail_intent();
        xapi_info_adds("expected", "{stat, content, notify, always, never}");
        xapi_info_adds("actual", val->s->s);
        fatal(config_throw, CONFIG_INVALID, val, config_cursor_path(&cursor));
      }

      if(!dry)
      {
        fatal(filesystem_create, key->s->s, opt, &fs);
        fatal(map_set, filesystems, key->s->s, key->s->l, fs);
        fs = 0;
      }
    }
  }

finally:
  filesystem_free(fs);
  config_cursor_destroy(&cursor);
coda;
}
