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

#define FABIPC_EVENT_TYPE_OPT                                                           0xff
#define FABIPC_EVENT_TYPE_TABLE                                                                                                 \
  DEF(FABIPC_EVENT_NODE_STALE          , "node-stale"          , FABIPC_EVENT_TYPE_OPT, 0x01)  /* node marked as stale */       \
  DEF(FABIPC_EVENT_NODE_FRESH          , "node-fresh"          , FABIPC_EVENT_TYPE_OPT, 0x02)  /* node marked as fresh */       \
  DEF(FABIPC_EVENT_NODE_DELETE         , "node-delete"         , FABIPC_EVENT_TYPE_OPT, 0x03)  /* node deleted */               \
  DEF(FABIPC_EVENT_GLOBAL_INVALIDATION , "global-invalidation" , FABIPC_EVENT_TYPE_OPT, 0x04)  /* global node invalidation */   \
  DEF(FABIPC_EVENT_FORMULA_EXEC_FORKED , "formula-exec-fork"   , FABIPC_EVENT_TYPE_OPT, 0x05)  /* formula execution - forked */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_WAITED , "formula-exec-wait"   , FABIPC_EVENT_TYPE_OPT, 0x06)  /* formula execution - waited */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDOUT , "formula-exec-stdout" , FABIPC_EVENT_TYPE_OPT, 0x07)  /* formula execution - stdout */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_STDERR , "formula-exec-stderr" , FABIPC_EVENT_TYPE_OPT, 0x08)  /* formula execution - stderr */ \
  DEF(FABIPC_EVENT_FORMULA_EXEC_AUXOUT , "formula-exec-auxout" , FABIPC_EVENT_TYPE_OPT, 0x09)  /* formula execution - auxout */ \
  DEF(FABIPC_EVENT_GOALS               , "goals"               , FABIPC_EVENT_TYPE_OPT, 0x0a)  /* goals changed */              \
  DEF(FABIPC_EVENT_BUILD_START         , "build-start"         , FABIPC_EVENT_TYPE_OPT, 0x0b)  /* build started */              \
  DEF(FABIPC_EVENT_BUILD_END           , "build-end"           , FABIPC_EVENT_TYPE_OPT, 0x0c)  /* build ended */                \
  DEF(FABIPC_EVENT_STDOUT              , "fabd-stdout"         , FABIPC_EVENT_TYPE_OPT, 0x0d)  /* fabd - stdout text */         \
  DEF(FABIPC_EVENT_STDERR              , "fabd-stderr"         , FABIPC_EVENT_TYPE_OPT, 0x0e)  /* fabd - stderr text */         \

typedef enum fabipc_event_type {
#undef DEF
#define DEF(x, s, r, y) x = y,
FABIPC_EVENT_TYPE_TABLE
} fabipc_event_type;

extern struct attrs32 * fabipc_event_type_attrs;

#endif
