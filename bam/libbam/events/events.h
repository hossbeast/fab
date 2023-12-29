/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct attrs32;

#define BAMIPC_EVENT_TYPE_TABLE                                                                            \
  /* freetext - relative path */                                                                           \
  DEF(BAMIPC_EVENT_NODE_STALE          , "node-stale"          , 0x0001)  /* node marked as stale */       \
  /* freetext - relative path */                                                                           \
  DEF(BAMIPC_EVENT_NODE_FRESH          , "node-fresh"          , 0x0002)  /* node marked as fresh */       \
  /* freetext - relative path */                                                                           \
  DEF(BAMIPC_EVENT_NODE_CREATE         , "node-create"         , 0x0004)  /* node created */               \
  /* freetext - relative path */                                                                           \
  DEF(BAMIPC_EVENT_NODE_DELETE         , "node-delete"         , 0x0008)  /* node deleted */               \
  /* none */                                                                                               \
  DEF(BAMIPC_EVENT_GLOBAL_INVALIDATE   , "global-invalidate"   , 0x0010)  /* global node invalidation */   \
  /* struct bam_build_slot_info */                                                                         \
  DEF(BAMIPC_EVENT_FORMULA_EXEC_FORKED , "formula-exec-fork"   , 0x0020)  /* formula execution - forked */ \
  /* struct bam_build_slot_results */                                                                      \
  DEF(BAMIPC_EVENT_FORMULA_EXEC_WAITED , "formula-exec-wait"   , 0x0040)  /* formula execution - waited */ \
  /* freetext */                                                                                           \
  DEF(BAMIPC_EVENT_FORMULA_EXEC_STDOUT , "formula-exec-stdout" , 0x0080)  /* formula execution - stdout */ \
  DEF(BAMIPC_EVENT_FORMULA_EXEC_STDERR , "formula-exec-stderr" , 0x0100)  /* formula execution - stderr */ \
  DEF(BAMIPC_EVENT_FORMULA_EXEC_AUXOUT , "formula-exec-auxout" , 0x0200)  /* formula execution - auxout */ \
  /* none */                                                                                               \
  DEF(BAMIPC_EVENT_GOALS               , "goals"               , 0x0400)  /* goals changed */              \
  /* freetext - old state -> new state */                                                                  \
  DEF(BAMIPC_EVENT_SYSTEM_STATE        , "system-state"        , 0x0800)  /* system state change */        \

typedef enum bamipc_event_type {
#undef DEF
#define DEF(x, s, y) x = y,
BAMIPC_EVENT_TYPE_TABLE
} bamipc_event_type;

extern struct attrs32 * bamipc_event_type_attrs;
