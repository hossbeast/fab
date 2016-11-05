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

#ifndef _PARAMS_H
#define _PARAMS_H

#include "xapi.h"

extern struct g_params
{
  //
  // non-configurable execution parameters
  //

  char *          cwd;         // cwd
  char *          exedir;      // location of the binary executable
} g_params;

xapi params_setup(void);
void params_teardown(void);


/// params_parse
//
// parses command-line options, populating g_params.
//
// Prints the usage statement and terminates the program if options are
// invalid, for example required options are not present, or invalid
// parameters are given to an option
//
xapi params_setup(void);

/// params_teardown
//
// free g_params
//
void params_teardown(void);

#endif
