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

#ifndef _FABD_REQUEST_INTERNAL_H
#define _FABD_REQUEST_INTERNAL_H

#include "request.h"
#include "node.h"

struct attrs32;

#define COMMAND_TYPE_OPT 0x000f
#define COMMAND_TYPE_TABLE                                                                                                          \
  DEF(COMMAND_STAGE_CONFIG      , "stage-config"      , COMMAND_TYPE_OPT, 0x01) /* stage config text */                             \
  DEF(COMMAND_RECONFIGURE       , "reconfigure"       , COMMAND_TYPE_OPT, 0x02) /* apply staged config, reconfigure subsystems */   \
  DEF(COMMAND_SELECT            , "select"            , COMMAND_TYPE_OPT, 0x03) /* add nodes to the selection */                    \
  DEF(COMMAND_RESET_SELECTION   , "reset-selection"   , COMMAND_TYPE_OPT, 0x04) /* reset the selection */                           \
  DEF(COMMAND_LIST              , "list"              , COMMAND_TYPE_OPT, 0x05) /* get a list of the nodes in the selection */      \
  DEF(COMMAND_DESCRIBE          , "describe"          , COMMAND_TYPE_OPT, 0x06) /* describe each of the nodes in the selection */   \
  DEF(COMMAND_INVALIDATE        , "invalidate"        , COMMAND_TYPE_OPT, 0x07) /* invalidate each of the nodes in the selection */ \
  DEF(COMMAND_GLOBAL_INVALIDATE , "global-invalidate" , COMMAND_TYPE_OPT, 0x08) /* generation-based global node invalidation */     \
  DEF(COMMAND_RUN               , "run"               , COMMAND_TYPE_OPT, 0x09) /* refresh the graph, pursue goals */               \
  DEF(COMMAND_AUTORUN           , "autorun"           , COMMAND_TYPE_OPT, 0x0a) /* run, and autorun */                              \
  DEF(COMMAND_GOALS             , "goals"             , COMMAND_TYPE_OPT, 0x0b) /* set the goal targets and outputs */              \

typedef enum command_type {
#define DEF(x, s, r, y) x = UINT32_C(y),
COMMAND_TYPE_TABLE
#undef DEF
} command_type;

extern struct attrs32 * command_type_attrs;

typedef struct command {
  union {
    uint32_t attrs;       // bitwise combo of FAB_REQUEST_*
    struct {
      command_type type:8;
    };
  };

  union {
    struct selector * selector;   // SELECT
    struct config * config;       // STAGE_CONFIG
    node_property property;       // LIST
    struct {                      // GOALS
      bool build;
      bool script;
      struct selector * target_direct;
      struct selector * target_transitive;
    } goals;
  };
} command;

#endif
