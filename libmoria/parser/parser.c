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

/* flex and bison do not agree on these names */
#define YYSTYPE GRAPH_YYSTYPE
#define YYLTYPE yyu_location

#include "parser.internal.h"
#include "MORIA.errtab.h"
#include "graph.tab.h"
#include "graph.tokens.h"
#include "graph.lex.h"
#include "graph.states.h"
#include "logging.internal.h"

static yyu_vtable vtable = {
    yy_scan_bytes : graph_yy_scan_bytes
  , yy_delete_buffer : graph_yy_delete_buffer
  , yyset_extra : graph_yyset_extra
  , yyparse : graph_yyparse
  , yylex_init : graph_yylex_init
  , yylex_destroy : graph_yylex_destroy
};

xapi API graph_parser_create(graph_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
#if DEBUG || DEVEL || XUNIT
  (*parser)->yyu.logs = L_MORIA | L_GRAPH;
#endif

  fatal(yyu_parser_init, &(*parser)->yyu, &vtable, MORIA_SYNTAX);

  fatal(yyu_parser_init_tokens
    , &(*parser)->yyu
    , graph_numtokens
    , graph_mintoken
    , graph_maxtoken
    , graph_tokenindexes
    , graph_tokennumbers
    , graph_tokennames
    , graph_tokenstrings
    , graph_tokenstring_tokens
  );

  fatal(yyu_parser_init_states
    , &(*parser)->yyu
    , graph_numstates
    , graph_statenumbers
    , graph_statenames
  );

  fatal(map_create, &(*parser)->vertex_map);

  finally : coda;
}

xapi API graph_parser_xfree(graph_parser* const p)
{
  enter;

  if(p)
  {
    fatal(map_xfree, p->vertex_map);

    fatal(yyu_parser_xdestroy, &p->yyu);
  }

  wfree(p);

  finally : coda;
}

xapi API graph_parser_ixfree(graph_parser ** const p)
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

xapi API graph_parser_graph_create(graph ** restrict g, uint32_t identity)
{
  xproxy(graph_createx, g, identity, sizeof(void*), 0, free_value, 0);
}

xapi API graph_parser_parse(
    graph_parser * restrict parser
  , graph * restrict g
  , char * const restrict text
  , size_t len
)
{
  enter;

  fatal(map_recycle, parser->vertex_map);

  parser->vertex_defs = g->vertex_defs;
  parser->edge_defs = g->edge_defs;
  parser->g = g;

  fatal(yyu_parse, &parser->yyu, text, len, "-graph-", 0, 0, 0);

  finally : coda;
}
