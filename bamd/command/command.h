/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria/traverse.h"

struct narrator;
struct attrs32;
struct command;
struct bacon_writer;

#define COMMAND_TYPE_OPT 0xff
#define COMMAND_TYPE_TABLE                                                                                                            \
  DEF(COMMAND_GLOBAL_STATS_READ         , "global-stats-read"         , 0x01) /* retrieve global stats */                             \
  DEF(COMMAND_GLOBAL_STATS_RESET        , "global-stats-reset"        , 0x02) /* reset global stats, retrieve previous values */      \
  DEF(COMMAND_STATS_READ                , "stats-read"                , 0x03) /* retrieve node stats */                               \
  DEF(COMMAND_STATS_RESET               , "stats-reset"               , 0x04) /* reset node stats, retrieve previous values */        \
  DEF(COMMAND_SELECT                    , "select"                    , 0x05) /* add nodes to the selection */                        \
  DEF(COMMAND_RESET_SELECTION           , "reset-selection"           , 0x06) /* reset the selection */                               \
  DEF(COMMAND_TREE_LIST                 , "tree-list"                 , 0x07) /* implementation for the tree command */               \
  DEF(COMMAND_LIST                      , "list"                      , 0x08) /* */                                                   \
  DEF(COMMAND_IDENTIFY                  , "identify"                  , 0x09) /* one-line info string for selected nodes */           \
  DEF(COMMAND_DESCRIBE                  , "describe"                  , 0x0a) /* bacon structured document for selected nodes */      \
  DEF(COMMAND_TELL                      , "tell"                      , 0x0b) /* non structured describe */                           \
  DEF(COMMAND_INVALIDATE                , "invalidate"                , 0x0c) /* invalidate the nodes in the selection */             \
  DEF(COMMAND_MODULES_INVALIDATE_PRIMARY, "modules-invalidate-primary", 0x0d) /* invalidate primary in all modules */                 \
  DEF(COMMAND_GOALS                     , "goals"                     , 0x0e) /* set the goal targets */                              \
  DEF(COMMAND_RECONCILE                 , "reconcile"                 , 0x0f) /* reload everything */                                 \
  DEF(COMMAND_CONTEXT_MODULE            , "context-module"            , 0x10) /* set context for interpreting subsequent commands */  \
  DEF(COMMAND_EVENTSUB                  , "eventsub"                  , 0x11) /* subscribe to events */                               \
  DEF(COMMAND_PLUGIN_READY              , "plugin-ready"              , 0x12) /* plugin is ready */                                   \
  /* needs reconcile */                                                                                                               \
  DEF(COMMAND_RUN       , "run"                , 0x1a) /* pursue goals */                                                             \
  DEF(COMMAND_AUTORUN   , "autorun"            , 0x1b) /* pursue goals */                                                             \

typedef enum command_type {
#undef DEF
#define DEF(x, s, y) x = y,
COMMAND_TYPE_TABLE
} command_type;

extern struct attrs32 *command_type_attrs;

typedef struct command {
  llist lln;
  command_type type;

  union {
    uint32_t mask;                // EVENTSUB
    struct selector *selector;    // SELECT
    struct {
      bool enabled;
    } autorun;
    struct {                      // GOALS
      bool reconcile;
      bool build;
      uint64_t token;
      char label[128];
      uint16_t label_len;
      struct selector *target_direct;
      struct selector *target_transitive;
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
  };
} command;

void command_say(command * restrict cmd, struct narrator * restrict N)
  __attribute__((nonnull));

void command_show(command * restrict cmd)
  __attribute__((nonnull));

struct command *command_alloc(void)
  __attribute__((nonnull));

void command_free(command *cmd);
void command_free_list(llist * restrict list)
  __attribute__((nonnull));

void command_describe(command * const restrict cmd, struct bacon_writer * const restrict builder_writer)
  __attribute__((nonnull));

void command_cleanup(void);
