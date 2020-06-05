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

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "xapi.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstat.h"
#include "xlinux/xunistd.h"
#include "xlinux/xdirent.h"
#include "xlinux/xstdlib.h"

#include "nftwat.h"

#include "zbuffer.h"

static xapi __attribute__((nonnull(1, 3, 5, 7, 9))) walk(
    xapi (*fn)(int method, ftwinfo * info, void * arg, int * stop)
  , void * arg
  , DIR *** const restrict dds
  , size_t ddsz
  , size_t * ddsp
  , int dirfd
  , char * const restrict path
  , size_t pathz
  , size_t * const restrict pathl
  , ftwinfo * dirinfo
)
{
  enter;

  DIR * dd = 0;
  size_t patho;
  long dirloc;
  int ddso = -1;
  int stop = 0;
  struct stat stb;
  uint8_t type;

  patho = *pathl;

  // pre-order callback for directories
  fatal(fn, FTWAT_PRE, dirinfo, arg, &stop);

  if(stop)
    goto XAPI_FINALIZE;

  // if theres no room, close the descriptor in an earlier activation/frame
  if(dds[*ddsp])
    fatal(ixclosedir, dds[*ddsp]);

  fatal(xopendirat, &dd, dirfd, path);
  ddso = *ddsp;
  *ddsp = ((*ddsp) + 1) % ddsz;
  dds[ddso] = &dd;

  ftwinfo child = {};
  child.parent = dirinfo;
  child.path = path;

  struct dirent * entp;
  fatal(xreaddir, dd, &entp);
  fatal(xtelldir, &dirloc, dd);
  do
  {
    if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
    {
      *pathl += znloads(path + *pathl, pathz - *pathl, "/");
      child.name_off = *pathl;
      *pathl += znloads(path + *pathl, pathz - *pathl, entp->d_name);
      path[*pathl] = 0;
      child.pathl = *pathl;
      child.udata = 0;

      type = entp->d_type;
      if(type == DT_UNKNOWN || type == DT_LNK)
      {
        fatal(xfstatats, dirfd, 0, &stb, path);
        type = (stb.st_mode & S_IFMT) >> 12;
      }

      if(type == DT_REG)
      {
        child.type = FTWAT_F;
        fatal(fn, 0, &child, arg, &stop);
      }
      else if(type == DT_DIR)
      {
        child.type = FTWAT_D;
        fatal(walk
          , fn
          , arg
          , dds
          , ddsz
          , ddsp
          , dirfd
          , path
          , pathz
          , pathl
          , &child
        );

        // reopen if necessary
        if(dd == 0)
        {
          *pathl = patho;
          path[*pathl] = 0;

          fatal(xopendirat, &dd, dirfd, path);
          ddso = *ddsp;
          *ddsp = ((*ddsp) + 1) % ddsz;
          dds[ddso] = &dd;
        }
      }
    }

    *pathl = patho;
    path[*pathl] = 0;

    seekdir(dd, dirloc);
    fatal(xreaddir, dd, &entp);
    fatal(xtelldir, &dirloc, dd);
  } while(entp);

  // post-order callback for directories
  fatal(fn, FTWAT_POST, dirinfo, arg, &stop);

finally:
  fatal(ixclosedir, &dd);
  if(ddso > -1 && dds[ddso] == &dd)
    dds[ddso] = 0;
coda;
}

xapi API nftwat(
    int dirfd
  , const char * dirpath
  , xapi (*fn)(int method, ftwinfo * restrict info, void * arg, int * stop)
  , int nopenfd
  , void * arg
)
{
  enter;

  DIR *** dds = 0;
  size_t ddsp = 0;

  char path[512]; // = {};
  size_t pathl = 0;

  fatal(xmalloc, &dds, sizeof(*dds) * nopenfd);
  pathl = snprintf(path, sizeof(path), "%s", dirpath);

  ftwinfo rootinfo = {};
  rootinfo.path = path;
  rootinfo.pathl = pathl;
  rootinfo.type = FTWAT_D;

  const char * e = path + pathl;
  while(e != path && *e != '/')
    e--;

  if(*e == '/')
    e++;

  rootinfo.name_off = e - path;

  fatal(walk
    , fn
    , arg
    , dds
    , nopenfd
    , &ddsp
    , dirfd
    , path
    , sizeof(path)
    , &pathl
    , &rootinfo
  );

finally:
  wfree(dds);
coda;
}
