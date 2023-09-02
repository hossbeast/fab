/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <fcntl.h>
#include <ftw.h>

#include "xlinux/xunistd.h"

#include "rmdirp.h"
#include "nftwat.h"

#include "macros.h"

static void fn(int method, ftwinfo * restrict info, void *arg, int * stop)
{
  int *rmself = arg;

  if(method == FTWAT_PRE) {
    return;
  }

  switch(info->type)
  {
    case FTWAT_F:
    case FTWAT_LNK:
      xunlinks(info->path);
      break;
    case FTWAT_D:
      if(info->parent != 0 || *rmself)
      {
        xrmdir(info->path);
      }
      break;
    RTA;
  }
}

//
// api
//

void API rmdirp(const char * const dirpath, int rmself)
{
  nftwat(AT_FDCWD, dirpath, fn, 32, FTWAT_SYMLINKS, &rmself);
}
