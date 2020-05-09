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

#ifndef FABD_CONFIG_H
#define FABD_CONFIG_H

#include "types.h"
#include "xapi.h"

struct config;
typedef struct config config;

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

/// config_stage
//
// SUMMARY
//  cumulatively apply config to the staging config
//
// PARAMETERS
//  text - configuration text to apply
//
xapi config_stage(config ** const restrict)
  __attribute__((nonnull));

/// reconfigure
//
// SUMMARY
//  promote the staging config to the active config, reconfigure subsystems, and call config_report
//
xapi config_reconfigure(void);

#endif
