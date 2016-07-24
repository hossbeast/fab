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

#ifndef _ARGS_H
#define _ARGS_H

#include <sys/types.h>

#include "xapi.h"

struct request;
struct memblk;

#define MODE_TABLE(x)                                                                                           \
/* execution modes */                                                                                           \
  _MODE(MODE_BUILD_EXEC                 , 0x01  , x)    /* execute the buildplan */                             \
  _MODE(MODE_BUILD_GENERATE             , 0x02  , x)    /* (only) generate the buildplan */                     \
  _MODE(MODE_BUILD_BUILDSCRIPT          , 0x03  , x)    /* create a buildscript from the buildplan */

#if DEBUG || DEVEL
#define MODE_TABLE_DEBUG(x)                                                                                     \
/* error reporting modes */                                                                                     \
  _MODE(MODE_BACKTRACE_FULL             , 0x0b  , x)    /* report on immediate error condition only */          \
  _MODE(MODE_BACKTRACE_PITHY            , 0x0c  , x)    /* unwind stack when reporting errors */
#endif
#if DEVEL
#define MODE_TABLE_DEVEL(x)                                                                                     \
/* buildscript license modes */                                                                                 \
  _MODE(MODE_LICENSE_STD                , 0x10  , x)    /* buildscripts have the standard license  */           \
  _MODE(MODE_LICENSE_FAB                , 0x11  , x)    /* buildscripts have the fab license */
#endif

enum {
#define _MODE(a, b, c) a = b,
#if DEBUG || DEVEL
MODE_TABLE_DEBUG(0)
#endif
#if DEVEL
MODE_TABLE_DEVEL(0)
#endif
MODE_TABLE(0)
#undef _MODE
};

#define _MODE(a, b, c) (c) == b ? #a :
#if DEBUG && DEVEL
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) MODE_TABLE_DEVEL(x) "UNKNWN"
#elif DEBUG
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEBUG(x) "UNKNWN"
#elif DEVEL
# define MODE_STR(x) MODE_TABLE(x) MODE_TABLE_DEVEL(x) "UNKNWN"
#else
# define MODE_STR(x) MODE_TABLE(x) "UNKNWN"
#endif

#define TARGET_TABLE(x)                       \
  TARGET(FABRICATE              , 0x01  , x)  \
  TARGET(FABRICATE_EXACT        , 0x02  , x)  \
  TARGET(FABRICATE_NOFILE       , 0x04  , x)  \
  TARGET(INVALIDATE             , 0x08  , x)

enum {
#define TARGET(a, b, x) TARGET_ ## a = UINT8_C(b),
TARGET_TABLE(0)
#undef TARGET
};

#define TARGET(a, b, x) (x) == b ? #a :
#define TARGET_STR(x) TARGET_TABLE(x) "UNKNOWN"

extern struct g_args
{
  int                 mode_build;                 // buildplan mode
#if DEBUG || DEVEL
  int                 mode_backtrace;             // backtrace reporting mode
#endif
#if DEVEL
  int                 mode_license;               // buildscript license mode
#endif

  int                 concurrency;                // concurrency setting
  int                 procs;                      // 

  char *              buildscript_path;           // path to buildscript
  char **             buildscript_runtime_vars;   // buildscript variables that are settable at runtime
  size_t              buildscript_runtime_varsl;

  struct target {                                 // selectors for build targets
    char *  text;
    char    mode;     // + or -
    uint8_t lists;    // bitwise combo of TARGET_*
  } *                 targets;
  size_t              targetsl;

  int                 invalidate_all;             // invalidate all nodes (-B)
} * g_args;

/// args_parse
//
// SUMMARY
//  parses command-line options, populating g_args.
//
// REMARKS
//  prints the usage statement and terminates the program if options are
//  invalid, for example required options are not present, or invalid
//  parameters are given to an option
//
xapi args_parse(void);

/// args_report
//
// SUMMARY
//  log a summary of args as-parsed
//
xapi args_report(void);

/// args_teardown
//
// SUMMARY
//  free g_args
//
void args_teardown(void);

/// args_collate_request
//
// SUMMARY
//  build a fab request from g_args
//
xapi args_collate_request(struct memblk * const restrict mb, struct request ** const restrict req)
  __attribute__((nonnull));

#endif
