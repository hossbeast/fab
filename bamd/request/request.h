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
  DEF(COMMAND_SELECT              , "select"             , 0x05) /* add nodes to the selection */                    \
  DEF(COMMAND_RESET_SELECTION     , "reset-selection"    , 0x06) /* reset the selection */                           \
  DEF(COMMAND_TREE_LIST           , "tree-list"          , 0x07) /* implementation for the tree command */           \
  DEF(COMMAND_LIST                , "list"               , 0x08) /* */           \
  DEF(COMMAND_IDENTIFY            , "identify"           , 0x09) /* one-line info string for selected nodes */       \
  DEF(COMMAND_DESCRIBE            , "describe"           , 0x0a) /* structured info document for selected nodes */   \
  DEF(COMMAND_TELL                , "tell"               , 0x0b) /* */           \
  DEF(COMMAND_INVALIDATE          , "invalidate"         , 0x0c) /* invalidate the nodes in the selection */         \
  DEF(COMMAND_INVALIDATE_PRIMARY  , "invalidate-primary" , 0x0d) /* generation-based invalidation of primary nodes */\
  DEF(COMMAND_GOALS               , "goals"              , 0x0e) /* set the goal targets */                          \
  DEF(COMMAND_RECONCILE           , "reconcile"          , 0x0f) /* reload everything */                             \
  DEF(COMMAND_CONTEXT             , "context"            , 0x10) /* set context for interpreting subsequent commands */  \
  DEF(COMMAND_EVENTSUB            , "eventsub"           , 0x11) /* subscribe to events */                           \
  DEF(COMMAND_PLUGIN_READY        , "plugin-ready"       , 0x12) /* plugin is ready */                               \
  /* needs reconcile */                                                                                              \
  DEF(COMMAND_RUN                 , "run"                , 0x1a) /* pursue goals */                                  \
  DEF(COMMAND_AUTORUN             , "autorun"            , 0x1b) /* pursue goals */                                  \

typedef enum command_type {
#undef DEF
#define DEF(x, s, y) x = y,
COMMAND_TYPE_TABLE
} command_type;

extern struct attrs32 *command_type_attrs;
extern struct attrs32 *traverse_type_attrs;
extern struct attrs32 *traverse_order_attrs;

typedef struct command {
  llist lln;
  command_type type;

  union {
    uint32_t mask;                // EVENTSUB
    struct selector * selector;   // SELECT
    struct {
      bool enabled;
    } autorun;
    struct {                      // GOALS
      bool reconcile;
      bool build;
      uint64_t token;
      char label[128];
      uint16_t label_len;
      struct selector * target_direct;
      struct selector * target_transitive;
    } goals;
    struct {
      bool special_only;
      bool no_special;
      bool no_stats;
    } describe;
    struct {
      bool special_only;
      bool no_special;
    } stats;
    struct {
      char module_id[40];
      uint16_t module_id_len;
    } context;
  };
} command;

void request_init(request * restrict req)
  __attribute__((nonnull));

void request_destroy(request * restrict req)
  __attribute__((nonnull));

void request_say(request * restrict req, struct narrator * restrict N)
  __attribute__((nonnull));

void command_say(command * restrict cmd, struct narrator * restrict N)
  __attribute__((nonnull));

void command_show(command * restrict cmd)
  __attribute__((nonnull));

struct command *request_command_alloc(request * restrict req)
  __attribute__((nonnull));

void request_command_free(command *cmd);

void request_cleanup(void);
