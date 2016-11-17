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

#ifndef _LOGGER_ARGUMENTS_INTERNAL_H
#define _LOGGER_ARGUMENTS_INTERNAL_H

#include <sys/types.h>

#include "xapi.h"

#include "arguments.h"

/// arguments_teardown
//
// SUMMARY
//  release resources
//
void arguments_teardown(void);

/// arguments_finalize
//
// SUMMARY
//  apply logger cmdline arguments
//
xapi arguments_finalize(void);

/// arguments_process
//
// SUMMARY
//  create g_argv from cmdline arguments
//
// PARAMETERS
//  argvs   - string of arguments with elements delimited by null bytes
//  argvsl  - size of argvs
//  binaryx - index in argvs of the executable name, or -1
//  interpx - index in argvs of the interpreter script, or -1
//
xapi arguments_process(const char * restrict argvs, size_t argvsl, int binaryx, int interpx);

#endif
