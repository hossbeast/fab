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

#include "xapi.h"

/*
 * liblogger-provided program arguments, populated during logger_initialize
 */
extern char **	g_argv;					// array of non-logger arguments
extern int			g_argc;					// count of g_argv
extern char *		g_argvs;				// single string of non-logger arguments
extern size_t		g_argvsl;				// length of g_argvs
extern char *		g_binary;				// executable; pointer into g_argv
extern char *		g_interpreter;	// interpreter script, if executing as an interpreter; pointer into g_argv

extern char **	g_logv;					// array of logexpr arguments
extern int			g_logc;					// count of g_logv
extern char *		g_logvs;				// single string of logexpr arguments
extern size_t   g_logvsl;				// length of g_logvs

/// logger_arguments_report
//
// SUMMARY
//  log a description of cmdline args to LOGGER
//
xapi logger_arguments_report();

#endif
