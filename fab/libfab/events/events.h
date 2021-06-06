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

#ifndef _FAB_EVENTS_H
#define _FAB_EVENTS_H

struct attrs32;

#define FABIPC_EVENT_TYPE_TABLE                                                                            \
  /* freetext - relative path */                                                                           \
  DEF(FABIPC_EVENT_NODE_STALE          , "node-stale"          , 0x0001)  /* node marked as stale */       \
  /* freetext - relative path */                                                                           \
  DEF(FABIPC_EVENT_NODE_FRESH          , "node-fresh"          , 0x0002)  /* node marked as fresh */       \
  /* freetext - relative path */                                                                           \
  DEF(FABIPC_EVENT_NODE_CREATE         , "node-create"         , 0x0004)  /* node created */               \
  /* freetext - relative path */                                                                           \
  DEF(FABIPC_EVENT_NODE_DELETE         , "node-delete"         , 0x0008)  /* node deleted */               \
  /* none */                                                                                               \
  DEF(FABIPC_EVENT_GLOBAL_INVALIDATE   , "global-invalidate"   , 0x0010)  /* global node invalidation */   \
  /* struct fab_build_slot_info */                                                                         \
  DEF(FABIPC_EVENT_FORMULA_EXEC_FORKED , "formula-exec-fork"   , 0x0020)  /* formula execution - forked */ \
  /* struct fab_build_slot_results */                                                                      \
  DEF(FABIPC_EVENT_FORMULA_EXEC_WAITED , "formula-exec-wait"   , 0x0040)  /* formula execution - waited */ \
  /* freetext */                                                                                           \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDOUT , "formula-exec-stdout" , 0x0080)  /* formula execution - stdout */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDERR , "formula-exec-stderr" , 0x0100)  /* formula execution - stderr */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_AUXOUT , "formula-exec-auxout" , 0x0200)  /* formula execution - auxout */ \
  /* none */                                                                                               \
  DEF(FABIPC_EVENT_GOALS               , "goals"               , 0x0400)  /* goals changed */              \
  /* freetext - old state -> new state */                                                                  \
  DEF(FABIPC_EVENT_SYSTEM_STATE        , "system-state"        , 0x0800)  /* system state change */        \

typedef enum fabipc_event_type {
#undef DEF
#define DEF(x, s, y) x = y,
FABIPC_EVENT_TYPE_TABLE
} fabipc_event_type;

extern struct attrs32 * fabipc_event_type_attrs;

#endif
