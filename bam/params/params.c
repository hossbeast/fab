/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <fcntl.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "common/sha1.h"
#include "common/bech32.h"
#include "lorien/path_normalize.h"

#include "info.h"
#include "params.h"
#include "zcat.h"

struct g_params g_params;

//
// public
//

size_t module_compute_id(char *dst, size_t dst_size, const char * restrict path, uint16_t path_len)
{
  sha1_ctx sha;
  uint8_t hash[20];

  sha1_init(&sha);
  sha1_update(&sha, MMS("module "));
  sha1_update(&sha, path, path_len);
  sha1_final(hash, &sha);

  return bech32_encode(dst, dst_size, MMS("bm"), hash, 12);
}

void params_setup()
{
  char path[PATH_MAX];
  const char *val;
  path_normalize_context ctx;

  g_params.pid = getpid();

  /* current working directory */
  if(getcwd(path, sizeof(path)) == 0) {
    dprintf(2, "getcwd failed %s\n", strerror(errno));
    exit(1);
  }
  if((val = getenv("PWD"))) {
    ctx.base = path;
    ctx.base_len = strlen(path);
    g_params.cwd_len = path_normalize(&ctx, g_params.cwd, sizeof(g_params.cwd), val, strlen(val));
  } else {
    g_params.cwd_len = zcats(g_params.cwd, sizeof(g_params.cwd), path);
  }

  /* module id from cwd */
  g_params.cwd_module_id_len = module_compute_id(
      g_params.cwd_module_id
    , sizeof(g_params.cwd_module_id)
    , g_params.cwd
    , g_params.cwd_len
  );

  // ipc directory
  if((val = getenv("BAM_IPC_DIR"))) {
    g_params.ipcdir_len = zcats(g_params.ipcdir, sizeof(g_params.ipcdir), val);
  } else if((val = getenv("XDG_RUNTIME_DIR"))) {
    g_params.ipcdir_len = zcatf(g_params.ipcdir, sizeof(g_params.ipcdir), "%s/bam", val);
  } else {
    dprintf(2, "neither $BAM_IPC_DIR nor $XDG_RUNTIME_DIR is set!\n");
    exit(1);
  }

#if CONFIG_DEVEL
  ssize_t r;

  if((val = getenv("BM_BAMD_PATH"))) {
    zcats(g_params.bamd_path, sizeof(g_params.bamd_path), val);
    if(faccessat(INT32_MAX, g_params.bamd_path, F_OK, AT_EACCESS) == 0) {
      return;
    }
  }

  /* check for the expected directory layout for bam devel builds and locate
   * the bamd executable */
  r = xreadlinks("/proc/self/exe", path, sizeof(path));
  while(r > 0 && path[r] != '/') {
    r--;
  }
  if(r > 21) {
    if(memncmp(path + r - 20, 21, MMS("/bam-build/bam/devel/")) == 0) {
      r = zcatw(g_params.bamd_path, sizeof(g_params.bamd_path), path, r - 20);
      zcats(g_params.bamd_path + r, sizeof(g_params.bamd_path) - r, "/bam-build/bamd/devel/bamd");
      if(faccessat(INT32_MAX, g_params.bamd_path, F_OK, AT_EACCESS) == 0) {
        return;
      }
    }
  }

  dprintf(2, "bamd executable not found (set $BM_BAMD_PATH ?)\n");
  exit(1);
#endif
}

void params_teardown()
{
}
