/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>


#include "xlinux/xdirent.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"

#include "nftwat.h"

#include "zbuffer.h"

static void __attribute__((nonnull(1, 4, 6, 8, 10))) walk(
    void (*fn)(int method, ftwinfo * info, void * arg, int * stop)
  , uint32_t flags
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
  DIR * dd = 0;
  size_t patho;
  long dirloc;
  int ddso = -1;
  int stop = 0;
  struct stat stb;
  uint8_t type;
  struct dirent *entp;
  int r;

  patho = *pathl;

  // pre-order callback for directories
  fn(FTWAT_PRE, dirinfo, arg, &stop);

  if(stop) {
    goto end;
  }

  // if theres no room, close the descriptor in an earlier activation/frame
  if(dds[*ddsp]) {
    ixclosedir(dds[*ddsp]);
  }

  dd = xopendirat(dirfd, path);
  ddso = *ddsp;
  *ddsp = ((*ddsp) + 1) % ddsz;
  dds[ddso] = &dd;

  ftwinfo child = {};
  child.parent = dirinfo;
  child.path = path;

  entp = xreaddir(dd);
  dirloc = xtelldir(dd);
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

      if(type == DT_LNK && (flags & FTWAT_SYMLINKS) != 0)
      {
        if((flags & FTWAT_FOLLOW) != 0)
        {
          /* follow non-dangling symlinks */
          r = uxfstatats(dirfd, 0, &stb, path);
          if(r == 0)
          {
            type = (stb.st_mode & S_IFMT) >> 12;
          }
        }
      }

      if(type == DT_REG)
      {
        child.type = FTWAT_F;
        fn(0, &child, arg, &stop);
      }
      else if(type == DT_LNK && (flags & FTWAT_SYMLINKS) != 0)
      {
        child.type = FTWAT_LNK;
        fn(0, &child, arg, &stop);
      }
      else if(type == DT_DIR)
      {
        child.type = FTWAT_D;
        walk(
            fn
          , flags
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

          dd = xopendirat(dirfd, path);
          ddso = *ddsp;
          *ddsp = ((*ddsp) + 1) % ddsz;
          dds[ddso] = &dd;
        }
      }
    }

    *pathl = patho;
    path[*pathl] = 0;

    seekdir(dd, dirloc);
    entp = xreaddir(dd);
    dirloc = xtelldir(dd);
  } while(entp);

  // post-order callback for directories
  fn(FTWAT_POST, dirinfo, arg, &stop);

end:
  ixclosedir(&dd);
  if(ddso > -1 && dds[ddso] == &dd) {
    dds[ddso] = 0;
  }
}

void API nftwat(
    int dirfd
  , const char * dirpath
  , void (*fn)(int method, ftwinfo * restrict info, void * arg, int * stop)
  , int nopenfd
  , uint32_t flags
  , void * arg
)
{
  DIR *** dds = 0;
  size_t ddsp = 0;
  char path[512];
  size_t pathl = 0;
  const char *e;

  xmalloc(&dds, sizeof(*dds) * nopenfd);
  pathl = snprintf(path, sizeof(path), "%s", dirpath);

  ftwinfo rootinfo = {};
  rootinfo.path = path;
  rootinfo.pathl = pathl;
  rootinfo.type = FTWAT_D;

  e = path + pathl;
  while(e != path && *e != '/') {
    e--;
  }

  if(*e == '/') {
    e++;
  }

  rootinfo.name_off = e - path;

  walk(
      fn
    , flags
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

  free(dds);
}
