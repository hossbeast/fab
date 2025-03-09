/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
pattern syntax elements

                      initial global sysvar nodesets
                       |   initial module sysvar nodesets
                       |   |   initial absolute-root
                       |   |   |   initial absolute-shadow
                       |   |   |   |   initial relative
                       |   |   |   |   |   alternations
                       |   |   |   |   |   |   classes
                       |   |   |   |   |   |   |   groups
                       |   |   |   |   |   |   |   |   type-qualifiers
                       |   |   |   |   |   |   |   |   |   variables
                       |   |   |   |   |   |   |   |   |   |  segment sysvars
 attachment_pattern   [ ] [ ] [ ] [x] [ ] [ ] [ ] [ ] [ ] [x] [x]
 generate_pattern     [x] [x] [x] [ ] [x] [x] [x] [ ] [ ] [x] [x]
 include_pattern      [x] [ ] [x] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
 lookup_pattern       [x] [x] [x] [x] [x] [ ] [ ] [ ] [x] [x] [x]
 match_pattern        [ ] [ ] [x] [ ] [x] [x] [x] [ ] [ ] [x] [x]
 path_pattern         [ ] [ ] [x] [x] [x] [ ] [ ] [ ] [x] [x] [x]
 search_pattern       [x] [x] [x] [x] [x] [x] [x] [x] [x] [x] [x]

search pattern - find existing nodes
 * passed to pattern_search()
 * used in rules, lhs pattern
 * left-to-right

generate pattern - create nodes in the root tree
 * passed to pattern_generate()
 * used in rules, rhs pattern
 * left-to-right

attachment pattern - create a node in the shadow tree
 * pass to pattern_attachment_generate()
 * used in bind rules, rhs pattern (the attachment)
 * matches 0-1 nodes

include pattern - create a node in the root tree
 * pass to pattern_include_generate()
 * filesystem.fstree
 * generates 1 node

match pattern - test if a node matches
 * pass to pattern_match()
 * filesystem.exclude
 * right-to-left matching by sections
 * can match only directory nodes

lookup pattern - lookup nodes from a namespace, disambiguating via prefix
 * pass to namespace_resolve()
 * pass to pattern_resolve_bound()
 * sections processed right-to-left
 * used in use, import, and require refs
 * used in bind rules, lhs pattern (the targets)
 * used in rules, rightmost formula-binding pattern
 * used in plugins config, file.lookup.pattern
 * used in plugin args, lookup.pattern
 * used in formulas, file.lookup.pattern
 * used in formula args, lookup.pattern
 * used in selector lookup.pattern
 * can be bound, or not
 * matches 1

path pattern - subset of lookup pattern
 * pass to pattern_resolve_bound()
 * used in selector path.pattern
 * starts with ./, ../, /, or // (always bound, with relative lookup)
 * matches 1
*/

#include "types.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"

#include "yyutil/parser.h"

struct attrs16;
struct narrator;      // narrator.h
struct bacon_writer;
struct bacon_value;

struct pattern_generate_context;
struct pattern_search_context;
struct pattern_match_context;

#define PATTERN_SEGMENT_TYPE_OPT 0x00ff

// PATTERN segment types
#define PATTERN_SEGMENT_TYPE_TABLE                    \
  /* common scalars */                                \
  DEF(PATTERN_CHARACTER       , 0x0010)               \
  DEF(PATTERN_WORD            , 0x0013)               \
  DEF(PATTERN_STAR            , 0x0014) /* *   */     \
  /* common aggregates */                             \
  DEF(PATTERN_ALTERNATION     , 0x0015) /* { } */     \
  DEF(PATTERN_CLASS           , 0x0016)               \
  DEF(PATTERN_RANGE           , 0x0017) /* [ ] */     \
  DEF(PATTERN_VARIABLE        , 0x0018) /* $   */     \
  /* match-only scalars */                            \
  DEF(PATTERN_GROUP           , 0x0019) /* ( ) */     \
  /* scalar that matches on properties not label */   \
  DEF(PATTERN_QUALIFIER       , 0x0020) /* ( ) */     \


#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum pattern_segment_type {
PATTERN_SEGMENT_TYPE_TABLE
} pattern_segment_type;

extern struct attrs16 * pattern_segment_type_attrs;

/* graph:axis+qualifiers */

// graph
#define PATTERN_GRAPH_OPT  0x000f
#define PATTERN_GRAPH_TABLE                         \
  DEF(PATTERN_GRAPH_FS      , "fs"        , 0x0001) \
  DEF(PATTERN_GRAPH_DIRS    , "dirs"      , 0x0002) \

#undef DEF
#define DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_graph {
PATTERN_GRAPH_TABLE
} pattern_graph;

extern struct attrs16 * pattern_graph_attrs;

// axis
#define PATTERN_AXIS_OPT  0x000f
#define PATTERN_AXIS_TABLE                                                                                              \
  /* contextual */                                                                                                      \
  DEF(PATTERN_AXIS_UP               , "up"            , 0x0005) /* parent, .. : nodes up 1 level from current fsent */  \
  DEF(PATTERN_AXIS_ABOVE            , "above"         , 0x0006) /* nodes above the current node, any level */           \
  DEF(PATTERN_AXIS_SELF_OR_ABOVE    , "self-or-above" , 0x0007) /* current and nodes above, any level */                \
  DEF(PATTERN_AXIS_DOWN             , "down"          , 0x0009) /* child : nodes down 1 level from current fsent */     \
  DEF(PATTERN_AXIS_BELOW            , "below"         , 0x000a) /* nodes below the current node, any level */           \
  DEF(PATTERN_AXIS_SELF_OR_BELOW    , "self-or-below" , 0x000b) /* ** : current and directory nodes below, any level */ \


#undef DEF
#define DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_axis {
PATTERN_AXIS_TABLE
} pattern_axis;

extern struct attrs16 * pattern_axis_attrs;

#define PATTERN_NODESET_OPT 0x000f
#define PATTERN_NODESET_TABLE                                                                                 \
  /* non-contextual */                                                                                        \
  DEF(PATTERN_NODESET_SELF         , "self"          , 0x0001) /* . : the current context fsent */            \
  DEF(PATTERN_NODESET_SHADOW       , "shadow"        , 0x0002) /* // : root of the shadow fs */               \
  DEF(PATTERN_NODESET_ROOT         , "root"          , 0x0003) /* / : root of the actual fs */                \
  DEF(PATTERN_NODESET_ANCHOR_REL   , "bm_anchor_rel" , 0x0004) /* $^A : relative to context anchor dir */     \
  DEF(PATTERN_NODESET_BUILD_REL    , "bm_build_rel"  , 0x0005) /* $^D : relative to context module dir */     \
  DEF(PATTERN_NODESET_ANCHOR       , "bm_anchor"     , 0x0006) /* $bm_anchor : context module anchor dir */   \
  DEF(PATTERN_NODESET_BUILD        , "bm_build"      , 0x0007) /* $bm_builddir : context module build dir */  \
  DEF(PATTERN_NODESET_BASEDIR      , "bm_basedir"    , 0x0008) /* $bm_base : project base dir */              \
  DEF(PATTERN_NODESET_HOMEDIR      , "bm_homedir"    , 0x0009) /* $bm_homedir : user home dir */              \
  DEF(PATTERN_NODESET_IPCDIR       , "bm_ipcdir"     , 0x000a) /* $bm_ipcdir : context module dir */          \


#undef DEF
#define DEF(x, s, y) x = UINT16_C(y),
typedef enum pattern_nodeset {
PATTERN_NODESET_TABLE
} pattern_nodeset;

extern struct attrs16 * pattern_nodeset_attrs;

// qualifier
#define PATTERN_QUALIFIER_TYPE_OPT 0x00ff
#define PATTERN_QUALIFIER_TYPE_TABLE                                                                              \
  DEF(PATTERN_QUALIFIER_TYPE_OR            , 0x0001)    /*   matches when or or more qualifiers are satisfied */  \
  DEF(PATTERN_QUALIFIER_TYPE_AND           , 0x0002)    /* + matches when all qualifiers are satisfied */         \
  DEF(PATTERN_QUALIFIER_TYPE_NOT           , 0x0003)    /* ~ matches when no qualifiers are satisfied */          \
  DEF(PATTERN_QUALIFIER_TYPE_DIR           , 0x0004)    /* (/) matches directories */                             \
  DEF(PATTERN_QUALIFIER_TYPE_DIR_NONEMPTY  , 0x0005)    /* (F) matches non-empty directories */                   \
  DEF(PATTERN_QUALIFIER_TYPE_FILE          , 0x0006)    /* (.) matches regular files */                           \
  DEF(PATTERN_QUALIFIER_TYPE_SYMLINK       , 0x0007)    /* (@) matches symlinks */                                \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum pattern_qualifier_type {
PATTERN_QUALIFIER_TYPE_TABLE
} pattern_qualifier_type;

extern struct attrs16 *pattern_qualifier_type_attrs;

// variable
#define PATTERN_VARIABLE_TYPE_OPT 0x000f
#define PATTERN_VARIABLE_TYPE_TABLE                       \
  DEF(PATTERN_VARIABLE_TYPE_NAME      , 0x0001) /* named variables, including bm_foo sysvars */           \
  DEF(PATTERN_VARIABLE_TYPE_NUM       , 0x0002)           \

#undef DEF
#define DEF(x, y) x = UINT16_C(y),
typedef enum pattern_variable_type {
PATTERN_VARIABLE_TYPE_TABLE
} pattern_variable_type;

extern struct attrs16 * pattern_variable_type_attrs;

#define PATTERN_SEGMENT_SEARCH_RECURSIVE 0x01

union pattern_segment;
typedef struct pattern_segment_vtable {
  pattern_segment_type type;
  uint8_t flags;

  void __attribute__((nonnull(1, 3))) (*say)(
      const union pattern_segment * restrict seg
    , const union pattern_segment * restrict next
    , struct narrator * restrict N
  );

  void __attribute__((nonnull)) (*describe)(
      const union pattern_segment * restrict seg
    , struct bacon_writer * restrict builder_writer
  );

  void __attribute__((nonnull)) (*search)(
      const union pattern_segment * seg
    , struct pattern_search_context * ctx
  );

  size_t __attribute__((nonnull(1,4))) (*cat)(
      void * restrict dst
    , size_t sz
    , const struct bacon_value * restrict vars
    , const union pattern_segment * restrict seg
  );

  void __attribute__((nonnull)) (*generate)(
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
  yyu_location loc;                       /* pattern location (cumulative) */
  char *fname;
  uint16_t groups_count;                  /* number of parenthesized groups */
  uint16_t capture_groups_count;          /* number of parenthesized groups which are not non-capturing */
  llist sections;                         // struct pattern_section *section_head;

  /* a bound pattern is one where the left-most section is one of the following,
   *  nodeset-self    (./ -- relative to cwd)
   *  axis-up         (../ -- relative to cwd)
   *  nodeset-root    (/ -- relative to the root)
   *  nodeset-shadow  // -- relative to the shadow-root)
   */
  bool bound;

  llist lln;    // in list
  rbnode rbn;   // in set
} pattern;

/* thin wrappers used to ensure that top-level functions are only passed patterns
 * of the correct type */

typedef struct path_pattern {
  struct pattern pattern;
} path_pattern;

typedef struct search_pattern {
  struct pattern pattern;
} search_pattern;

typedef struct generate_pattern {
  struct pattern pattern;
} generate_pattern;

typedef struct lookup_pattern {
  struct pattern pattern;
} lookup_pattern;

typedef struct include_pattern {
  struct pattern pattern;
} include_pattern;

typedef struct attachment_pattern {
  struct pattern pattern;
} attachment_pattern;

typedef struct match_pattern {
  struct pattern pattern;
} match_pattern;

/* list of segments */
typedef struct pattern_segments {
  yyu_location loc;
  llist lln;                  /* in list-of-segments list */
  pattern_qualifier_type qualifier_type;
  uint32_t type_qualifiers_mask;
  llist list;                 // union pattern_segment *segment_head;
} pattern_segments;

typedef struct pattern_section {
  yyu_location loc;            /* node location (cumulative) */
  llist lln;                   /* in section list */
  pattern_nodeset nodeset;
  pattern_graph graph;
  pattern_axis axis;
  llist qualifiers;           // struct pattern_segments
  uint32_t type_qualifiers_mask;
} pattern_section;

/* segment types */
#define PATTERN_SEGMENT_BASE struct {                             \
  pattern_segment_type type;                                      \
  const pattern_segment_vtable * vtab;                            \
  yyu_location loc;            /* node location (cumulative) */   \
  llist lln;                   /* in segment list */              \
}

typedef struct pattern_alternation {
  PATTERN_SEGMENT_BASE;
  llist segments; // no qualifiers
  bool epsilon;
} pattern_alternation;

typedef struct pattern_byte {
  PATTERN_SEGMENT_BASE;
  uint8_t code;
} pattern_byte;

typedef struct pattern_class {
  PATTERN_SEGMENT_BASE;
  llist segments; // no qualifiers
  bool invert;
} pattern_class;

typedef struct pattern_range {
  PATTERN_SEGMENT_BASE;
  uint8_t start;
  uint8_t end;
} pattern_range;

typedef struct pattern_word {
  PATTERN_SEGMENT_BASE;
  char *text;
  uint16_t len;
} pattern_word;

typedef struct pattern_group {
  PATTERN_SEGMENT_BASE;
  llist segments; // qualifiers, trailing
  uint16_t group_num;
  uint16_t capture_group_num;
  char * name;
  uint8_t name_len;
  uint32_t type_qualifiers_mask;
} pattern_group;

typedef struct pattern_star {
  PATTERN_SEGMENT_BASE;
} pattern_star;

typedef struct pattern_variable {
  PATTERN_SEGMENT_BASE;
  pattern_variable_type variable_type;
  union {
    uint16_t num;         // NUM
    struct {              // NAME
      char * name;
      uint8_t name_len;
    };
  };
} pattern_variable;

typedef struct pattern_qualifier {
  PATTERN_SEGMENT_BASE;
  pattern_qualifier_type qualifier_type;
} pattern_qualifier;

typedef union pattern_segment {
  PATTERN_SEGMENT_BASE;

  // common
  pattern_alternation alternation;
  pattern_byte byte;
  pattern_class class;
  pattern_range range;
  pattern_word word;
  pattern_variable variable;
  pattern_qualifier qualifier;

  // match only
  pattern_group group;
  pattern_star star;
} pattern_segment;

/*
 * free a pattern with free semantics
 */
void pattern_free(pattern * const restrict);

void pattern_segments_list_free(llist * restrict list);
void pattern_segments_free(pattern_segments * const restrict);

void pattern_section_free(pattern_section * const restrict);

void pattern_segment_list_free(llist * restrict list)
  __attribute__((nonnull));

void pattern_segment_free(pattern_segment * const restrict);

int pattern_cmp(const pattern * A, const pattern * B);

void pattern_list_free(struct llist * restrict list)
  __attribute__((nonnull));

//
//
//

void pattern_segment_list_say(const llist * restrict list, struct narrator * restrict N)
  __attribute__((nonnull));

void pattern_say(const pattern * restrict ffn, struct narrator * restrict N)
  __attribute__((nonnull));

size_t pattern_cat(void * restrict dst, size_t sz, const pattern * restrict pat)
  __attribute__((nonnull));

void pattern_describe(const pattern * restrict ffn, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

void pattern_show(const pattern * restrict pattern)
  __attribute__((nonnull));

void pattern_writer_write(const pattern * restrict pat, struct bacon_writer * restrict writer)
  __attribute__((nonnull));
