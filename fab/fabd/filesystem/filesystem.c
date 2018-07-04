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
#include "valyria/strutil.h"
#include "valyria/pstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "lorien/path_normalize.h"

#include "filesystem.internal.h"
#include "config.internal.h"
#include "reconfigure.h"
#include "config_cursor.h"
#include "CONFIG.errtab.h"

#include "zbuffer.h"
#include "cksum.h"

#define restrict __restrict

map * filesystems;
static uint64_t filesystems_config_hash[2];

// sorted by name
static struct {
  const char * name;
  int opt;
} invalidate_opts[] = {
#define FILESYSTEM(a, b, c) { opt : (b), name : c },
FILESYSTEM_TABLE
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
  fatal(strloads, &(*fs)->path, path);
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

  fatal(map_createx, &filesystems, 0, filesystem_free, 0);
  qsort(invalidate_opts, sizeof(invalidate_opts) / sizeof(*invalidate_opts), sizeof(*invalidate_opts), invalidate_opts_compare);

  char * s;
  size_t sz;
  size_t z;

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
  memset(filesystems_config_hash, 0, sizeof(filesystems_config_hash));

  finally : coda;
}

filesystem * filesystem_lookup(const char * const restrict path, size_t pathl)
{
  /* path is a normalized absolute path */
  const char * end = path + strlen(path);

  filesystem * fs = 0;
  while(end != path && !fs)
  {
    fs = map_get(filesystems, path, end - path);

    end--;
    while(end != path && *end != '/')
      end--;
  }

  if(!fs)
    fs = map_get(filesystems, "/", 1);

  return fs;
}

static int keys_compare(const void * A, const void * B)
{
  return strcmp(*(char **)A, *(char **)B);
}

xapi filesystem_reconfigure(struct reconfigure_context * restrict ctx, const value * restrict config, uint32_t dry)
{
  enter;

  char space[512];

  value * map;
  value * val;
  value * key;
  filesystem * fs = 0;
  config_cursor cursor = {};
  int opt = 0;
  const char ** keys = 0;
  size_t keysl = 0;
  int has_root = 0;
  int x;

  if(!dry && !ctx->filesystems_changed)
    goto XAPI_FINALIZE;

  if(!dry)
    fatal(map_recycle, filesystems);

  if(dry)
    filesystems_config_hash[1] = 0;

  fatal(config_cursor_init, &cursor);
  fatal(config_cursor_sets, &cursor, "filesystem");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_MAP & dry, &map);
  if(map)
  {
    fatal(config_cursor_mark, &cursor);

    for(x = 0; x < map->keys->l; x++)
    {
      key = list_get(map->keys, x);

      // hash the config value for this filesystem
      if(dry)
      {
        filesystems_config_hash[1] += value_hash(key);
        filesystems_config_hash[1] += value_hash(list_get(map->vals, x));
      }

      // query the relevant properties
      fatal(config_cursor_pushf, &cursor, "%s.invalidate", key->s->s);
      fatal(config_query, map, config_cursor_path(&cursor), config_cursor_query(&cursor), (VALUE_TYPE_STRING | CONFIG_QUERY_NOTNULL) & dry, &val);

      if(val)
        opt = invalidate_opts_lookup(val->s->s);

      if(dry && !opt)
      {
        xapi_info_pushs("expected", invalidate_opts_list);
        xapi_info_pushs("actual", val->s->s);
        fatal(config_throw, CONFIG_INVALID, val, config_cursor_path(&cursor));
      }

      if(!dry)
      {
        path_normalize(space, sizeof(space), key->s->s);
        fatal(filesystem_create, space, opt, &fs);
        fatal(map_set, filesystems, MMS(fs->path), fs);
        fs = 0;
        if(strcmp(space, "/") == 0)
          has_root = 1;
      }
    }
  }

  // the absence of filesystem config
  else
  {
    filesystems_config_hash[1] = 0xdeadbeef;
  }

  if(dry)
    ctx->filesystems_changed = filesystems_config_hash[0] != filesystems_config_hash[1];

  else
  {
    // synthetic root
    if(!has_root)
    {
      fatal(filesystem_create, "/", FILESYSTEM_INVALIDATE_NOTIFY, &fs);
      fatal(map_set, filesystems, MMS(fs->path), fs);
      fs = 0;
    }

    // determine which filesystems are leaves
    fatal(map_keys, filesystems, &keys, &keysl);
    qsort(keys, keysl, sizeof(*keys), keys_compare);

    for(x = 0; x < keysl; x++)
    {
      filesystem * fs = map_get(filesystems, MMS(keys[x]));
      fs->leaf = 1;

      size_t len = strlen(keys[x]);

      int y;
      for(y = x + 1; y < keysl; y++)
      {
        if(strcmp(keys[x], keys[y]) > 0)
          break;

        if(len == 1 || keys[y][len] == '/')
        {
          fs->leaf = 0;
          break;
        }
      }
    }

    // the circle is now complete
    filesystems_config_hash[0] = filesystems_config_hash[1];
  }

finally:
  filesystem_free(fs);
  config_cursor_destroy(&cursor);
  wfree(keys);
coda;
}
