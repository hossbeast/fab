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

#ifndef _RECONFIGURE_H
#define _RECONFIGURE_H

#include "xapi.h"
#include "types.h"

typedef struct reconfigure_context
{
  bool filesystems_changed;
  bool references_changed;
  bool extern_changed;

  const char * proj_dir;
} reconfigure_context;

/// reconfigure_setup
//
// SUMMARY
//  initializes the config module, does not apply any config files
//
xapi reconfigure_setup(void);

/// reconfigure_cleanup
//
// SUMMARY
//  free resources
//
xapi reconfigure_cleanup(void);

/// reconfigure
//
// SUMMARY
//  promote the staging config to the active config, reconfigure subsystems, and call config_report
//
xapi reconfigure(void);

/// config_stage_files
//
// SUMMARY
//  reload the config files and apply them to the staging config
//
xapi reconfigure_stage_files(void);

/// config_stage
//
// SUMMARY
//  cumulatively apply config text to the staging config
//
// PARAMETERS
//  text - configuration text to apply
//
xapi reconfigure_stage(const char * const restrict text)
  __attribute__((nonnull));

#endif
