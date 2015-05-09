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

#ifndef _CFG_H
#define _CFG_H

#define restrict __restrict

// cfg parser ; opaque type
struct cfg_parser;
typedef struct cfg_parser cfg_parser;

// cfg file ; opaque type
struct cfg_file;
typedef struct cfg_file cfg_file;

/// cfg_mkparser
//
// creates an instance of a cfg parser which may be passed repeatedly to parse
//
int cfg_mkparser(cfg_parser ** const restrict p)
	__attribute__((nonnull));

/// cfg_load
//
// SUMMARY
//  load (parsing if necessary) the cfg at the specified path
//
// PARAMETERS
//  p      - parser returned from mkparser
//  path   - canonical path to cfg file
//  [cfg]  - (returns) cfg file
//
// RETURNS
//  nonzer on error - otherwise *cfg is nonzero
//
int cfg_load(
	  const cfg_parser * const restrict p
	, const char * const restrict path
	, cfg_file ** restrict cfg
)
	__attribute__((nonnull(1,2)));

/// cfg_freeparser
//
// free a cfg parser returned from mkparser with free semantics
//
void cfg_freeparser(cfg_parser * const restrict);

/// cfg_xfreeparser
//
// free a cfg parser returned from mkparser with xfree semantics
//
void cfg_xfreeparser(cfg_parser ** const restrict)
	__attribute__((nonnull));

/// cfg_init
//
// SUMMARY
//  initialize
//
int cfg_init();

/// cfg_teardown
//
// SUMMARY
//  free cfg_files
//
void cfg_teardown();

/// cfg_finalize
//
// SUMMARY
//  invoked after cfg files are parsed to prepare lookup structures
//
int cfg_finalize();

#undef restrict
#endif
