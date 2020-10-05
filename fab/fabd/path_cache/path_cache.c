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
#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "valyria/list.h"
#include "valyria/set.h"

#include "path_cache.h"
#include "config.internal.h"
#include "CONFIG.errtab.h"
#include "yyutil/box.h"

#include "common/hash.h"
#include "zbuffer.h"

static list * path_dirs;            // effective
static list * staging_path_dirs[2];
static set * path_search_cache;
static char env_path_buf[2048];
char *path_cache_env_path;

//
// static
//

static xapi path_entry_xfree(void * arg)
{
  enter;

  path_cache_entry *pe = arg;

  if(pe)
  {
//if(pe->fd != -1) {
//  printf("3 close %d for %.*s (%s)\n", pe->fd, (int)pe->len, pe->s, pe->filename);
//}
    fatal(close, pe->fd);
  }

  wfree(pe);

  finally : coda;
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

static xapi path_entry_alloc(path_cache_entry ** pep, const char *s, uint16_t l, int fd, path_cache_entry *dir)
{
  enter;

  path_cache_entry *pe;

  fatal(xmalloc, &pe, sizeof(*pe) + l + 1);
  memcpy(pe->s, s, l);
  pe->len = l;
  pe->fd = fd;
  pe->dir = dir;

  if((pe->filename = strrchr(pe->s, '/')))
    pe->filename++;
  else
    pe->filename = pe->s;

//printf("opened %d -> %.*s (%s)\n", pe->fd, pe->len, pe->s, pe->filename);

  *pep = pe;

  finally : coda;
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

xapi path_cache_setup()
{
  enter;

  fatal(list_createx, &staging_path_dirs[0], 0, 0, 0, path_entry_xfree);
  fatal(list_createx, &staging_path_dirs[1], 0, 0, 0, path_entry_xfree);
  fatal(set_createx, &path_search_cache, 0, path_entry_hash, path_entry_cmp, 0, path_entry_xfree);

  finally : coda;
}

xapi path_cache_cleanup()
{
  enter;

  fatal(list_xfree, staging_path_dirs[0]);
  fatal(list_xfree, staging_path_dirs[1]);
  fatal(set_xfree, path_search_cache);

  finally : coda;
}

xapi path_cache_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  int x;
  char *pathenv;
  char *s;
  char *e = 0;
  box_string *ent;
  list * staging;
  int fd = -1;
  char T;
  path_cache_entry *pe = 0;

  if(dry)
  {
    staging = staging_path_dirs[0];
    if(path_dirs == staging) {
      staging = staging_path_dirs[1];
    }

    fatal(list_recycle, staging);

    /* paths from config have precedence over paths from $PATH */
    for(x = 0; x < cfg->formula.path.dirs.entries->table_size; x++)
    {
      if(!(ent = set_table_get(cfg->formula.path.dirs.entries, x)))
        continue;

      /* open an fd on the directory */
      fatal(uxopenats, &fd, O_PATH | O_DIRECTORY, 0, ent->v);

      if(fd == -1)
      {
        xapi_info_pushw("path", ent->v, ent->l);
        fatal(config_throw, &ent->bx);
      }

      fatal(path_entry_alloc, &pe, ent->v, ent->l, fd, 0);
      fd = -1;

      fatal(list_push, staging, pe, 0);
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
        fatal(uxopenats, &fd, O_PATH | O_DIRECTORY, 0, s);
        *e = T;

        if(fd == -1) {
          xapi_info_pushw("path", s, e - s);
          fail(CONFIG_INVALID);
        }

        fatal(path_entry_alloc, &pe, s, e - s, fd, 0);
        fd = -1;

        fatal(list_push, staging, pe, 0);
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
      fatal(list_recycle, path_dirs);
    if(path_dirs == staging_path_dirs[0])
      path_dirs = staging_path_dirs[1];
    else
      path_dirs = staging_path_dirs[0];

    path_cache_env_path = 0;
    if(path_dirs->size)
    {
      path_cache_env_path = render_env_path();
    }

//    printf("$PATH : %s\n", path_cache_env_path);
  }

finally:
if(fd != -1) {
//  printf("1 close %d\n", fd);
}
  fatal(xclose, fd);
  fatal(path_entry_xfree, pe);
coda;
}

xapi path_cache_search(const path_cache_entry ** restrict pep, const char * restrict file, uint16_t file_len)
{
  enter;

  int x;
  path_cache_entry *pe = 0;
  int fd = -1;

  if((pe = set_search(path_search_cache, (void*)file, file_len, cache_key_hash, cache_key_cmp)))
  {
//printf("GOT '%.*s'(%d) -> %d\n", (int)file_len, file, file_len, pe->fd);
    *pep = pe;
    pe = 0;
    goto XAPI_FINALLY;
  }

  for(x = 0; x < path_dirs->size; x++)
  {
    pe = list_get(path_dirs, x);
    fatal(uxopenats, &fd, O_RDONLY, pe->fd, file);
    if(fd != -1)
      break;
  }

  /* cache the result, positive or negative */
  fatal(path_entry_alloc, &pe, file, file_len, fd, pe);
  fd = -1;

  fatal(set_put, path_search_cache, pe, 0);
//printf("PUT '%.*s'(%d) -> %d\n", (int)file_len, file, file_len, pe->fd);

  *pep = pe;
  pe = 0;

finally:
//if(fd != -1) {
//  printf("2 close %d\n", fd);
//}
  fatal(xclose, fd);
  fatal(path_entry_xfree, pe);
coda;
}

xapi path_cache_reset()
{
  enter;

//printf("PATH CACHE RESET\n");
  fatal(set_recycle, path_search_cache);

  finally : coda;
}
