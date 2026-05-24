/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xdirent.h"
#include "xfcntl.h"

#include "types.h"

DIR * xopendir(const char * name)
{
  DIR *dd;

  dd = opendir(name);
  RUNTIME_ASSERT(dd);

  return dd;
}

DIR * uxopendir(const char * name)
{
  DIR *dd;

  if((dd = opendir(name)) == 0 && errno != ENOENT)
  {
    RTA;
  }

  return dd;
}

DIR * xfdopendir(int fd)
{
  DIR *dd;

  if((dd = fdopendir(fd)) == 0)
  {
    RTA;
  }

  return dd;
}

DIR * xopendirat(int dirfd, const char * const restrict path)
{
  DIR *dd;
  int fd;

  fd = xopenats(O_RDONLY | O_DIRECTORY, dirfd, path);
  dd = xfdopendir(fd);

  return dd;
}

struct dirent * xreaddir(DIR * dd)
{
  struct dirent *result;

  errno = 0;
  if((result = readdir(dd)) == 0 && errno != 0)
  {
    RTA;
  }

  return result;
}

void xclosedir(DIR * dd)
{
  if(dd == 0) {
    return;
  }

  RUNTIME_ASSERT(closedir(dd) == 0);
}

void ixclosedir(DIR ** dd)
{
  xclosedir(*dd);
  *dd = 0;
}

long xtelldir(DIR * dd)
{
  long loc;

  loc = telldir(dd);
  RUNTIME_ASSERT(loc != -1);

  return loc;
}
