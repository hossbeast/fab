/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <fcntl.h>

#include "xlinux/xdirent.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"

#include "nftwat.h"

#include "zcat.h"

static int __attribute__((nonnull(1, 5, 7, 9, 11))) walk(
    void (*fn)(int method, ftwinfo * info, void * udata, int * stop)
  , void (*err)(int method, ftwinfo * info, void * udata, int error)
  , uint32_t flags
  , void * udata
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
  DIR *dd = 0;
  size_t patho;
  long dirloc;
  int ddso = -1;
  int stop = 0;
  struct stat stb;
  uint8_t type;
  struct dirent *entp;
  int r;
  int fd;
  ftwinfo child;
  int rv;

  patho = *pathl;
  rv = 0;

  // pre-order callback for directories
  fn(FTWAT_PRE, dirinfo, udata, &stop);

  if(stop) {
    goto end;
  }

  // if theres no room, close the descriptor in an earlier activation/frame
  if(dds[*ddsp]) {
    ixclosedir(dds[*ddsp]);
  }

  if((fd = openat(dirfd, path, O_RDONLY | O_DIRECTORY)) == -1) {
    err(FTWAT_OPENDIR, dirinfo, udata, errno);
    rv = -1;
    goto end;
  }
  dd = fdopendir(fd);
  ddso = *ddsp;
  *ddsp = ((*ddsp) + 1) % ddsz;
  dds[ddso] = &dd;

  memset(&child, 0, sizeof(child));
  child.parent = dirinfo;
  child.path = path;

  entp = xreaddir(dd);
  dirloc = xtelldir(dd);
  do
  {
    if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
    {
      *pathl += zcats(path + *pathl, pathz - *pathl, "/");
      child.name_off = *pathl;
      *pathl += zcats(path + *pathl, pathz - *pathl, entp->d_name);
      path[*pathl] = 0;
      child.pathl = *pathl;
      child.udata = 0;

      type = entp->d_type;

      if(type == DT_LNK && (flags & FTWAT_SYMLINKS) != 0)
      {
        if((flags & FTWAT_FOLLOW) != 0)
        {
          /* follow non-dangling symlinks */
          r = fstatat(dirfd, path, &stb, 0);
          if(r == 0) {
            type = (stb.st_mode & S_IFMT) >> 12;
          } else {
            memset(&stb, 0, sizeof(stb));
          }
        }
      }

      if(type == DT_REG)
      {
        child.type = FTWAT_F;
        fn(0, &child, udata, &stop);
      }
      else if(type == DT_LNK && (flags & FTWAT_SYMLINKS) != 0)
      {
        child.type = FTWAT_LNK;
        fn(0, &child, udata, &stop);
      }
      else if(type == DT_DIR)
      {
        child.type = FTWAT_D;
        rv = walk(
            fn
          , err
          , flags
          , udata
          , dds
          , ddsz
          , ddsp
          , dirfd
          , path
          , pathz
          , pathl
          , &child
        );
        if(rv != 0) {
          goto end;
        }

        // reopen if necessary
        if(dd == 0)
        {
          *pathl = patho;
          path[*pathl] = 0;

          if((fd = openat(dirfd, path, O_RDONLY | O_DIRECTORY)) == -1) {
            err(FTWAT_OPENDIR, dirinfo, udata, errno);
            rv = -1;
            goto end;
          }
          dd = fdopendir(fd);

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
  fn(FTWAT_POST, dirinfo, udata, &stop);

end:
  ixclosedir(&dd);
  if(ddso > -1 && dds[ddso] == &dd) {
    dds[ddso] = 0;
  }

  return rv;
}

void nftwat(
    int dirfd
  , const char * dirpath
  , void (*fn)(int method, ftwinfo * restrict info, void * udata, int * stop)
  , void (*err)(int method, ftwinfo * restrict info, void * udata, int error)
  , int nopenfd
  , uint32_t flags
  , void *udata
)
{
  DIR *** dds = 0;
  size_t ddsp = 0;
  char path[PATH_MAX];
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
    , err
    , flags
    , udata
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
