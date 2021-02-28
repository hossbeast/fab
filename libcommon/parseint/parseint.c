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

#include <string.h>

#include "xapi/SYS.errtab.h"

#include "parseint.h"

int API parseint(const char* const s, char* f, intmax_t lo, intmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
  intmax_t  tr = 0;
  int   tn = 0;

  errno = 0;
  if(strchr(f, 'd'))
  {
    if(sscanf(s, "%"SCNdMAX"%n", &tr, &tn) != 1)
    {
      return 1;
    }
  }
  else if(strchr(f, 'i'))
  {
    if(sscanf(s, "%"SCNiMAX"%n", &tr, &tn) != 1)
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }

  if(errno == ERANGE)         /* larger, or smaller, than intmax can hold */
  {
    return 1;
  }
  else if(tr < lo)            /* minimum value threshold  */
  {
    return 1;
  }
  else if(tr > hi)            /* maximum value threshold  */
  {
    return 1;
  }
  else if(tn < min)           /* minimum length threshold */
  {
    return 1;
  }
  else if(tn > max)           /* maximum length threshold */
  {
    return 1;
  }

  if(strstr(f, "ll") == f)
  {
    if(r)
      *(signed long long*)r = tr;
  }
  else if(strstr(f, "l") == f)
  {
    if(r)
      *(signed long*)r = tr;
  }
  else if(strstr(f, "hh") == f)
  {
    if(r)
      *(signed char*)r = tr;
  }
  else if(strstr(f, "h") == f)
  {
    if(r)
      *(signed short*)r = tr;
  }
  else
  {
    if(r)
      *(signed int*)r = tr;
  }

  if(rn)
    *rn = tn;

  return 0;
}

int API parseuint(const char* const s, char* f, uintmax_t lo, uintmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
  uintmax_t tr = 0;
  int   tn = 0;

  errno = 0;
  if(strchr(f, 'o'))
  {
    if(sscanf(s, "%"SCNoMAX"%n", &tr, &tn) != 1)
    {
      return 1;
    }
  }
  else if(strchr(f, 'u'))
  {
    if(sscanf(s, "%"SCNuMAX"%n", &tr, &tn) != 1)
    {
      return 1;
    }
  }
  else if(strchr(f, 'x'))
  {
    if(sscanf(s, "%"SCNxMAX"%n", &tr, &tn) != 1)
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }

  if(errno == ERANGE)         /* larger, or smaller, than intmax can hold */
  {
    return 1;
  }
  else if(tr < lo)            /* minimum value threshold  */
  {
    return 1;
  }
  else if(tr > hi)            /* maximum value threshold  */
  {
    return 1;
  }
  else if(tn < min)           /* minimum length threshold */
  {
    return 1;
  }
  else if(tn > max)           /* maximum length threshold */
  {
    return 1;
  }

  if(r)
  {
    if(strstr(f, "ll") == f)
    {
      *(unsigned long long*)r = tr;
    }
    else if(strstr(f, "l") == f)
    {
      *(unsigned long*)r = tr;
    }
    else if(strstr(f, "hh") == f)
    {
      *(unsigned char*)r = tr;
    }
    else if(strstr(f, "h") == f)
    {
      *(unsigned short*)r = tr;
    }
    else
    {
      *(unsigned int*)r = tr;
    }
  }

  if(rn)
    *rn = tn;

  return 0;
}

xapi API xparseint(const char* const s, char* f, intmax_t lo, intmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
  enter;

  if(parseint(s, f, lo, hi, min, max, r, rn)) {
    fail(SYS_INVALID);
  }

finally:
  xapi_infos("string", s);
  xapi_infos("format", f);
coda;
}

xapi API xparseuint(const char* const s, char* f, uintmax_t lo, uintmax_t hi, uint8_t min, uint8_t max, void* r, int* rn)
{
  enter;

  if(parseuint(s, f, lo, hi, min, max, r, rn)) {
    fail(SYS_INVALID);
  }

finally:
  xapi_infos("string", s);
  xapi_infos("format", f);
coda;
}
