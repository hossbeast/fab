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

#ifndef _XAPI_ERRORCODE_H
#define _XAPI_ERRORCODE_H

/// enter
//
// SUMMARY
//  must be the first line of any xapi function
//
#define enter                                         \
  __label__ XAPI_LEAVE, XAPI_FINALIZE, XAPI_FINALLY;  \
  int __xapi_f1 = 0;                                  \
  xapi __xapi_r[2] = { 0, 0 }

/*
** called at the site of an error
*/

/// fail
//
// SUMMARY
//  fail the current frame with the specified error code
//
// ARGUMENTS
//  etab    - error table
//  code    - error code
//  [table] - discarded
//  [key]   - discarded
//  [vstr]  - discarded
//  [vlen]  - discarded
//  [vfmt]  - discarded

#define tfail(etab, code)                          \
  do {                                             \
    __xapi_r[0] = (code) & 0xFFFF;                 \
    if(etab)                                       \
      __xapi_r[0] |= (((etable *)etab)->id << 16); \
    goto XAPI_FINALIZE;                            \
  } while(0)

#define tfails(etab, code, key, vstr)       tfail(etab, code)
#define tfailw(etab, code, key, vbuf, vlen) tfail(etab, code)
#define tfailf(etab, code, key, vfmt, ...)  tfail(etab, code)

#define fail(code)                   tfail (0, code)
#define fails(code, key, vstr)       tfails(0, code, key, vstr)
#define failw(code, key, vbuf, vlen) tfailw(0, code, vbuf, vlen)
#define failf(code, key, fmt, ...)   tfailf(0, code, vfmt, ##__VA_ARGS__)

/*
** called elsewhere in the stack
*/

#define xapi_invoke(func, ...)  \
  ({ func(__VA_ARGS__); })

/// invoke
//
// SUMMARY
//  invoke an xapi-enabled function and return its exit status
//
#define invoke(func, ...) xapi_invoke(func, ##__VA_ARGS__)

/// fatal
//
// SUMMARY
//  invoke another function and fail the current frame if that function fails
//
#define fatal(func, ...)                                  \
  do {                                                    \
    if((__xapi_r[0] = invoke(func, ##__VA_ARGS__)) != 0)  \
    {                                                     \
      goto XAPI_FINALIZE;                                 \
    }                                                     \
  } while(0)

/// fatalize
//
// SUMMARY
//  invoke a non-xapi-enabled function and if it fails, fail the current frame with the specified
//  error code, which is evaluated after the function returns
//
#define tfatalize(perrtab, code, func, ...)   \
  do {                                        \
    if(func(__VA_ARGS__) != 0)                \
      tfail(perrtab, code);                   \
  } while(0)

#define fatalize(code, func, ...)  tfatalize (perrtab, code, func, ##__VA_ARGS__)

/// xproxy
//
// 1-liner
//
#define xproxy(func, ...) return func(__VA_ARGS__)

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally                 \
  goto XAPI_FINALIZE;           \
XAPI_FINALIZE:                  \
  if(__xapi_f1)                 \
  {                             \
    goto XAPI_LEAVE;            \
  }                             \
  __xapi_r[1] = __xapi_r[0];    \
  __xapi_f1 = 1;                \
  goto XAPI_FINALLY;            \
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda            \
  goto XAPI_LEAVE;      \
XAPI_LEAVE:             \
  return XAPI_ERRVAL

#define conclude(r)     \
  goto XAPI_LEAVE;      \
XAPI_LEAVE:             \
  *(r) = XAPI_ERRVAL

/*
** called after finally
*/ 

#define xapi_infos(key, vstr)
#define xapi_infow(key, vbuf, vlen)
#define xapi_infof(key, vfmt, ...)
#define xapi_vinfof(key, vfmt, va)

/// XAPI_UNWINDING
//
// SUMMARY
//  true if an error has been raised
//
#define XAPI_UNWINDING (__xapi_r[0] || __xapi_r[1])

// does not apply to MODE_ERRORCODE
#define XAPI_ERRTAB 0

/// XAPI_ERROR
//
// SUMMARY
//  while unwinding, the error id, that is, errtab->id << 16 | errcode
//
#define XAPI_ERRVAL (__xapi_r[1] ?: __xapi_r[0])

/// XAPI_ERRCODE
//
// SUMMARY
//  while unwinding, the error code, and zero otherwise
//
#define XAPI_ERRCODE ((XAPI_ERRVAL) & 0xFFFF)

/// XAPI_THROWING
//
// SUMMARY
//  true while unwinding due to throwing the specified error
//
// PARAMETERS
//  errtab  - pointer to error table
//  errcode - error code
//
#define XAPI_THROWING(c) (XAPI_UNWINDING && XAPI_ERRCODE == ((c) & 0xFFFF))

#endif
