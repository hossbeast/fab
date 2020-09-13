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

#include "types.h"
#include "xapi.h"

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstring.h"

#include "formula.h"
#include "formula_parser.h"
#include "formula_value.h"
#include "logging.h"
#include "node.h"

#include "common/snarf.h"

static llist formulas_invalidated = LLIST_INITIALIZER(formulas_invalidated);
static formula_parser *parser;

static xapi formula_refresh(formula * restrict fml)
{
  enter;

  char * text = 0;
  size_t text_len;

  // open the file, both to read its contents, and to exec later
  fatal(ixclose, &fml->fd);
  fatal(xopenats, &fml->fd, O_RDONLY, 0, fml->abspath);

  formula_value_ifree(&fml->file);
  formula_value_ifree(&fml->envs);
  formula_value_ifree(&fml->args);

  fatal(fsnarf, &text, &text_len, fml->fd);
  if(text)
  {
    fatal(formula_parser_parse, parser, text, text_len, fml->abspath, fml);
  }

  logf(L_MODULE, "parsed formula @ %s", fml->abspath);

finally:
  wfree(text);
coda;
}

//
// public
//

xapi formula_xfree(formula * restrict fml)
{
  enter;

  if(fml)
  {
    fatal(xclose, fml->fd);
    wfree(fml->abspath);
    formula_value_free(fml->file);
    formula_value_free(fml->envs);
    formula_value_free(fml->args);
  }
  free(fml);

  finally : coda;
}

xapi formula_node_initialize(node * restrict fml_node)
{
  enter;

  char path[512];
  formula *fml;

  if(node_kind_get(fml_node) == VERTEX_FML) {
    goto XAPI_FINALIZE;
  }
  node_kind_set(fml_node, VERTEX_FML);

  fatal(xmalloc, &fml, sizeof(*fml));
  llist_init_node(&fml->lln_invalidated);
  fml->fd = -1;
  node_absolute_path_znload(path, sizeof(path), fml_node);
  fatal(ixstrdup, &fml->abspath, path);

  fml_node->self_fml = fml;
  fml->fml_node = fml_node;

  formula_invalidated(fml);

  finally : coda;
}

void formula_invalidated(formula * restrict fml)
{
  if(llist_attached(fml, lln_invalidated)) {
    llist_delete(fml, lln_invalidated);
  }

  llist_append(&formulas_invalidated, fml, lln_invalidated);
}

xapi formula_full_refresh()
{
  enter;

  formula *fml;

  llist_foreach(&formulas_invalidated, fml, lln_invalidated) {
    fatal(formula_refresh, fml);
  }

  llist_init_node(&formulas_invalidated);

  finally : coda;
}

xapi formula_setup()
{
  enter;

  fatal(formula_parser_create, &parser);

  finally : coda;
}

xapi formula_cleanup()
{
  enter;

  fatal(formula_parser_xfree, parser);

  finally : coda;
}
