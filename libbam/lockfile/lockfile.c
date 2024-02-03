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

/* ugo+rw world read/write */
#define LOCKFILE_MODE 0666

//
// static
//

int64_t lockfile_read(int fd);
int64_t lockfile_read(int fd)
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

DIR *bamipc_lockfile_scan_start(const char * restrict ipcdir_base)
{
  return xopendirat(AT_FDCWD, ipcdir_base);
}

void bamipc_lockfile_scan_stop(DIR * restrict dd)
{
  xclosedir(dd);
}

int64_t bamipc_lockfile_scan_scan(DIR * restrict dd, uint64_t * restrict hash, char *path, size_t path_sz, size_t *path_len)
{
  struct dirent *entp;
  int dfd;
  int fd;
  int64_t pid;

  dfd = dirfd(dd);

  while((entp = xreaddir(dd))) {
    if(strcmp(entp->d_name, ".") == 0 || strcmp(entp->d_name, "..") == 0) {
      continue;
    }

    if(parseuint(entp->d_name, SCNx64, 0, UINT64_MAX, 16, 16, hash, 0) != 0) {
      continue; /* malformed directory name ? */
    }

    *path_len = uxreadlinkatf(dfd, "%016"PRIx64"/basedir", path, path_sz, *hash);
    if(*path_len < 0) {
      continue; /* no basedir ? */
    }

    if((fd = uxopenatf(O_RDONLY, dfd, "%016"PRIx64"/bamd/lock", *hash)) == -1) {
      continue;
    }

    pid = lockfile_read(fd);
    close(fd);
    return pid;
  }

  return 0;
}
