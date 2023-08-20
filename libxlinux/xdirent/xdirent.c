/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "macros.h"

#include "xdirent.h"
#include "xfcntl.h"
#include "xunistd.h"


DIR * API xopendir(const char * name)
{
  DIR *dd;

  dd = opendir(name);
  RUNTIME_ASSERT(dd);

  return dd;
}

DIR * API uxopendir(const char * name)
{
  DIR *dd;

  if((dd = opendir(name)) == 0 && errno != ENOENT)
  {
    RUNTIME_ABORT();
  }

  return dd;
}

DIR * API xfdopendir(int fd)
{
  DIR *dd;

  if((dd = fdopendir(fd)) == 0)
  {
    RUNTIME_ABORT();
  }

  return dd;
}

DIR * API xopendirat(int dirfd, const char * const restrict path)
{
  DIR *dd;
  int fd;

  fd = xopenats(O_RDONLY | O_DIRECTORY, dirfd, path);
  dd = xfdopendir(fd);

  return dd;
}

struct dirent * API xreaddir(DIR * dd)
{
  struct dirent *result;

  errno = 0;
  if((result = readdir(dd)) == 0 && errno != 0)
  {
    RUNTIME_ABORT();
  }

  return result;
}

void API xclosedir(DIR * dd)
{
  if(dd == 0) {
    return;
  }

  RUNTIME_ASSERT(closedir(dd) == 0);
}

void API ixclosedir(DIR ** dd)
{
  xclosedir(*dd);
  *dd = 0;
}

long API xtelldir(DIR * dd)
{
  long loc;

  loc = telldir(dd);
  RUNTIME_ASSERT(loc != -1);

  return loc;
}
