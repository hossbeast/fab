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
  DEF(FABIPC_EVENT_NODE_STALE          , "node-stale"          , 0x0001)  /* node marked as stale */       \
  DEF(FABIPC_EVENT_NODE_FRESH          , "node-fresh"          , 0x0002)  /* node marked as fresh */       \
  DEF(FABIPC_EVENT_NODE_DELETE         , "node-delete"         , 0x0004)  /* node deleted */               \
  DEF(FABIPC_EVENT_GLOBAL_INVALIDATE   , "global-invalidate"   , 0x0008)  /* global node invalidation */   \
  DEF(FABIPC_EVENT_FORMULA_EXEC_FORKED , "formula-exec-fork"   , 0x0010)  /* formula execution - forked */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_WAITED , "formula-exec-wait"   , 0x0020)  /* formula execution - waited */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDOUT , "formula-exec-stdout" , 0x0040)  /* formula execution - stdout */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDERR , "formula-exec-stderr" , 0x0080)  /* formula execution - stderr */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_AUXOUT , "formula-exec-auxout" , 0x0100)  /* formula execution - auxout */ \
  DEF(FABIPC_EVENT_GOALS               , "goals"               , 0x0200)  /* goals changed */              \
  DEF(FABIPC_EVENT_BUILD_START         , "build-start"         , 0x0400)  /* build started */              \
  DEF(FABIPC_EVENT_BUILD_END           , "build-end"           , 0x0800)  /* build ended */                \
  DEF(FABIPC_EVENT_BAMD_STDOUT         , "bamd-stdout"         , 0x1000)  /* daemon - stdout text */       \
  DEF(FABIPC_EVENT_BAMD_STDERR         , "bamd-stderr"         , 0x2000)  /* daemon - stderr text */       \

typedef enum fabipc_event_type {
#undef DEF
#define DEF(x, s, y) x = y,
FABIPC_EVENT_TYPE_TABLE
} fabipc_event_type;

extern struct attrs32 * fabipc_event_type_attrs;

#endif
