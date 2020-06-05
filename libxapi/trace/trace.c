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

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"
#include "trace.internal.h"
#include "calltree.internal.h"
#include "errtab/XAPI.errtab.h"
#include "exit.internal.h"
#include "frame.internal.h"
#include "info.internal.h"

#include "zbuffer.h"
#include "color.h"
#include "macros.h"

#define TRACE_COLORIZE_OPT        UINT16_C(0x0003)
#define TRACE_NEWLINE_OPT         UINT16_C(0x000C)
#define TRACE_INFO_UNIQUE_OPT     UINT16_C(0x0030)
#define TRACE_INFO_LOCATION_OPT   UINT16_C(0x00C0)

#define sayf(...)  z += _sayf (dst + z, sz - z, ##__VA_ARGS__)
#define csayf(...) z += _csayf(dst + z, sz - z, attrs, ##__VA_ARGS__)

#define says(...)  z += _says (dst + z, sz - z, ##__VA_ARGS__)
#define csays(...) z += _csays(dst + z, sz - z, attrs, ##__VA_ARGS__)

static inline size_t _csayvf(char * restrict dst, size_t sz, uint16_t attrs, void * restrict cbuf, size_t clen, const char * restrict fmt, va_list va)
{
  size_t z = 0;
  if((attrs & TRACE_COLORIZE_OPT) == XAPI_TRACE_COLORIZE)
    z += znloadw(dst + z, sz - z, cbuf, clen);
  z += znloadvf(dst + z, sz - z, fmt, va);
  if((attrs & TRACE_COLORIZE_OPT) == XAPI_TRACE_COLORIZE)
    z += znloadw(dst + z, sz - z, NOCOLOR);
  return z;
}

static inline size_t _csayf(char * restrict dst, size_t sz, uint16_t attrs, void * restrict cbuf, size_t clen, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  size_t z = _csayvf(dst, sz, attrs, cbuf, clen, fmt, va);

  va_end(va);
  return z;
}

static inline size_t _sayf(char * restrict dst, size_t sz, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  size_t z = _csayvf(dst, sz, 0, 0, 0, fmt, va);

  va_end(va);
  return z;
}

static inline size_t _csays(char * restrict dst, size_t sz, uint16_t attrs, void * restrict cbuf, size_t clen, const char * restrict s)
{
  size_t z = 0;
  if((attrs & TRACE_COLORIZE_OPT) == XAPI_TRACE_COLORIZE && cbuf)
    z += znloadw(dst + z, sz - z, cbuf, clen);
  z += znloads(dst + z, sz - z, s);
  if((attrs & TRACE_COLORIZE_OPT) == XAPI_TRACE_COLORIZE && cbuf)
    z += znloadw(dst + z, sz - z, NOCOLOR);
  return z;
}

static inline size_t _says(char * restrict dst, size_t sz, const char * restrict s)
{
  return _csays(dst, sz, 0, 0, 0, s);
}

//
// static
//

/// calltree_locate_substack
//
// RETURNS
//  greatest parent index within the window, or -1
//
static int calltree_locate_substack(calltree * const restrict ct, int a, int b)
{
  int x;
  for(x = b; x > a; x--)
  {
    if(ct->frames.v[x].parent_index >= a)
      return x;
  }

  return -1;
}

static size_t error_trace(char * const dst, const size_t sz, xapi e, uint16_t attrs)
{
  size_t z = 0;

  const errtab * etab  = xapi_exit_errtab(e);
  const xapi_code code = xapi_exit_errcode(e);

  if(etab && code)
  {
    says("[");
    csays(BOLD_RED, etab->name);
    says(":");
    csays(BOLD_RED, code > etab->max ? "UNKNWN" : etab->v[code + (etab->min * -1)].name);
    says("]");
    says(" ");
    csays(LIGHT_GRAY, code > etab->max ? "unspecified error" : etab->v[code + (etab->min * -1)].desc);
  }

  // this can happen if, for example, the object containing the error table has been unloaded
  else
  {
    says("[");
    csays(BOLD_RED, "(TABNAME)");
    says(":");
    csays(BOLD_RED, "(ERRNAME)");
    says("]");
    says(" ");
    csays(LIGHT_GRAY, "(ERRDESC)");
  }

  return z;
}

/// infos_trace
//
// SUMMARY
//  write a summary of the infos in a sequence of frames in a calltree. The sequence is
//  contiguous but may contain a hole, which is skipped
//
// PARAMETERS
//  dst - buffer to write to
//  sz  - size of dst
//  ct  - calltree
//  a1  - index of the first frame in the sequence
//  a2  - index of the last frame in the hole, or -1
//  b2  - index of the first frame in the hole
//  b1  - index of the last frame in the sequence
//  attrs - whether to attrs using terminal escapes
//
// RETURNS
//  number of bytes written to dst
//
static size_t infos_trace(char * const dst, const size_t sz, calltree * const restrict ct, int a1, int a2, int b2, int b1, uint16_t attrs);
static size_t infos_trace(char * const dst, const size_t sz, calltree * const restrict ct, int a1, int a2, int b2, int b1, uint16_t attrs)
{
  info * nfo = 0;
  size_t z = 0;
  int x;
  int y;
  int i;
  int j;

  for(x = a1; x <= b1; x++)
  {
    if(x == a2) // skip the subsequence
      x = b2 + 1;

    for(y = 0; y < ct->frames.v[x].infos.l; y++)
    {
      // determine whether an info by this name was already used on this sequence
      for(i = a1; i < x; i++)
      {
        if(i == a2)
          i = b2;

        for(j = 0; j < ct->frames.v[i].infos.l; j++)
        {
          if(memncmp(
              ct->frames.v[x].infos.v[y].ks
            , ct->frames.v[x].infos.v[y].kl
            , ct->frames.v[i].infos.v[j].ks
            , ct->frames.v[i].infos.v[j].kl) == 0)
          {
            break;
          }
        }
        if(j < ct->frames.v[i].infos.l)
        {
          break;
        }
      }

      if(i == x)
      {
        if(nfo)
        {
          if(z == 0)
            csays(DIM_CYAN, " with ");
          else
            csays(DIM_CYAN, " ");

          csayf(LIGHT_YELLOW, "%.*s", (int)nfo->kl, nfo->ks);
          csayf(LIGHT_GRAY, " %.*s", (int)nfo->vl, nfo->vs);
        }
        nfo = &ct->frames.v[x].infos.v[y];
      }
    }
  }

  if(nfo)
  {
    if(z == 0)
      csays(DIM_CYAN, " with ");
    else
      csays(DIM_CYAN, " and ");

    csayf(LIGHT_YELLOW, "%.*s", (int)nfo->kl, nfo->ks);
    csayf(LIGHT_GRAY, " %.*s", (int)nfo->vl, nfo->vs);
  }

  return z;
}

static size_t frame_trace_function(char * const dst, const size_t sz, frame * f, uint16_t attrs)
{
  size_t z = 0;
  csays(GREEN, f->func);
  return z;
}

static size_t frame_trace_location(char * const dst, const size_t sz, frame * f, uint16_t attrs)
{
  size_t z = 0;

  const char * file = f->file;
  const char * n;
  while((n = strstr(file, "/")))
  {
    file = n + 1;
  }

  csayf(GREEN, "%s:%d", file, f->line);

  return z;
}

static size_t frame_trace(char * const dst, const size_t sz, frame * f, int loc, int in, int level, uint16_t attrs)
{
  size_t z = 0;

  if(in)
    csays(DIM_CYAN, "in ");

  z += frame_trace_function(dst + z, sz - z, f, attrs);

  if(loc && f->file)
  {
    csays(DIM_CYAN, " at ");
    z += frame_trace_location(dst + z, sz - z, f, attrs);
  }

  return z;
}

/// calltree_trace_frames
//
// SUMMARY
//  write a trace of a sequence of frames in a calltree
//
// PARAMETERS
//  dst   - buffer to write to
//  sz    - size of dst
//  ct    - calltree
//  a     - index of the first frame in the sequence
//  b     - index of the last frame in the sequence
//  level - indentation
//
// RETURNS
//  number of bytes written to dst
//
static size_t calltree_trace_frames(char * const dst, const size_t sz, calltree * const restrict ct, int a1, int b1, int level, uint16_t attrs);
static size_t calltree_trace_frames(char * const dst, const size_t sz, calltree * const restrict ct, int a1, int b1, int level, uint16_t attrs)
{
  int x;
  size_t z = 0;

  int a2 = -1; // start index for the subsequence
  int b2 = -1; // end index for the subsequence

  if((x = calltree_locate_substack(ct, a1, b1)) != -1)
  {
    a2 = ct->frames.v[x].parent_index + 1;
    b2 = x;
  }

  sayf("%*s", level * 2, "");
  z += error_trace(dst + z, sz - z, ct->frames.v[a1].exit, attrs);
  z += infos_trace(dst + z, sz - z, ct, a1, a2, b2, b1, attrs);

  sayf("\n");

  // main sequence
  for(x = a1; x <= b1; x++)
  {
    if(a2 != -1 && x == a2)
      break;

    sayf("%*s", level * 2, "");
    if(a2 != -1)
      csayf(DIM_CYAN, " %2d : ", b1 - x - ((b2 - a2) + 1));
    else
      csayf(DIM_CYAN, " %2d : ", b1 - x);

    z += frame_trace(dst + z, sz - z, &ct->frames.v[x], 1, 1, level, attrs);

    if(x < b1)
      sayf("\n");
  }

  if(a2 != -1)
  {
    // subsequence
    z += calltree_trace_frames(dst + z, sz - z, ct, a2, b2, level + 1, attrs);

    // trailing segment of the main sequence
    if(b1 != b2)
      sayf("\n");

    for(x = b2 + 1; x <= b1; x++)
    {
      sayf("%*s", level * 2, "");
      csayf(DIM_CYAN, " %2d : ", b1 - x);
      z += frame_trace(dst + z, sz - z, &ct->frames.v[x], 1, 1, level, attrs);

      if(x < b1)
        sayf("\n");
    }
  }

  return z;
}

//
// API
//

size_t API xapi_trace_calltree_pithy(calltree * const restrict ct, char * const restrict dst, const size_t sz, uint16_t attrs)
{
  size_t z = 0;

  int a2 = -1; // start index for the subsequence
  int b2 = -1; // end index for the subsequence

  int x;
  if((x = calltree_locate_substack(ct, 0, ct->frames.l - 1)) != -1)
  {
    a2 = ct->frames.v[x].parent_index + 1;
    b2 = x;
  }

  z += error_trace(dst + z, sz - z, ct->frames.v[0].exit, attrs);
  z += infos_trace(dst + z, sz - z, ct, 0, a2, b2, ct->frames.l - 1, attrs);

  if((attrs & TRACE_NEWLINE_OPT) != XAPI_TRACE_NONEWLINE)
    z += znloadc(dst + z, sz - z, '\n');

  return z;
}

size_t API xapi_trace_calltree_full(calltree * const restrict ct, char * const restrict dst, const size_t sz, uint16_t attrs)
{
  size_t z = 0;

  z += calltree_trace_frames(dst, sz, ct, 0, ct->frames.l - 1, 0, attrs);

  if((attrs & TRACE_NEWLINE_OPT) != XAPI_TRACE_NONEWLINE)
    z += znloadc(dst + z, sz - z, '\n');

  return z;
}

size_t API xapi_trace_pithy(char * const dst, const size_t sz, uint16_t attrs)
{
  return xapi_trace_calltree_pithy(g_calltree, dst, sz, attrs);
}

size_t API xapi_trace_full(char * const dst, const size_t sz, uint16_t attrs)
{
  return xapi_trace_calltree_full(g_calltree, dst, sz, attrs);
}

void API xapi_pithytrace_to(int fd)
{
  char space[4096];
  size_t z = xapi_trace_pithy(space, sizeof(space), 0);
  int __attribute__((unused)) r = write(fd, space, z);
}

void API xapi_fulltrace_to(int fd)
{
  char space[4096];
  size_t z = xapi_trace_full(space, sizeof(space), 0);
  int __attribute__((unused)) r = write(fd, space, z);
}

void API xapi_backtrace_to(int fd)
{
  char space[4096];
  size_t z = xapi_trace_full(space, sizeof(space), 0);
  int __attribute__((unused)) r = write(fd, space, z);
}

void API xapi_pithytrace()
{
  xapi_pithytrace_to(2);
}

void API xapi_fulltrace()
{
  xapi_fulltrace_to(2);
}

void API xapi_backtrace()
{
  xapi_backtrace_to(2);
}

size_t API xapi_trace_info(const char * restrict name, char * const restrict dst, const size_t sz)
{
  int a1 = 0;
  int a2 = -1; // start index for the subsequence
  int b2 = -1; // end index for the subsequence
  int b1 = g_calltree->frames.l - 1;

  int x;
  if((x = calltree_locate_substack(g_calltree, a1, b1)) != -1)
  {
    a2 = g_calltree->frames.v[x].parent_index + 1;
    b2 = x;
  }

  for(x = a1; x <= b1; x++)
  {
    if(x == a2)
      x = b2 + 1;

    int y;
    for(y = 0; y < g_calltree->frames.v[x].infos.l; y++)
    {
      if(memncmp(
          g_calltree->frames.v[x].infos.v[y].ks
        , g_calltree->frames.v[x].infos.v[y].kl
        , name
        , strlen(name)) == 0)
      {
        size_t len = MIN(g_calltree->frames.v[x].infos.v[y].vl, sz - 1);
        if(dst)
        {
          memcpy(dst, g_calltree->frames.v[x].infos.v[y].vs, len);
          dst[len] = 0;
        }
        return len;
      }
    }
  }

  if(dst)
    dst[0] = 0;
  return 0;
}
