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

#ifndef _LOGGER_ARGUMENTS_H
#define _LOGGER_ARGUMENTS_H

#include <sys/types.h>
#include "xapi.h"

/*

MODULE
 logger/arguments

SUMMARY
 program arguments pre-processed by liblogger

*/

extern char *   g_binary;       // name of the executable; pointer into g_argv
extern char *   g_interpreter;  // interpreter script, if executing as an interpreter; pointer into g_argv

extern char **  g_argv;         // array of non-logger arguments
extern size_t   g_argc;         // elements in g_argv
extern char *   g_argvs;        // elements of g_argv joined by single spaces
extern size_t   g_argvsl;       // length of g_argvs

extern char **  g_logv;         // array of recognized logexpr arguments
extern size_t   g_logc;         // elements in g_logv
extern char *   g_logvs;        // elements of g_logv joined by single spaces
extern size_t   g_logvsl;       // length of g_logvs

extern char **  g_ulogv;        // array of unrecognized logexpr arguments
extern size_t   g_ulogc;        // elements in g_ulogv
extern char *   g_ulogvs;       // elements of g_ulogv joined by single spaces
extern size_t   g_ulogvsl;      // length of g_ulogvs

/// logger_arguments_report
//
// SUMMARY
//  log a description of program arguments to LOGGER
//
xapi logger_arguments_report(void);

#endif
