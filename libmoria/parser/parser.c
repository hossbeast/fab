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

#include "xapi.h"

#include "xlinux/KERNEL.errtab.h"
#include "valyria/map.h"
#include "valyria/dictionary.h"

#include "internal.h"
#include "parser.internal.h"
#include "MORIA.errtab.h"
#include "graph.tab.h"
#include "graph.tokens.h"
#include "graph.lex.h"
#include "graph.states.h"
#include "logging.internal.h"

struct graph_parser
{
  void * p;
};

//
// static
//

static const char * tokenname(int token)
{
  return graph_tokennames[token];
}

static const char * statename(int state)
{
  return state >= 0 ? graph_statenames[state] : "";
}

//
// protected
//

//
// public
//

API xapi graph_parser_create(graph_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
  tfatalize(perrtab_KERNEL, ENOMEM, graph_yylex_init, &(*parser)->p);

  finally : coda;
}

API xapi graph_parser_xfree(graph_parser* const p)
{
  enter;

  if(p)
    graph_yylex_destroy(p->p);

  wfree(p);

  finally : coda;
}

API xapi graph_parser_ixfree(graph_parser ** const p)
{
  enter;

  fatal(graph_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

static void free_value(void * value)
{
  free(*(char**)value);
}

API xapi graph_parser_graph_create(graph ** restrict g, uint32_t identity)
{
  xproxy(graph_createx, g, identity, sizeof(void*), free_value, 0);
}

API xapi graph_parser_parse(
    graph_parser ** restrict parser
  , const char * const restrict text
  , size_t len
  , graph ** restrict g
  , uint32_t identity
)
{
  enter;

  graph * lg = 0;
  graph_parser * lp = 0;
  void * lexer_state = 0;

  graph_xtra pp = {
      .tokname      = tokenname
    , .statename    = statename
#if DEBUG || DEVEL || XUNIT
    , .state_logs   = L_MORIA | L_GRAPH
    , .token_logs   = L_MORIA | L_GRAPH
#endif
  };

  // parser
  if(!parser)
    parser = &lp;
  if(!*parser)
    fatal(graph_parser_create, parser);

  // storage
  if(!g)
    g = &lg;
  if(!*g)
    fatal(graph_parser_graph_create, g, identity);

  // create state specific to this parse
  if((lexer_state = graph_yy_scan_bytes(text, len, (*parser)->p)) == 0)
    fail(KERNEL_ENOMEM);

  fatal(dictionary_create, &pp.definitions, sizeof(uint32_t));
  fatal(map_create, &pp.vertex_map);

  pp.scanner = (*parser)->p;
  pp.g = *g;

  // make available to the lexer
  graph_yyset_extra(&pp, (*parser)->p);

  // invoke the appropriate parser, raise errors as needed
  fatal(yyu_reduce, graph_yyparse, &pp, MORIA_SYNTAX);

  // ownership transfer
  lg = 0;

finally:
  graph_yy_delete_buffer(lexer_state, (*parser)->p);
  yyu_extra_destroy(&pp.yyu);
  fatal(map_xfree, pp.vertex_map);
  fatal(dictionary_xfree, pp.definitions);

  fatal(graph_parser_xfree, lp);
  fatal(graph_ixfree, &lg);
coda;
}
