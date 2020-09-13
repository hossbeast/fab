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

#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"
#include "xapi.h"

struct config;
typedef struct config config;
struct attrs32;

// configuration
#define STREAM_PART_OPT 0xf
#define STREAM_PART_TABLE                                                 \
  STREAM_PART_DEF(STREAM_PART_LEADING   , leading , STREAM_PART_OPT, 0x1) \
  STREAM_PART_DEF(STREAM_PART_TRAILING  , trailing, STREAM_PART_OPT, 0x2) \
  STREAM_PART_DEF(STREAM_PART_NONE      , none    , STREAM_PART_OPT, 0x3)

typedef enum stream_part {
#define STREAM_PART_DEF(x, n, r, y) x = UINT32_C(y),
STREAM_PART_TABLE
#undef STREAM_PART_DEF
} stream_part;

extern struct attrs32 * stream_part_attrs;

/// config_setup
//
// SUMMARY
//  initializes the config module, does not apply any config files
//
xapi config_setup(void);

/// config_cleanup
//
// SUMMARY
//  free resources
//
xapi config_cleanup(void);

/// config_report
//
// SUMMARY
//  log the active config with L_CONFIG
//
xapi config_report(void);

/// config_begin_staging
//
// SUMMARY
//  reset the staging config, reload config files and apply them to the staging config
//
xapi config_begin_staging(void);

/// reconfigure
//
// SUMMARY
//  promote the staging config to the active config, reconfigure subsystems, and call config_report
//
xapi config_reconfigure(void);

#endif
