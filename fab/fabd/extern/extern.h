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

#ifndef FABD_EXTERN_H
#define FABD_EXTERN_H

/*

SUMMARY

 portions of the filesystem outside the module directory are loaded from directory trees specified
 in config under the "extern" key.

*/

#include "xapi.h"
#include "types.h"

struct reconfigure_context;
struct value;

xapi extern_setup(void);
xapi extern_cleanup(void);

/// extern_reconfigure
//
// SUMMARY
//  reapply configuration to the extern module
//
// PARAMETERS
/// ctx    - reconfiguration context
//  config - root of the config tree
//  dry    - whether to perform a dry-run
//
xapi extern_reconfigure(struct reconfigure_context * restrict ctx, const struct value * restrict config, uint32_t dry)
  __attribute__((nonnull));

#endif
