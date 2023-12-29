/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria/traverse.h"

struct narrator;
struct attrs32;
struct command;

typedef struct request {
  llist commands;           // struct command
  bool has_last_command;
} request;

#define TRAVERSE_TYPE_OPT 0x3
#define TRAVERSE_TYPE_TABLE                                                                                             \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_UP    , "up"    , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_UP)    /* traverse upwards */    \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_DOWN  , "down"  , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_DOWN)  /* traverse downwards */  \

typedef enum traverse_type {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_TYPE_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_type;

#define TRAVERSE_ORDER_OPT 0x7
#define TRAVERSE_ORDER_TABLE                                                                                          \
  TRAVERSE_ATTR_DEF(TRAVERSE_ORDER_UP    , "preorder"   , TRAVERSE_ORDER_OPT , MORIA_TRAVERSE_PRE)   /* preorder */   \
  TRAVERSE_ATTR_DEF(TRAVERSE_ORDER_DOWN  , "postorder"  , TRAVERSE_ORDER_OPT , MORIA_TRAVERSE_POST)  /* postorder */  \

typedef enum traverse_order {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_ORDER_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_order;

#define COMMAND_TYPE_OPT 0xff
#define COMMAND_TYPE_TABLE                                                                                           \
  DEF(COMMAND_GLOBAL_STATS_READ   , "global-stats-read"  , 0x01) /* retrieve global stats */                         \
  DEF(COMMAND_GLOBAL_STATS_RESET  , "global-stats-reset" , 0x02) /* reset global stats, retrieve previous values */  \
  DEF(COMMAND_STATS_READ          , "stats-read"         , 0x03) /* retrieve node stats */                           \
  DEF(COMMAND_STATS_RESET         , "stats-reset"        , 0x04) /* reset node stats, retrieve previous values */    \
  DEF(COMMAND_CONFIG_DESCRIBE     , "config-describe"    , 0x05) /* read effective configuration */                  \
  DEF(COMMAND_VARS_DESCRIBE       , "vars-describe"      , 0x06) /* read effective vars */                           \
  DEF(COMMAND_ENVS_DESCRIBE       , "envs-describe"      , 0x07) /* read effective envs */                           \
  DEF(COMMAND_RMA_DESCRIBE        , "rule-describe"      , 0x08) /* describe a rule */                               \
  DEF(COMMAND_RULES_DESCRIBE      , "rules-describe"     , 0x09) /* read effective rules */                          \
  DEF(COMMAND_FSTREE_DESCRIBE     , "fstree-describe"    , 0x0a) /* read effective fstree */                         \
  DEF(COMMAND_FILESYSTEM_DESCRIBE , "filesystem-describe", 0x0b) /* read effective filesystem */                     \
  DEF(COMMAND_SELECT              , "select"             , 0x0c) /* add nodes to the selection */                    \
  DEF(COMMAND_RESET_SELECTION     , "reset-selection"    , 0x0d) /* reset the selection */                           \
  DEF(COMMAND_LIST                , "list"               , 0x0e) /* get minimal info about each selected node */     \
  DEF(COMMAND_DESCRIBE            , "describe"           , 0x0f) /* describe each of the nodes in the selection */   \
  DEF(COMMAND_INVALIDATE          , "invalidate"         , 0x10) /* invalidate the nodes in the selection */         \
  DEF(COMMAND_GLOBAL_INVALIDATE   , "global-invalidate"  , 0x11) /* generation-based global node invalidation */     \
  DEF(COMMAND_METADATA            , "metadata"           , 0x12) /* retrieve project metadata */                     \
  DEF(COMMAND_GOALS               , "goals"              , 0x13) /* set the goal targets */                          \
  DEF(COMMAND_RECONCILE           , "reconcile"          , 0x14) /* reload everything */                             \
  DEF(COMMAND_CONTEXT             , "context"            , 0x15) /* context for interpreting subsequent commands */  \
  /* needs reconcile */                                                                                              \
  DEF(COMMAND_RUN                 , "run"                , 0x16) /* pursue goals */                                  \
  DEF(COMMAND_AUTORUN             , "autorun"            , 0x17) /* pursue goals */                                  \

typedef enum command_type {
#undef DEF
#define DEF(x, s, y) x = y,
COMMAND_TYPE_TABLE
} command_type;

extern struct attrs32 * command_type_attrs;
extern struct attrs32 * traverse_type_attrs;
extern struct attrs32 * traverse_order_attrs;

typedef struct command {
  llist lln;

  union {
    uint32_t attrs;       // bitwise combo of BAM_REQUEST_*
    struct {
      command_type type:8;
    };
  };

  union {
    struct selector * selector;   // SELECT
    struct {                      // GOALS
      bool reconcile;
      bool build;
      bool script;
      struct selector * target_direct;
      struct selector * target_transitive;
    } goals;
    struct {
      bool special_only;
      bool no_special;
    } stats;
    struct {
      uint64_t module_id;
    } context;
  };
} command;

void request_init(request * restrict req)
  __attribute__((nonnull));

void request_destroy(request * restrict req)
  __attribute__((nonnull));

void request_say(request * restrict req, struct narrator * restrict N)
  __attribute__((nonnull));

void request_command_alloc(request * restrict req, struct command ** cmdp)
  __attribute__((nonnull));

void request_cleanup(void);
