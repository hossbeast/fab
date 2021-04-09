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
#include "fsent.h"

struct attrs32;

#define COMMAND_TYPE_OPT 0x00ff
#define COMMAND_TYPE_TABLE                                                                                                      \
  DEF(COMMAND_GLOBAL_STATS_READ  , "global-stats-read"  , COMMAND_TYPE_OPT) /* retrieve global stats */                         \
  DEF(COMMAND_GLOBAL_STATS_RESET , "global-stats-reset" , COMMAND_TYPE_OPT) /* reset global stats, retrieve previous values */  \
  DEF(COMMAND_STATS_READ         , "stats-read"         , COMMAND_TYPE_OPT) /* retrieve node stats */                           \
  DEF(COMMAND_STATS_RESET        , "stats-reset"        , COMMAND_TYPE_OPT) /* reset node stats, retrieve previous values */    \
  DEF(COMMAND_CONFIG_READ        , "config-read"        , COMMAND_TYPE_OPT) /* read effective configuration */                  \
  DEF(COMMAND_VARS_READ          , "vars-read"          , COMMAND_TYPE_OPT) /* read effective vars */                           \
  DEF(COMMAND_SELECT             , "select"             , COMMAND_TYPE_OPT) /* add nodes to the selection */                    \
  DEF(COMMAND_RESET_SELECTION    , "reset-selection"    , COMMAND_TYPE_OPT) /* reset the selection */                           \
  DEF(COMMAND_LIST               , "list"               , COMMAND_TYPE_OPT) /* get minimal info about each selected node */     \
  DEF(COMMAND_DESCRIBE           , "describe"           , COMMAND_TYPE_OPT) /* describe each of the nodes in the selection */   \
  DEF(COMMAND_INVALIDATE         , "invalidate"         , COMMAND_TYPE_OPT) /* invalidate the nodes in the selection */         \
  DEF(COMMAND_GLOBAL_INVALIDATE  , "global-invalidate"  , COMMAND_TYPE_OPT) /* generation-based global node invalidation */     \
  DEF(COMMAND_METADATA           , "metadata"           , COMMAND_TYPE_OPT) /* retrieve project metadata */                     \
  DEF(COMMAND_GOALS              , "goals"              , COMMAND_TYPE_OPT) /* set the goal targets */                          \
  DEF(COMMAND_RECONCILE          , "reconcile"          , COMMAND_TYPE_OPT) /* reload everything */                             \
  /* needs reconcile */                                                                                                         \
  DEF(COMMAND_RUN                , "run"                , COMMAND_TYPE_OPT) /* pursue goals */                                  \
  DEF(COMMAND_AUTORUN            , "autorun"            , COMMAND_TYPE_OPT) /* pursue goals */                                  \

typedef enum command_type {
COMMAND_TYPE_RANGE_BEFORE = 0,
#undef DEF
#define DEF(x, s, r) x,
COMMAND_TYPE_TABLE
} command_type;

extern struct attrs32 * command_type_attrs;

typedef struct command {
  llist lln;

  union {
    uint32_t attrs;       // bitwise combo of FAB_REQUEST_*
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
  };
} command;

#endif
