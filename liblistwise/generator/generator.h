/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_GENERATOR_H
#define _LISTWISE_GENERATOR_H

/// definitions are in operator.h
struct arg;
struct operation;
typedef struct generator_parser_t generator_parser;

typedef struct generator
{
	struct arg **				args;		// initial list contents
	int									argsl;

	struct operation **	ops;		// operator/args pairs to be executed sequentially
	int									opsl;
} generator;

///
/// [[ GENERATOR API ]]
///

#define restrict __restrict

/// generator_mkparser
//
// returns a generator parser
//
int generator_mkparser(generator_parser ** p);

/// (XAPI) generator_parse
//
// parse a generator string
//
// parameters
//
//  [p]    - parser returned from mkparser
//   s     - generator string
//   l     - length of generator string, or 0 for strlen
//   r     - receives parsed generator
//  [name] - also specify a name for this input (that is only used in informational and error messages)
//
int generator_parse(generator_parser ** p, char* s, int l, generator** r);
int generator_parse2(generator_parser ** p, char* s, int l, generator** r, void * udata);

/// generator_parse_named
//
// see generator_parse
//
int generator_parse_named(generator_parser ** p, char* s, int l, char * name, int namel, generator** r);
int generator_parse_named2(generator_parser ** p, char* s, int l, char * name, int namel, generator** r, void * udata);

/// generator_freeparser 
//
// frees a generator parser returned from mkparser
//
void generator_parser_free(generator_parser*);
void generator_parser_xfree(generator_parser**);

/// generator_free
//
// frees a generator returned from parse
//
void generator_free(generator*);
void generator_xfree(generator**);

#undef restrict
#endif
