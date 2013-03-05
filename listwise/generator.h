#ifndef _LISTWISE_GENERATOR_H
#define _LISTWISE_GENERATOR_H

#include <listwise/operator.h>

/// generator declaration - definition is private
struct generator_t;
typedef struct generator_t generator;

struct generator_t
{
	arg **				args;		// initial list contents
	int						argsl;

	operation **	ops;		// operator/args pairs to be executed sequentially
	int						opsl;
};


/// generator parser declaration
struct generator_parser_t;
typedef struct generator_parser_t generator_parser;

///
/// [[ GENERATOR API ]]
///

/// generator_mkparser
//
// returns a generator parser
//
int generator_mkparser(generator_parser** p);

/// generator_parse
//
// parse a generator string
//
// parameters
//
//   p  - parser returned from mkparser, or 
//   s  - generator string
//   l  - length of generator string, or 0 for strlen
//   r  - receives parsed generator
//
// returns zero on failure
//
int generator_parse(generator_parser* p, char* s, int l, generator** r);

/// generator_freeparser 
//
// frees a generator parser returned from mkparser
//
void generator_parser_free(generator_parser*);
void generator_parser_xfree(generator_parser**);

/// generator_dump
//
// print a generator to stdout
//
void generator_dump(generator*);

/// generator_free
//
// frees a generator returned from parse
//
void generator_free(generator*);
void generator_xfree(generator**);

#endif
