/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "common/hash.h"
#include "bacon/value.h"
#include "bacon/location.h"

#include "path_cache.h"
#include "configuration.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"

#include "zcat.h"
#include "macros.h"

static list * path_dirs;            // effective
static list * staging_path_dirs[2];
static hashtable path_search_cache;
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

  free(pe);
}

static uint32_t path_entry_hash(uint32_t h, const void * val)
{
  const path_cache_entry *pe = val;
  return hash32(h, pe->s, pe->len);
}

static int path_entry_cmp(const void * _A, const void *_B)
{
  const path_cache_entry *sa = _A;
  const path_cache_entry *sb = _B;

  return memncmp(sa->s, sa->len, sb->s, sb->len);
}

static char *render_env_path(void)
{
  path_cache_entry *pe;
  size_t z;
  size_t l;
  char *s;
  int x;

  s = env_path_buf;
  l = sizeof(env_path_buf);
  z = 0;

  z += zcats(s + z, l - z, "PATH=");
  for(x = 0; x < path_dirs->size; x++)
  {
    pe = list_get(path_dirs, x);
    if(x) {
      z += zcatc(s + z, l - z, ':');
    }
    z += zcatw(s + z, l - z, pe->s, pe->len);
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

struct cache_key {
  const char * file;
  uint16_t file_len;
};

static uint32_t cache_key_hash(uint32_t h, const void * K)
{
  const struct cache_key *key = K;
  return hash32(h, key->file, key->file_len);
}

static int cache_key_cmp(const void * A, const void * K)
{
  const path_cache_entry *pe = A;
  const struct cache_key *key = K;
  return memncmp(pe->s, pe->len, key->file, key->file_len);
}

//
// public
//

void path_cache_setup()
{
  list_createx(&staging_path_dirs[0], 0, path_entry_free);
  list_createx(&staging_path_dirs[1], 0, path_entry_free);
  hashtable_initx(&path_search_cache, 0, path_entry_hash, path_entry_cmp, path_entry_free);
}

void path_cache_cleanup()
{
  list_free(staging_path_dirs[0]);
  list_free(staging_path_dirs[1]);
  hashtable_destroy(&path_search_cache);
}

void path_cache_reconfigure(configuration * restrict cfg, bool dryrun)
{
  char *pathenv;
  char *s;
  char *e = 0;
  configuration_formula_path_dirs_entry *ent;
  list * staging;
  int fd = -1;
  char T;
  path_cache_entry *pe = 0;
  channel *chan;
  bamipc_message *msg;

  if(dryrun)
  {
    staging = staging_path_dirs[0];
    if(path_dirs == staging) {
      staging = staging_path_dirs[1];
    }

    list_recycle(staging);

    /* paths from config have precedence over paths from $PATH */
    rbtree_foreach(&cfg->formula.path.dirs.set, ent, rbn) {

      /* open an fd on the directory */
      fd = uxopenats(O_PATH | O_DIRECTORY, 0, ent->s);

      if(fd == -1)
      {
        msg = system_error_event(&chan);
        msg->code = ENOENT;
        msg->size = 0;
        msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, "path %.*s", (int)ent->len, ent->s);
        msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, " location [%d,%d - %d,%d]"
          , ent->v->loc->f_lin + 1
          , ent->v->loc->f_col + 1
          , ent->v->loc->l_lin + 1
          , ent->v->loc->l_col + 1
        );
        events_publish(chan, msg);

        return;
      }

      path_entry_alloc(&pe, ent->s, ent->len, fd, 0);
      fd = -1;

      list_push(staging, pe);
      pe = 0;
    }

    if(cfg->formula.path.copy_from_env.b && (pathenv = getenv("PATH")))
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
          msg = system_error_event(&chan);
          msg->code = ENOENT;
          msg->size = zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, "path %.*s", (int)(e - s), s);
          events_publish(chan, msg);
          return;
        }

        path_entry_alloc(&pe, s, e - s, fd, 0);
        fd = -1;

        list_push(staging, pe);
        pe = 0;

        s = e;
        if(!*s)
          break;
        s++;
      }
    }
  }
  else // if(cfg->formula.path.changed)
  {
    if(path_dirs)
      list_recycle(path_dirs);
    if(path_dirs == staging_path_dirs[0])
      path_dirs = staging_path_dirs[1];
    else
      path_dirs = staging_path_dirs[0];

    path_cache_env_path = 0;
    if(path_dirs->size) {
      path_cache_env_path = render_env_path();
    }
  }

  xclose(fd);
  path_entry_free(pe);

  return;
}

void path_cache_search(const path_cache_entry ** restrict pep, const char * restrict file, uint16_t file_len)
{
  int x;
  path_cache_entry *pe = 0;
  struct cache_key key;
  int fd = -1;
  hashtable_search_context ctx;

  key.file = file;
  key.file_len = file_len;
  ctx = (typeof(ctx)) {
    entry : &key,
    hash_fn : cache_key_hash,
    cmp_fn : cache_key_cmp
  };
  if(hashtable_search(&path_search_cache, &ctx))
  {
    *pep = hashtable_fetch(&path_search_cache, &ctx);
    goto end;
  }

  /* check against active paths */
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

  ctx.entry = pe;
  hashtable_set(&path_search_cache, &ctx);

  *pep = pe;
  pe = 0;

end:
  xclose(fd);
  path_entry_free(pe);
}

void path_cache_reset()
{
  hashtable_recycle(&path_search_cache);
}
