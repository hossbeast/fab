/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_FF_NODE_PATTERN_H
#define FABD_FF_NODE_PATTERN_H

#include "ff_node.h"

#include "ff_node_alternation.h"
#include "ff_node_char.h"
#include "ff_node_class.h"
#include "ff_node_dirsep.h"
#include "ff_node_stem.h"
#include "ff_node_usrvar.h"
#include "ff_node_variant.h"
#include "ff_node_word.h"

typedef union ff_node_pattern_part
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_alternation alternation;
  ff_node_char character;
  ff_node_class class;
  ff_node_dirsep dirsep;
  ff_node_stem stem;
  ff_node_usrvar usrvar;
  ff_node_variant variant;
  ff_node_word word;
} ff_node_pattern_part;

typedef struct ff_node_pattern
{
  union {
    ff_node;
    ff_node base;
  };

  ff_node_pattern_part * chain;
} ff_node_pattern;

/// ffn_bydir_context
//
//
//
typedef struct ffn_bydir_context
{
  const ff_node_pattern_part * first;  // (returns) first node in the by-directories segment, or null
  const ff_node_pattern_part * stop;   // (returns) last node in the by-directories segment
  const ff_node_pattern_part * state;  // opaque
} ffn_bydir_context;

/// ffn_bydir_walk
//
//
//
typedef struct ffn_bydir_walk
{
  ffn_bydir_context * context;
  const ff_node_pattern_part * ffn;
  struct ffn_bydir_walk * outer;
} ffn_bydir_walk;

/// ffn_pattern_bydir_ltr_init
//
// SUMMARY
//  initialize a walk of ff_nodes by directory segments in left to right order
//
// PARAMETERS
//  [start] - starting node
//  ctx     - dynamic context
//
void ffn_pattern_bydir_ltr_init(const ff_node_pattern_part * restrict start, ffn_bydir_context * restrict ctx)
  __attribute__((nonnull(2)));

/// ffn_pattern_bydir_ltr
//
// SUMMARY
//  get the next segment in an ltr ff_nodes walk
//
// PARAMETERS
//  first - (returns) first node in the next segment, or null
//  last  - (returns) last node in the next segment
//  state - opaque
//
void ffn_pattern_bydir_ltr(ffn_bydir_context * restrict ctx)
  __attribute__((nonnull));

/// ffn_pattern_bydir_rtl_iniit
//
// SUMMARY
//  as for ffn_pattern_bydir_ltr, but walk directory segments right to left
//
void ffn_pattern_bydir_rtl_init(const ff_node_pattern_part * restrict start, ffn_bydir_context * restrict ctx)
  __attribute__((nonnull(2)));

/// ffn_pattern_bydir_rtl
//
// SUMMARY
//  as for ffn_pattern_bydir_ltr, but walk directory segments right to left
//
void ffn_pattern_bydir_rtl(ffn_bydir_context * restrict ctx)
  __attribute__((nonnull));

/// ffn_pattern_bydir_strings_rtl_init
//
// SUMMARY
//  initialize a walk of ff_nodes by directory segments in right to left order
//
xapi ffn_pattern_bydir_strings_rtl_init(
    const ff_node_pattern_part * restrict start
  , ffn_bydir_context * restrict ctx
  , char * restrict tmp
  , size_t tmpsz
  , const char ** restrict segment
  , uint16_t * restrict segment_len
)
  __attribute__((nonnull));

/// ffn_string_bydir_rtl
//
// SUMMARY
//  get the next segment in an ff_nodes walk rendered to a string
//
// PARAMETERS
//  ctx   - ff_nodes walk context
//  tmp   - temp space to render the segment to, if necessary
//  tmpsz - size of the temp space
//  str   - (returns) pointer to the rendered string
//
xapi ffn_pattern_bydir_strings_rtl(
    ffn_bydir_context * restrict ctx
  , char * restrict tmp
  , size_t tmpsz
  , const char ** restrict segment
  , uint16_t * restrict segment_len
)
  __attribute__((nonnull));

void ffn_bydir_rtl_setup(ffn_bydir_context * restrict ctx, const ff_node_pattern_part * restrict start)
  __attribute__((nonnull));

#endif
