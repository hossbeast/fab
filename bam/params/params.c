/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>
#include <inttypes.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "common/hash.h"

#include "info.h"
#include "params.h"
#include "zcat.h"

struct g_params g_params;

//
// public
//

size_t params_path_hash(char *dst, size_t dst_size, const char * restrict path, uint16_t path_len)
{
  char inpath[PATH_MAX];
  char abspath[PATH_MAX];
  uint16_t abspath_len;
  uint64_t hash;

  zcatw(inpath, sizeof(inpath), path, path_len);
  inpath[path_len] = 0;
  if((realpath(inpath, abspath)) == 0) {
    dprintf(2, "realpath(%s) : %s\n", inpath, strerror(errno));
    exit(1);
  }
  abspath_len = strlen(abspath);

  hash = hash64(0, abspath, abspath_len);

  return zcatf(dst, dst_size, "%016"PRIx64, hash);
}

void params_setup()
{
  const char *ipc;

  g_params.pid = getpid();

  // current working directory
  xgetcwd(g_params.cwd, sizeof(g_params.cwd));
  g_params.cwd_len = strlen(g_params.cwd);
  g_params.cwd_hash_len = params_path_hash(g_params.cwd_hash, sizeof(g_params.cwd_hash), g_params.cwd, g_params.cwd_len);

  // ipc directory
  if((ipc = getenv("BAM_IPC_DIR"))) {
    g_params.ipcdir_len = zcats(g_params.ipcdir, sizeof(g_params.ipcdir), ipc);
  } else if((ipc = getenv("XDG_RUNTIME_DIR"))) {
    g_params.ipcdir_len = zcatf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/bam", ipc);
  } else {
    dprintf(2, "neither $BAM_IPC_DIR nor $XDG_RUNTIME_DIR is set!\n");
    exit(1);
  }

#if CONFIG_DEVEL
  char space[PATH_MAX];
  ssize_t r;

  // exedir is the canonical path to directory containing the executing binary
  r = xreadlinks("/proc/self/exe", space, sizeof(space));
  r--;
  while(space[r] != '/') {
    r--;
  }

  ixstrndup(&g_params.exedir, space, r);
#endif
}

void params_teardown()
{
#if CONFIG_DEVEL
  ifree(&g_params.exedir);
#endif
}
