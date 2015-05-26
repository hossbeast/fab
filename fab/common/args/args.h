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

#ifndef _FAB_ARGS_H
#define _FAB_ARGS_H

/* args parsing */

#include <stdint.h>
#include <sys/types.h>

// args definition and persistence
#include "args.def.h"

#define restrict __restrict

/// args_parse
//
// SUMMARY
//  parses command-line options, populating g_args.
//
// REMARKS
//  prints the usage statement and terminates the program if options are
//  invalid, for example required options are not present, or invalid
//  parameters are given to an option
//
int args_parse();

/// args_summarize
//
// SUMMARY
//  use logger to output a summary of params/args as received
//
int args_summarize();

/// args_teardown
//
// free g_args
//
void args_teardown();

#undef restrict
#endif
