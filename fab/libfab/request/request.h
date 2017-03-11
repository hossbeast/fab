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

#ifndef FAB_REQUEST_H
#define FAB_REQUEST_H

struct memblk;
struct narrator;

typedef struct fab_request fab_request;

#define FAB_ATTR_TABLE(x)                               \
  FAB_ATTR(TARGET         , target        , 0x01  , x)  \
  FAB_ATTR(TARGET_EXACT   , target-exact  , 0x02  , x)  \
  FAB_ATTR(TARGET_NOFILE  , target-nofile , 0x03  , x)

enum {
#define FAB_ATTR(a, b, c, x) FAB_ATTR_ ## a = UINT32_C(c),
FAB_ATTR_TABLE(0)
#undef FAB_ATTR
};

const char ** fab_attr_names;
size_t fab_attr_num;

#define FAB_ATTR_TARGET_OPT 0x03

#define FAB_ATTR_TARGET_NAME(x)  \
  ((x) < 1 || (x) > fab_attr_num) ? "(unknown)" : fab_attr_names[(x)]

#define restrict __restrict

/// fab_request_create
//
// SUMMARY
//  create a request
//
// PARAMETERS
//  request - (returns) request instance
//
xapi fab_request_create(fab_request ** restrict request)
  __attribute__((nonnull));

/// fab_request_free
//
// SUMMARY
//  free a request with free semantics
//
void fab_request_free(fab_request * restrict request);

/// fab_request_ifree
//
// SUMMARY
//  free a request with idempotent semantics
//
void fab_request_ifree(fab_request ** restrict request)
  __attribute__((nonnull));

/// fab_request_freeze
//
// SUMMARY
//
//
void fab_request_freeze(fab_request * restrict req, struct memblk * restrict mb)
  __attribute__((nonnull));

/// fab_request_thaw
//
//
//
void fab_request_thaw(fab_request * restrict req, void * restrict mb)
  __attribute__((nonnull));

/// fab_request_say
//
//
//
xapi fab_request_say(const fab_request * restrict res, struct narrator * restrict N)
  __attribute__((nonnull));

xapi fab_request_select(fab_request * restrict req, uint32_t attr, const char * restrict targets)
  __attribute__((nonnull));

xapi fab_request_build(fab_request * restrict req)
  __attribute__((nonnull));

xapi fab_request_plan(fab_request * restrict req)
  __attribute__((nonnull));

xapi fab_request_script(fab_request * restrict req)
  __attribute__((nonnull));

xapi fab_request_invalidate(fab_request * restrict req, const char * restrict text)
  __attribute__((nonnull));

xapi fab_request_invalidate_all(fab_request * restrict req)
  __attribute__((nonnull));

xapi fab_request_inspect(fab_request * restrict req, const char * restrict text)
  __attribute__((nonnull));

xapi fab_request_query(fab_request * restrict req, const char * restrict text)
  __attribute__((nonnull));

xapi fab_request_config_stages(fab_request * restrict req, const char * restrict s)
  __attribute__((nonnull(1, 2)));

xapi fab_request_config_stagef(fab_request * restrict req, const char * restrict fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi fab_request_config_apply(fab_request * restrict req)
  __attribute__((nonnull));

#if 0
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
xapi fab_request_commands(fab_request * restrict req, uint32_t attrs, const char * restrict s)
  __attribute__((nonnull));

xapi fab_request_commandf(fab_request * restrict req, uint32_t attrs, const char * restrict fmt, ...)
  __attribute__((nonnull(1, 3)));

xapi fab_request_commandvf(fab_request * restrict req, uint32_t attrs, const char * restrict fmt, va_list va)
  __attribute__((nonnull(1, 3)));

xapi fab_request_commandu8(fab_request * restrict req, uint32_t attrs, uint8_t value)
  __attribute__((nonnull));
#endif

#undef restrict
#endif
