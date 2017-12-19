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

#ifndef FABD_FF_NODE_H
#define FABD_FF_NODE_H

/*

SUMMARY
 fab-file node

*/

#include "xapi.h"
#include "types.h"

#include "yyutil/parser.h"

struct vertex;

struct ff_file;
struct ff_node;
struct module;
struct narrator;      // narrator.h
struct node;

union ff_node_pattern_part;

/// FFN types
#define FFN_TYPE_TABLE	  		                         \
  /* scalars */                                        \
  FFN_TYPE_DEF(FFN_CHAR               , 0x10)          \
  FFN_TYPE_DEF(FFN_DIRSEP             , 0x11) /* / */  \
  FFN_TYPE_DEF(FFN_STEM               , 0x12) /* % */  \
  FFN_TYPE_DEF(FFN_USRVAR             , 0x13)          \
  FFN_TYPE_DEF(FFN_VARIANT            , 0x14) /* ? */  \
  FFN_TYPE_DEF(FFN_WORD               , 0x15)          \
  /* aggregates */                                     \
	FFN_TYPE_DEF(FFN_RULE               , 0x21)          \
  FFN_TYPE_DEF(FFN_ALTERNATION        , 0x22)          \
  FFN_TYPE_DEF(FFN_ARTIFACT           , 0x23)          \
  FFN_TYPE_DEF(FFN_ARTIFACT_PATTERN   , 0x24)          \
  FFN_TYPE_DEF(FFN_CLASS              , 0x25)          \
  FFN_TYPE_DEF(FFN_PATTERN            , 0x26)          \
  FFN_TYPE_DEF(FFN_PATTERNS           , 0x27)          \
  FFN_TYPE_DEF(FFN_RANGE              , 0x28)          \
  FFN_TYPE_DEF(FFN_REQUIRE            , 0x29)          \
  FFN_TYPE_DEF(FFN_STRING             , 0x2a)          \
  FFN_TYPE_DEF(FFN_STRINGS            , 0x2b)          \
  FFN_TYPE_DEF(FFN_USE                , 0x2c)          \
  FFN_TYPE_DEF(FFN_VARDEF             , 0x2d)          \
  FFN_TYPE_DEF(FFN_VARNAME            , 0x2e)          \

typedef enum ffn_type {
  FFN_TYPE_RANGE_BEFORE = 0,

#define FFN_TYPE_DEF(t, x) t = x,
FFN_TYPE_TABLE
#undef FFN_TYPE_DEF

  FFN_TYPE_RANGE_AFTER
} ffn_type;

#define FFN_TYPE_SCALAR     0x10
#define FFN_TYPE_AGGREGATE  0x20

/// ffn_type_name
//
// SUMMARY
//  get the string name for an ffn_type
//
const char *ffn_type_name(ffn_type type);

/// FFN attrs
#define FFN_ATTRS_TABLE                    \
  FFN_ATTR_DEF(FFN_INVERT          , 0x01) \
  FFN_ATTR_DEF(FFN_VARIANT_GROUP   , 0x02) \

enum {
#define FFN_ATTR_DEF(a, b)  a = UINT16_C(b),
FFN_ATTRS_TABLE
#undef FFN_ATTR_DEF
};

/// ffn_attr_byname
//
// SUMMARY
//  get an ffn attr by name
//
uint16_t ffn_attr_byname(const char * name, size_t namel)
  __attribute__((nonnull));

/// ffn_attrs_say
//
// SUMMARY
//  write the names of ffn attrs to a narrator
//
xapi ffn_attrs_say(uint16_t attrs, struct narrator * restrict N)
  __attribute__((nonnull));

typedef struct
{
	yyu_location;
	const struct ff_file * ff;
} ff_loc;

typedef struct ff_node
{
	ffn_type type;
  uint16_t attrs;
	ff_loc loc;			         // node location (cumulative)
	struct ff_node * next;	 // next sibling
  struct ff_node * tail;   // last sibling (only set on the first in a chain)
  struct ff_node * prev;   // previous sibling
  struct ff_node * parent; // containing aggregate node
} ff_node;

///  ffn_mknode
//
// SUMMARY
//  create a new ff_node
//
// PARAMETERS
//  n    - new node returned here
//  loc  - location
//  type - node type
//
xapi ffn_mknode(ff_node ** restrict n, const struct yyu_location * restrict loc, ffn_type type, ...)
	__attribute__((nonnull(1, 2)));

/// ffn_append
//
// SUMMARY
//  attach a node to a chain as the last node
//
// PARAMETERS
//  h - head of the chain
//  n - node to append
//
ff_node* ffn_append(ff_node * restrict a, ff_node * const restrict b);

/// ffn_free
//
// SUMMARY
//  free a ff_node with free semantics
//
void ffn_free(ff_node * const restrict);

/// ffn_xfreenode
//
// SUMMARY
//  free a ff_node with xfree semantics
//
void ffn_xfree(ff_node ** const restrict)
	__attribute__((nonnull));

/// ffn_say_normal
//
// SUMMARY
//  write a normalized representation of an ffn to a narrator
//
xapi ffn_say_normal(const ff_node * restrict ffn, struct narrator * restrict N)
  __attribute__((nonnull));

xapi ffn_say_tree(const ff_node * restrict ffn, struct narrator * restrict N)
  __attribute__((nonnull));

/// ffn_pattern_segment_say_normal
//
// SUMMARY
//  write an ffn segment to a narrator in its normalized form
//
// PARAMETERS
//  first - first node in the segment
//  end   - stopping node
//  N     - narrator to write to
//
xapi ffn_segment_say_normal(const ff_node * restrict first, const ff_node * restrict end, struct narrator * restrict N)
  __attribute__((nonnull));

/// ffn_semantic_error
//
// SUMMARY
//  throw an FF error, with infos for the ref string and ff location
//
// PARAMETERS
//  error      - FF error type
//  ffn_ref    - ff node with the reference
//  dirnode    - module node for NOMODULE
//  (ref)      - ref string being resolved
//  (matching) - matching vertices for AMBIGREF
//
xapi ffn_semantic_error(
    xapi error
  , const ff_node * restrict ffn_ref
  , const char * restrict ref
  , const struct node * restrict dirnode
  , struct vertex * restrict matching[2]
);

#endif
