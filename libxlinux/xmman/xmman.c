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

#include <errno.h>

#include "internal.h"
#include "xmman/xmman.h"
#include "errtab/SYS.errtab.h"

API xapi xmmap(void * addr, size_t length, int prot, int flags, int fd, off_t offset, void ** r)
{
  enter;

  if(r && (*r = mmap(addr, length, prot, flags, fd, offset)) == MAP_FAILED)
    fail(errno);

  else if(!r && mmap(addr, length, prot, flags, fd, offset) == MAP_FAILED)
    fail(errno);
  
finally:
  XAPI_INFOF("length", "%zu", length);
coda;
}

API xapi xmunmap(void * addr, size_t length)
{
  enter;

  fatalize(errno, munmap, addr, length);

  finally : coda;
}

API xapi ixmunmap(void * addr, size_t length)
{
  enter;

  if(*(void**)addr)
    fatalize(errno, munmap, *(void**)addr, length);

  *(void**)addr = 0;

  finally : coda;
}
