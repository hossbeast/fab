
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

#ifndef _LOGGER_CONFIGURE_H
#define _LOGGER_CONFIGURE_H

/// log_config
// 
// SUMMARY
//  parse cmdline logger arguments gathered during log_init and configure logging options
//
// PARAMETERS
//  [prefix] - matching logs will emit preceeding prefix
//  [trace]  - matching logs will emit trailing trace
//  [bits]   - bits to use when logging the description
//
#if DEBUG || DEVEL
xapi log_config(uint64_t prefix, uint64_t trace);
xapi log_log_config_and_describe(const char * const restrict func, const char * const restrict file, int line, uint64_t prefix, uint64_t trace, uint64_t bits)
	__attribute__((nonnull(1, 2)));

#define log_config_and_describe(...) log_log_config_and_describe(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
xapi log_config(uint64_t prefix);
xapi log_log_config_and_describe(uint64_t prefix, uint64_t bits);

#define log_config_and_describe(...) log_log_config_and_describe(__VA_ARGS__)
#endif

/// log_parse
//
// SUMMARY
//  parse the logexpr to enable/disable logging categories
//
// PARAMETERS
//  [func]     - function name
//  [file]     - file name
//  [line]     - line number
//  expr       - logexpr
//  [expr_len] - length of expr, or 0 for strlen
//  [before]   - prepend to the list of filters (append otherwise)
//  [bits]     - bits to use when logging the description
//
xapi log_parse(char * expr, int expr_len, int prepend)
	__attribute__((nonnull(1)));

#if DEBUG || DEVEL
xapi log_log_parse_and_describe(const char * const restrict func, const char * const restrict file, int line, char * expr, int expr_len, int prepend, uint64_t bits)
	__attribute__((nonnull(1, 2, 4)));

#define log_parse_and_describe(...) log_log_parse_and_describe(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
xapi log_log_parse_and_describe(char * expr, int expr_len, int prepend, uint64_t bits)
	__attribute__((nonnull(1)));

#define log_parse_and_describe(...) log_log_parse_and_describe(__VA_ARGS__)
#endif

/// log_parse_pop
//
// SUMMARY
//  remove the most recently parsed logexpr
//
xapi log_parse_pop();

/// log_parse_clear
//
// SUMMARY
//  remove all logexprs
//
xapi log_parse_clear();


#endif
