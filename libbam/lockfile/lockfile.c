/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>
#include <inttypes.h>
#include <dirent.h>
#include <string.h>

#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "xlinux/xsignal.h"
#include "xlinux/xdirent.h"
#include "xlinux/xstdlib.h"

#include "lockfile.h"

#include "common/fmt.h"
#include "common/parseint.h"

#include "zcat.h"

/* ugo+rw world read/write */
#define LOCKFILE_MODE 0666

//
// static
//

static bool ishex(const char * restrict s, size_t len)
{
  int x;
  for(x = 0; x < len; x++) {
    if(s[x] >= 'a' && s[x] <= 'f') {
      continue;
    }
    if(s[x] >= 'A' && s[x] <= 'F') {
      continue;
    }
    if(s[x] >= '0' && s[x] <= '9') {
      continue;
    }
    return false;
  }
  return true;
}

static int64_t lockfile_read(int fd)
{
  char buf[64];
  size_t len;
  int64_t i64;
  int r;

  len = 0;
  while(len < 32) {
    r = read(fd, buf + len, 32);
    if(r <= 0) {
      break;
    }
    len += r;
  }

  if(len == 0) {
    return -1;
  }

  buf[len] = 0;
  if(parseint(buf, SCNd64, 1, INT64_MAX, len - 1 /* permits trailing newline */, len, &i64, 0) == 0) {
    return i64;
  }
  return -1;
}

void bamipc_lockfile_acquire(int64_t * restrict pid, int * restrict fd, const char * const restrict path)
{
  int lfd;

  *pid = -1;
  *fd = uxopen_modes(O_CREAT | O_WRONLY | O_EXCL, LOCKFILE_MODE, path);
  if(*fd != -1) {
    return;
  }
  if((lfd = uxopens(O_RDONLY, path)) != -1) {
    *pid = lockfile_read(lfd);
    close(lfd);
  }
}

void bamipc_lockfile_validate(int64_t * restrict pid, int * restrict fd, const char * const restrict path)
{
  if(*fd == -1 && *pid == -1) {
    /* bad lockfile */
    uxunlinks(path);
  } else if(*pid != -1 && uxkill(*pid, 0) != 0) {
    /* stale lockfile */
    uxunlinks(path);
  } else {
    return;
  }

  /* reload */
  ixclose(fd);
  *pid = -1;
  bamipc_lockfile_acquire(pid, fd, path);
}

void bamipc_lockfile_update(int fd)
{
  char buf[32];
  int len;

  len = snprintf(buf, sizeof(buf), "%"PRId64"\n", (int64_t)getpid());
  xlseek(fd, 0, SEEK_SET);
  axwrite(fd, buf, len);
  (void)!!ftruncate(fd, len);
}

int64_t bamipc_lockfile_scan_scan(
    DIR * restrict dd
  , char * restrict bamd_id
  , size_t bamd_id_size
  , uint16_t * restrict bamd_id_lenp
  , char *path
  , size_t path_sz
  , size_t *path_len
)
{
  struct dirent *entp;
  int dfd;
  int fd;
  int64_t pid;
  ssize_t rv;

  dfd = dirfd(dd);

  while((entp = xreaddir(dd))) {
    if(strcmp(entp->d_name, ".") == 0 || strcmp(entp->d_name, "..") == 0) {
      continue;
    }

    if(!ishex(entp->d_name, strlen(entp->d_name))) {
      continue; /* malformed directory name ? */
    }

    *bamd_id_lenp = zcats(bamd_id, bamd_id_size, entp->d_name);
    if((rv = uxreadlinkatf(dfd, "%.*s/basedir_real", path, path_sz, (int)*bamd_id_lenp, bamd_id)) < 0) {
      continue; /* no basedir ? */
    }
    *path_len = rv;

    if((fd = uxopenatf(O_RDONLY, dfd, "%.*s/bamd/lock", (int)*bamd_id_lenp, bamd_id)) == -1) {
      continue;
    }

    pid = lockfile_read(fd);
    close(fd);
    return pid;
  }

  return 0;
}

DIR *bamipc_lockfile_scan_start(const char * restrict ipcdir_root)
{
  return xopendirat(AT_FDCWD, ipcdir_root);
}

void bamipc_lockfile_scan_stop(DIR * restrict dd)
{
  xclosedir(dd);
}
