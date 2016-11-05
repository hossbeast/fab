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

#ifndef _FAB_REQUEST_H
#define _FAB_REQUEST_H

#include <stdarg.h>
#include <sys/types.h>
#include <stdarg.h>

struct fab_command;

struct memblk;
struct narrator;

#define FABCORE_TABLE(x, y)                                  \
  /* commands */                                             \
  FABCORE(MODE_BUILD                  , 0x00000001  , x, y)  \
  FABCORE(MODE_LICENSE                , 0x00000002  , x, y)  \
  FABCORE(MODE_TRACE                  , 0x00000003  , x, y)  \
  FABCORE(TARGET_FABRICATE            , 0x00000004  , x, y)  \
  FABCORE(TARGET_FABRICATE_EXACT      , 0x00000005  , x, y)  \
  FABCORE(TARGET_FABRICATE_NOFILE     , 0x00000006  , x, y)  \
  FABCORE(TARGET_INVALIDATE           , 0x00000007  , x, y)  \
  FABCORE(INVALIDATE                  , 0x00000008  , x, y)  \
  FABCORE(INVALIDATE_ALL              , 0x00000009  , x, y)  \
  FABCORE(CONFIGURATION_MERGE         , 0x0000000a  , x, y)  \
  FABCORE(CONFIGURATION_APPLY         , 0x0000000b  , x, y)  \
  FABCORE(BUILDSCRIPT_PATH            , 0x0000000c  , x, y)  \
  FABCORE(BUILDSCRIPT_VARS            , 0x0000000d  , x, y)  \
  /* list ops */                                             \
  FABCORE(SET                         , 0x00000100  , x, y)  \
  FABCORE(PUSH                        , 0x00000200  , x, y)  \
  FABCORE(CLEAR                       , 0x00000300  , x, y)

#define FABCORE_BUILD_TABLE(x)                      \
  /* FABCORE_MODE_BUILD options */                  \
  FABCORE_BUILD(EXEC                  , 0x01  , x)  \
  FABCORE_BUILD(GENERATE              , 0x02  , x)  \
  FABCORE_BUILD(SCRIPT                , 0x03  , x)

#define FABCORE_LICENSE_TABLE(x)                    \
  /* FABCORE_MODE_LICENSE options */                \
  FABCORE_LICENSE(STANDARD            , 0x01  , x)  \
  FABCORE_LICENSE(FAB                 , 0x02  , x)  \

#define FABCORE_TRACE_TABLE(x)                      \
  /* FABCORE_MODE_TRACE options */                  \
  FABCORE_TRACE(FULL                  , 0x01  , x)  \
  FABCORE_TRACE(PITHY                 , 0x02  , x)

#define FABCORE_TARGET_TABLE(x)                     \
  /* FABCORE_TARGET options */                      \
  FABCORE_TARGET(ADD                  , 0x01  , x)  \
  FABCORE_TARGET(REMOVE               , 0x02  , x)

enum {
#define FABCORE(a, b, x, y) FABCORE_ ## a = UINT32_C(b),
FABCORE_TABLE(0, 0)
#undef FABCORE
};

enum {
#define FABCORE_BUILD(a, b, x) FABCORE_BUILD_ ## a = UINT8_C(b),
FABCORE_BUILD_TABLE(0)
#undef FABCORE_BUILD
};

enum {
#define FABCORE_LICENSE(a, b, x) FABCORE_LICENSE_ ## a = UINT8_C(b),
FABCORE_LICENSE_TABLE(0)
#undef FABCORE_LICENSE
};

enum {
#define FABCORE_TRACE(a, b, x) FABCORE_TRACE_ ## a = UINT8_C(b),
FABCORE_TRACE_TABLE(0)
#undef FABCORE_TRACE
};

enum {
#define FABCORE_TARGET(a, b, x) FABCORE_TARGET_ ## a = UINT8_C(b),
FABCORE_TARGET_TABLE(0)
#undef FABCORE_TARGET
};

typedef struct fab_request
{
  pid_t                 client_pid;
  struct fab_command ** commands;
  size_t                commandsl;
  size_t                commandsa;
} fab_request;

#define restrict __restrict

/// fab_request_create
//
// SUMMARY
//  create a request
//
// PARAMETERS
//  request - (returns) request instance
//
xapi fab_request_create(fab_request ** const restrict request, pid_t client_pid)
  __attribute__((nonnull));

/// fab_request_free
//
// SUMMARY
//  free a request with free semantics
//
void fab_request_free(fab_request * const restrict request);

/// fab_request_ifree
//
// SUMMARY
//  free a request with iwfree semantics
//
void fab_request_ifree(fab_request ** const restrict request)
  __attribute__((nonnull));

/// fab_request_freeze
//
// SUMMARY
//  
//
void fab_request_freeze(fab_request * const restrict req, struct memblk * const restrict mb)
  __attribute__((nonnull));

/// fab_request_thaw
//
//
//
void fab_request_thaw(fab_request * const restrict req, char * const restrict mb)
  __attribute__((nonnull));

/// fab_request_say
//
//
//
xapi fab_request_say(const fab_request * const restrict res, struct narrator * const restrict N)
  __attribute__((nonnull));

/// fab_request_command
//
// SUMMARY
//  append a command to a request
//
// PARAMETERS
//  req   - request
//  attrs - command attributes
//  text  - 
//  u8    - 
//
xapi fab_request_command(fab_request * const restrict req, uint32_t attrs)
  __attribute__((nonnull));

xapi fab_request_commands(fab_request * const restrict req, uint32_t attrs, const char * const restrict s)
  __attribute__((nonnull));

xapi fab_request_commandf(fab_request * const restrict req, uint32_t attrs, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 3)));

xapi fab_request_commandvf(fab_request * const restrict req, uint32_t attrs, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(1, 3)));

xapi fab_request_commandu8(fab_request * const restrict req, uint32_t attrs, uint8_t value)
  __attribute__((nonnull));

/// fab_request_command_target
//
// SUMMARY
//  append a target command to a request
//
// PARAMETERS
//  req   - request
//  attrs - command attributes
//  opt   - target option
//  text  - target text
//
xapi fab_request_command_target(fab_request * const restrict req, uint32_t attrs, uint8_t opt, const char * const restrict s)
  __attribute__((nonnull));

#undef restrict
#endif
