/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "common/attrs.h"

/*
 * These events contain either no message, or a smallish bacon-text message.
 */
#define BAMIPC_EVENT_BACON_TABLE                                                                              \
  /* freetext - relative path */                                                                              \
  DEF(BAMIPC_EVENT_NODE_STALE          , "node-stale"          , 0x00001)  /* node marked as stale */         \
  DEF(BAMIPC_EVENT_NODE_FRESH          , "node-fresh"          , 0x00002)  /* node marked as fresh */         \
  DEF(BAMIPC_EVENT_NODE_CREATE         , "node-create"         , 0x00004)  /* node created */                 \
  DEF(BAMIPC_EVENT_NODE_DELETE         , "node-delete"         , 0x00008)  /* node deleted */                 \
  /* none */                                                                                                  \
  DEF(BAMIPC_EVENT_GLOBAL_INVALIDATION , "global-invalidation" , 0x00010)  /* global node invalidation */     \
  DEF(BAMIPC_EVENT_GOALS_SET           , "goals-set"           , 0x00020)  /* goals set */                    \
  DEF(BAMIPC_EVENT_AUTORUN_SET         , "autorun-set"         , 0x00040)  /* autorun set */                  \
  /* freetext - old state -> new state */                                                                     \
  DEF(BAMIPC_EVENT_SYSTEM_STATE_CHANGE , "system-state-change" , 0x00080)  /* system state change */          \
  /* bacon */                                                                                                 \
  DEF(BAMIPC_EVENT_FORMULA_FORK        , "formula-fork"        , 0x00100)  /* formula execution - forked */   \
  DEF(BAMIPC_EVENT_FORMULA_WAIT        , "formula-wait"        , 0x00200)  /* formula execution - waited */   \
  DEF(BAMIPC_EVENT_FORMULA_REAP        , "formula-reap"        , 0x000002)  /* formula execution - reaped */  \
  /* bacon */                                                                                                 \
  DEF(BAMIPC_EVENT_BUILD_START         , "build-start"         , 0x00400)  /* build has started */            \
  DEF(BAMIPC_EVENT_BUILD_END           , "build-end"           , 0x00800)  /* build has completed */          \
  /* bacon */                                                                                                   \
  DEF(BAMIPC_EVENT_RECONCILE_START     , "reconcile-start"     , 0x00001000)  /* reconcile started */           \
  DEF(BAMIPC_EVENT_RECONCILE_END       , "reconcile-end"       , 0x00002000)  /* reconcile completed */         \
  /* bacon */                                                                                                   \
  DEF(BAMIPC_EVENT_RMA_START           , "rma-start"           , 0x00004000)  /* rma run started */             \
  DEF(BAMIPC_EVENT_RMA_END             , "rma-end"             , 0x00008000)  /* rma run completed */           \
  DEF(BAMIPC_EVENT_RMA_SEARCH          , "rma-search"          , 0x00010000)  /* rma search */                  \
  DEF(BAMIPC_EVENT_RMA_GENERATE        , "rma-generate"        , 0x00020000)  /* rma generate */                \
  DEF(BAMIPC_EVENT_RMA_RESOLVE         , "rma-resolve"         , 0x00040000)  /* rma resolve */                 \
  DEF(BAMIPC_EVENT_RMA_BIND            , "rma-bind"            , 0x00080000)  /* rma bind */                    \

/*
 * These events contain a message that can potentially be very large. The message is binary.
 */
#define BAMIPC_EVENT_BINARY_TABLE                                                                             \
  /* struct bam_build_slot_info */                                                                            \
  DEF(BAMIPC_EVENT_FORMULA_FORK_BIN   , "formula-fork-bin"     , 0x01000)  /* formula execution - forked */   \
  /* struct bam_build_slot_results */                                                                         \
  DEF(BAMIPC_EVENT_FORMULA_WAIT_BIN   , "formula-wait-bin"     , 0x02000)  /* formula execution - waited */   \
  DEF(BAMIPC_EVENT_FORMULA_REAP_BIN   , "formula-reap-bin"     , 0x02000)  /* formula execution - reaped */  \
  /* unprocessed binary output from the program */                                                            \
  DEF(BAMIPC_EVENT_FORMULA_STDOUT_BIN , "formula-stdout-bin"   , 0x04000)  /* formula execution - stdout */   \
  DEF(BAMIPC_EVENT_FORMULA_STDERR_BIN , "formula-stderr-bin"   , 0x08000)  /* formula execution - stderr */   \
  DEF(BAMIPC_EVENT_FORMULA_AUXOUT_BIN , "formula-auxout-bin"   , 0x10000)  /* formula execution - auxout */   \

typedef enum bamipc_event_type {
#undef DEF
#define DEF(x, s, y) x = y,
BAMIPC_EVENT_BACON_TABLE
BAMIPC_EVENT_BINARY_TABLE
} bamipc_event_type;

/* convert a single event type to a string */
extern struct attrs32 bamipc_event_type_attrs;

/* convert an event mask to a string */
extern struct attrs32 bamipc_event_mask_attrs;
