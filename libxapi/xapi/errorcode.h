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
#define enter           \
  int __xapi_f1 = 0;    \
  uint32_t __xapi_r = 0;

/*
** called at the site of an error
*/

/// fail
//
// SUMMARY
//  fail the current frame with the specified error code
//
// ARGUMENTS
//  code    - error code
//  [table] - discarded
//  [msg]   - discarded
//  [msgl]  - discarded
//  [fmt]   - discarded

#define tfail(perrtab, code)                        \
  do {                                              \
    __xapi_r = code & 0xFFFF;                       \
    if(perrtab)                                     \
      __xapi_r |= (((etable *)perrtab)->id << 16);  \
    goto XAPI_FINALIZE;                             \
  } while(0)

#define tfails(perrtab, code, msg)        tfail(perrtab, code)
#define tfailw(perrtab, code, msg, msgl)  tfail(perrtab, code)
#define tfailf(perrtab, code, fmt, ...)   tfail(perrtab, code)

#define fail(code)             tfail (0, code)
#define fails(code, msg)       tfails(0, code, msg)
#define failw(code, msg, msgl) tfailw(0, code, msg, msgl)
#define failf(code, fmt, ...)  tfailf(0, code, fmt, ##__VA_ARGS__)

/*
** called elsewhere in the stack
*/

/// fatal
//
// SUMMARY
//  invoke another function and fail the current frame if that function fails
//
#define fatal(func, ...)                      \
  do {                                        \
    if((__xapi_r = func(__VA_ARGS__)) != 0)   \
    {                                         \
      goto XAPI_FINALIZE;                     \
    }                                         \
  } while(0)

/// fatalize
//
// SUMMARY
//  invoke a non-xapi-enabled function and if it fails, fail the current frame with its error code
//
#define tfatalize(perrtab, code, func, ...)   \
  do {                                        \
    if(func(__VA_ARGS__) != 0)                \
      tfail(perrtab, code);                   \
  } while(0)

#define tfatalizes(perrtab, code, msg, func, ...)         \
  tfatalizew(perrtab, code, msg, 0, func, ##__VA_ARGS__)

#define tfatalizew(perrtab, code, msg, msgl, func, ...)   \
  do {                                                    \
    if(func(__VA_ARGS__))                                 \
      tfailw(perrtab, code, msg, msgl);                   \
  } while(0)

#define fatalize(code, func, ...)  tfatalize (perrtab, code, func, ##__VA_ARGS__)
#define fatalizes(code, func, ...) tfatalizes(perrtab, code, func, ##__VA_ARGS__)
#define fatalizew(code, func, ...) tfatalizew(perrtab, code, func, ##__VA_ARGS__)

/// xproxy
//
// 1-liner
//
#define xproxy(func, ...)       \
  fatal(func, ##__VA_ARGS__);   \
  finally : coda

#define prologue                \
  __label__ XAPI_FINALLY;

/// invoke
//
// SUMMARY
//  
//

/// finally
//
// SUMMARY
//  statements between finally and coda are executed even upon fail/leave
//
#define finally         \
  goto XAPI_FINALIZE;   \
XAPI_FINALIZE:          \
  if(__xapi_f1)         \
  {                     \
    goto XAPI_LEAVE;    \
  }                     \
  __xapi_f1 = 1;        \
  goto XAPI_FINALLY;    \
XAPI_FINALLY

/// coda
//
// SUMMARY
//  return from the current function
//
#define coda            \
  goto XAPI_LEAVE;      \
XAPI_LEAVE:             \
  return __xapi_r


#define conclude(r)     \
  goto XAPI_LEAVE;      \
XAPI_LEAVE:             \
  *(r) = __xapi_r

/*
** called after finally
*/ 

#define XAPI_INFOS(k, vstr)
#define XAPI_INFOW(k, vstr, len)
#define XAPI_INFOF(k, vfmt, ...)

/// XAPI_UNWINDING
//
// SUMMARY
//  true if an error has been raised
//
#define XAPI_UNWINDING __xapi_r

// does not apply to MODE_ERRORCODE
#define XAPI_ERRTAB 0

/// XAPI_ERROR
//
// SUMMARY
//  while unwinding, the error id, that is, errtab->id << 16 | errcode
//
#define XAPI_ERRVAL __xapi_r

/// XAPI_ERRCODE
//
// SUMMARY
//  while unwinding, the error code, and zero otherwise
//
#define XAPI_ERRCODE (__xapi_r & 0xFFFF)

#endif
