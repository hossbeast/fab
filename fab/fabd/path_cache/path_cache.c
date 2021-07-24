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

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "valyria/list.h"
#include "valyria/set.h"

#include "path_cache.h"
#include "config.internal.h"
#include "yyutil/box.h"

#include "common/hash.h"
#include "zbuffer.h"
#include "macros.h"

static list * path_dirs;            // effective
static list * staging_path_dirs[2];
static set * path_search_cache;
static char env_path_buf[2048];
char *path_cache_env_path;

//
// static
//

static void path_entry_free(void * arg)
{
  path_cache_entry *pe = arg;

  if(pe)
  {
    close(pe->fd);
  }

  wfree(pe);
}

static uint32_t path_entry_hash(uint32_t h, const void * restrict _val, size_t sz)
{
  path_cache_entry *pe = (void*)_val;
  return hash32(h, pe->s, pe->len);
}

static int path_entry_cmp(const void * _A, size_t Az, const void *_B, size_t Bz)
{
  path_cache_entry *sa = (void*)_A;
  path_cache_entry *sb = (void*)_B;

  return memncmp(sa->s, sa->len, sb->s, sb->len);
}

static char * render_env_path(void)
{
  path_cache_entry *pe;
  size_t z;
  size_t l;
  char *s;
  int x;

  s = env_path_buf;
  l = sizeof(env_path_buf);
  z = 0;

  z += znloads(s + z, l - z, "PATH=");
  for(x = 0; x < path_dirs->size; x++)
  {
    pe = list_get(path_dirs, x);
    if(x) {
      z += znloadc(s + z, l - z, ':');
    }
    z += znloadw(s + z, l - z, pe->s, pe->len);
  }
  s[z] = 0;

  return env_path_buf;
}

static void path_entry_alloc(path_cache_entry ** pep, const char *s, uint16_t l, int fd, path_cache_entry *dir)
{
  path_cache_entry *pe;

  xmalloc(&pe, sizeof(*pe) + l + 1);
  memcpy(pe->s, s, l);
  pe->len = l;
  pe->fd = fd;
  pe->dir = dir;

  if((pe->filename = strrchr(pe->s, '/')))
    pe->filename++;
  else
    pe->filename = pe->s;

  *pep = pe;
}

static uint32_t cache_key_hash(uint32_t h, const void * key, size_t sz)
{
  return hash32(h, key, sz);
}

static int cache_key_cmp(const void * A, size_t Asz, const void * key, size_t sz)
{
  const path_cache_entry *pe = A;
  return memncmp(pe->s, pe->len, key, sz);
}

//
// public
//

void path_cache_setup()
{
  list_createx(&staging_path_dirs[0], 0, 0, path_entry_free);
  list_createx(&staging_path_dirs[1], 0, 0, path_entry_free);
  set_createx(&path_search_cache, 0, path_entry_hash, path_entry_cmp, path_entry_free);
}

void path_cache_cleanup()
{
  list_xfree(staging_path_dirs[0]);
  list_xfree(staging_path_dirs[1]);
  set_xfree(path_search_cache);
}

int path_cache_reconfigure(configblob * restrict cfg, char * restrict err, uint16_t err_sz)
{
  int x;
  char *pathenv;
  char *s;
  char *e = 0;
  box_string *ent;
  list * staging;
  int fd = -1;
  char T;
  path_cache_entry *pe = 0;
  uint16_t z;

  if(err)
  {
    staging = staging_path_dirs[0];
    if(path_dirs == staging) {
      staging = staging_path_dirs[1];
    }

    list_recycle(staging);

    /* paths from config have precedence over paths from $PATH */
    for(x = 0; x < cfg->formula.path.dirs.entries->table_size; x++)
    {
      if(!(ent = set_table_get(cfg->formula.path.dirs.entries, x)))
        continue;

      /* open an fd on the directory */
      fd = uxopenats(O_PATH | O_DIRECTORY, 0, ent->v);

      if(fd == -1)
      {
        z = 0;
        z += znloadf(err + z, err_sz - z, "path %.*s", (int)ent->l, ent->v);
        z += znloadf(err + z, err_sz - z, " location [%d,%d - %d,%d]"
          , ent->bx.loc.f_lin + 1
          , ent->bx.loc.f_col + 1
          , ent->bx.loc.l_lin + 1
          , ent->bx.loc.l_col + 1
        );
        return z;
      }

      path_entry_alloc(&pe, ent->v, ent->l, fd, 0);
      fd = -1;

      list_push(staging, pe, 0);
      pe = 0;
    }

    if(cfg->formula.path.copy_from_env && cfg->formula.path.copy_from_env->v && (pathenv = getenv("PATH")))
    {
      s = pathenv;
      while(1)
      {
        e = strchr(s, ':') ?: strchr(s, 0);

        T = *e;
        *e = 0;
        fd = uxopenats(O_PATH | O_DIRECTORY, 0, s);
        *e = T;

        if(fd == -1)
        {
          z = 0;
          z += znloadf(err + z, err_sz - z, "path %.*s", (int)(e - s), s);
          return z;
        }

        path_entry_alloc(&pe, s, e - s, fd, 0);
        fd = -1;

        list_push(staging, pe, 0);
        pe = 0;

        s = e;
        if(!*s)
          break;
        s++;
      }
    }
  }
  else if(cfg->formula.path.changed)
  {
    if(path_dirs)
      list_recycle(path_dirs);
    if(path_dirs == staging_path_dirs[0])
      path_dirs = staging_path_dirs[1];
    else
      path_dirs = staging_path_dirs[0];

    path_cache_env_path = 0;
    if(path_dirs->size)
    {
      path_cache_env_path = render_env_path();
    }
  }

  xclose(fd);
  path_entry_free(pe);

  return 0;
}

void path_cache_search(const path_cache_entry ** restrict pep, const char * restrict file, uint16_t file_len)
{
  int x;
  path_cache_entry *pe = 0;
  int fd = -1;

  if((pe = set_search(path_search_cache, (void*)file, file_len, cache_key_hash, cache_key_cmp)))
  {
    *pep = pe;
    pe = 0;
    goto end;
  }

  for(x = 0; x < path_dirs->size; x++)
  {
    pe = list_get(path_dirs, x);
    fd = uxopenats(O_RDONLY, pe->fd, file);
    if(fd != -1) {
      break;
    }
  }

  /* cache the result, positive or negative */
  path_entry_alloc(&pe, file, file_len, fd, pe);
  fd = -1;

  set_put(path_search_cache, pe, 0);

  *pep = pe;
  pe = 0;

end:
  xclose(fd);
  path_entry_free(pe);
}

void path_cache_reset()
{
  set_recycle(path_search_cache);
}
