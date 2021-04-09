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

#ifndef FABD_PATTERN_H
#define FABD_PATTERN_H

/*

PATTERN TYPES

match pattern - match nodes starting from a base
 * used in rules, leftmost (match) pattern
 * does not support NODESET_SELF '.'

generate pattern - create nodes, starting from a base
 * used in rules, middle (generate) pattern

lookup pattern - find nodes, disambiguating via prefix
 * used in module.bam for use, import, and require references  PATTERN_LOOKUP_ONE - one matching node per frag
 * used in module.bam to specify variants                      render only
 * used in selector pattern for node lookup                    matches zero or more

reference pattern - find nodes by left -> right descent (class/alternation not permitted)
 * used in module.bam for formula-binding pattern

*/

#include "xapi.h"
#include "types.h"

#include "valyria/chain.h"
#include "valyria/llist.h"

#include "yyutil/parser.h"

struct attrs16;
struct narrator;      // narrator.h

struct pattern_render_context;
struct pattern_generate_context;
struct pattern_match_context;

#define PATTERN_SEGMENT_TYPE_OPT 0x00ff

// PATTERN segment types
#define PATTERN_SEGMENT_TYPE_TABLE                    \
  /* common scalars */                                \
  ATTR_DEF(PATTERN_CHARACTER       , 0x0010)          \
  ATTR_DEF(PATTERN_STEM            , 0x0011) /* % */  \
  ATTR_DEF(PATTERN_VARIANTS        , 0x0012) /* ? */  \
  ATTR_DEF(PATTERN_WORD            , 0x0013)          \
  ATTR_DEF(PATTERN_STAR            , 0x0014)          \
  /* common aggregates */                             \
  ATTR_DEF(PATTERN_ALTERNATION     , 0x0015)          \
  ATTR_DEF(PATTERN_CLASS           , 0x0016)          \
  ATTR_DEF(PATTERN_RANGE           , 0x0017)          \
  /* match-only scalars */                            \
  ATTR_DEF(PATTERN_GROUP           , 0x0018)          \
  /* generate scalars */                              \
  ATTR_DEF(PATTERN_REPLACEMENT     , 0x0019)          \


#undef ATTR_DEF
#define ATTR_DEF(x, y) x = UINT16_C(y),
typedef enum pattern_segment_type {
PATTERN_SEGMENT_TYPE_TABLE
} pattern_segment_type;

extern struct attrs16 * pattern_segment_type_attrs;

/* graph:axis+qualifiers */

// graph
#define PATTERN_GRAPH_OPT  0x000f
#define PATTERN_GRAPH_TABLE                                     \
  ATTR_NAME_DEF(PATTERN_GRAPH_FS      , "fs"          , 0x0001) \
  ATTR_NAME_DEF(PATTERN_GRAPH_DIRS    , "directories" , 0x0002) \
  ATTR_NAME_DEF(PATTERN_GRAPH_REQUIRES, "requires"    , 0x0004) \
  ATTR_NAME_DEF(PATTERN_GRAPH_USES    , "uses"        , 0x0005) \

#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_graph {
PATTERN_GRAPH_TABLE
} pattern_graph;

extern struct attrs16 * pattern_graph_attrs;

// axis
#define PATTERN_AXIS_OPT  0x000f
#define PATTERN_AXIS_TABLE                                                                                                      \
  /* contextual */                                                                                                              \
  ATTR_NAME_DEF(PATTERN_AXIS_UP               , "up"            , 0x0005) /* parent, .. : nodes up 1 level from current fsent */ \
  ATTR_NAME_DEF(PATTERN_AXIS_ABOVE            , "above"         , 0x0006) /* nodes above the current node, any level */         \
  ATTR_NAME_DEF(PATTERN_AXIS_SELF_OR_ABOVE    , "self-or-above" , 0x0007) /* current and nodes above, any level */              \
  ATTR_NAME_DEF(PATTERN_AXIS_DOWN             , "down"          , 0x0008) /* child : nodes down 1 level from current fsent */    \
  ATTR_NAME_DEF(PATTERN_AXIS_BELOW            , "below"         , 0x0009) /* nodes below the current node, any level */         \
  ATTR_NAME_DEF(PATTERN_AXIS_SELF_OR_BELOW    , "self-or-below" , 0x000a) /* ** : current and nodes below, any level */         \

#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_axis {
PATTERN_AXIS_TABLE
} pattern_axis;

extern struct attrs16 * pattern_axis_attrs;

#define PATTERN_NODESET_OPT 0x000f
#define PATTERN_NODESET_TABLE                                                                                \
  /* non-contextual */                                                                                       \
  ATTR_NAME_DEF(PATTERN_NODESET_SELF     , "self"      , 0x0001) /* . : the current context fsent */          \
  ATTR_NAME_DEF(PATTERN_NODESET_MATCH    , "match"     , 0x0002) /* matched node, (generate pattern) */      \
  ATTR_NAME_DEF(PATTERN_NODESET_MATCHDIR , "match-dir" , 0x0003) /* $^D directory of the matched node  */    \
  ATTR_NAME_DEF(PATTERN_NODESET_SHADOW   , "shadow"    , 0x0004) /* root of the shadow fs */                 \

#undef ATTR_NAME_DEF
#define ATTR_NAME_DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_nodeset {
PATTERN_NODESET_TABLE
} pattern_nodeset;

extern struct attrs16 * pattern_nodeset_attrs;

#define PATTERN_QUALIFIER_TYPE_OPT 0x000f

// qualifier
#define PATTERN_QUALIFIER_TYPE_TABLE            \
  ATTR_DEF(PATTERN_QUALIFIER_TYPE_AND , 0x0001) \
  ATTR_DEF(PATTERN_QUALIFIER_TYPE_NOT , 0x0002) \

#undef ATTR_DEF
#define ATTR_DEF(x, y) x = UINT16_C(y),
typedef enum pattern_qualifier_type {
PATTERN_QUALIFIER_TYPE_TABLE
} pattern_qualifier_type;

extern struct attrs16 * pattern_qualifier_type_attrs;

#define PATTERN_REPLACEMENT_TYPE_OPT 0x000f

// replacement
#define PATTERN_REPLACEMENT_TYPE_TABLE                 \
  ATTR_DEF(PATTERN_REPLACEMENT_TYPE_NAME     , 0x0001) \
  ATTR_DEF(PATTERN_REPLACEMENT_TYPE_NUM      , 0x0002) \
  ATTR_DEF(PATTERN_REPLACEMENT_TYPE_VARIANT  , 0x0004) \


#undef ATTR_DEF
#define ATTR_DEF(x, y) x = UINT16_C(y),
typedef enum pattern_replacement_type {
PATTERN_REPLACEMENT_TYPE_TABLE
} pattern_replacement_type;

extern struct attrs16 * pattern_replacement_type_attrs;

union pattern_segment;
typedef struct pattern_segment_vtable {
  pattern_segment_type type;

  xapi __attribute__((nonnull)) (*say)(
      const union pattern_segment * restrict seg
    , struct narrator * restrict N
  );

  xapi __attribute__((nonnull)) (*match)(
/* switch these */
      struct pattern_match_context * ctx
    , const union pattern_segment * seg
  );

  xapi __attribute__((nonnull)) (*render)(
      const union pattern_segment * restrict seg
    , struct pattern_render_context * restrict ctx
  );

  xapi __attribute__((nonnull)) (*generate)(
      const union pattern_segment * restrict seg
    , struct pattern_generate_context * restrict ctx
  );

  void __attribute__((nonnull)) (*destroy)(
    union pattern_segment * restrict n
  );

  int (*cmp)(const union pattern_segment * A, const union pattern_segment *B);
} pattern_segment_vtable;

/* list of sections */
typedef struct pattern {
  yyu_location loc;           /* pattern location (cumulative) */
  char * fname;
  struct pattern_section * section_head;

  llist lln;
} pattern;

/* list of segments */
typedef struct pattern_segments {
  yyu_location loc;
  chain chn;                  /* in list-of-segments list */
  pattern_qualifier_type qualifier_type;
  union pattern_segment * segment_head;
} pattern_segments;

typedef struct pattern_section {
  yyu_location loc;            /* node location (cumulative) */
  chain chn;                   /* in section list */
  pattern_nodeset nodeset;  // << GENERATE ONLY
  pattern_graph graph;
  pattern_axis axis;
  pattern_segments * qualifiers_head;
} pattern_section;

/* segment types */
#define PATTERN_SEGMENT_BASE struct {                             \
  pattern_segment_type type;                                      \
  const pattern_segment_vtable * vtab;                            \
  yyu_location loc;            /* node location (cumulative) */   \
  chain chn;                   /* in segment list */              \
}

typedef struct pattern_alternation {
  PATTERN_SEGMENT_BASE;
  pattern_segments * segments_head; // no qualifiers
  bool epsilon;
} pattern_alternation;

typedef struct pattern_byte {
  PATTERN_SEGMENT_BASE;
  uint8_t code;
} pattern_byte;

typedef struct pattern_class {
  PATTERN_SEGMENT_BASE;
  pattern_segments * segments_head; // no qualifiers
  bool invert;
} pattern_class;

typedef struct pattern_range {
  PATTERN_SEGMENT_BASE;
  uint8_t start;
  uint8_t end;
} pattern_range;

typedef struct pattern_word {
  PATTERN_SEGMENT_BASE;
  char * text;
  uint16_t len;
} pattern_word;

typedef struct pattern_group {
  PATTERN_SEGMENT_BASE;
  pattern_segments * segments_head;  // qualifiers, trailing
  uint16_t num;
  char * name;
  uint8_t name_len;
} pattern_group;

typedef struct pattern_star {
  PATTERN_SEGMENT_BASE;
} pattern_star;

typedef struct pattern_variants {
  PATTERN_SEGMENT_BASE;
} pattern_variants;

typedef struct pattern_replacement {
  PATTERN_SEGMENT_BASE;
  pattern_replacement_type replacement_type;
  union {
    uint16_t num;         // NUM
    struct {              // NAME
      char * name;
      uint8_t name_len;
    };
    struct {
      char text[64];    // VARIANT - null-separated tag specs
      uint8_t len;
    } tags;
  };
} pattern_replacement;

typedef union pattern_segment {
  PATTERN_SEGMENT_BASE;

  // common
  pattern_alternation alternation;
  pattern_byte byte;
  pattern_class class;
  pattern_range range;
  pattern_variants variants;
  pattern_word word;

  // match only
  pattern_group group;
  pattern_star star;

  // generate only
  pattern_replacement replacement;
} pattern_segment;

/// pattern_free
//
// SUMMARY
//  free a pattern with free semantics
//
void pattern_free(pattern * const restrict);

void pattern_segments_list_free(pattern_segments * const restrict);
void pattern_segments_free(pattern_segments * const restrict);

// void pattern_section_list_free(pattern_section * const restrict);
void pattern_section_free(pattern_section * const restrict);

void pattern_segment_list_free(pattern_segment * const restrict);
void pattern_segment_free(pattern_segment * const restrict);

/// pattern_say
//
// SUMMARY
//  write a normalized representation of an ffn to a narrator
//
xapi pattern_say(const pattern * restrict ffn, struct narrator * restrict N)
  __attribute__((nonnull));

typedef struct pattern_match_group {
  const char * start; // points into node->name.name
  uint16_t len;
} pattern_match_group;

/// pattern_segments_say
//
// SUMMARY
//
// PARAMETERS
//  head  -
//  item  -
//  N     -
//  sep   -
//  first -
//
xapi pattern_segment_chain_say(const pattern_segment * restrict head, struct narrator * restrict N)
  __attribute__((nonnull));

int pattern_cmp(const pattern * A, const pattern * B);

#endif
