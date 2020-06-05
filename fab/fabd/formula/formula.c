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

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "valyria/array.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"
#include "value.h"
#include "moria/traverse.h"
#include "narrator.h"
#include "narrator/growing.h"

#include "formula.internal.h"
#include "formula_parser.h"
#include "formula_value.h"
#include "build_thread.internal.h"
#include "config.h"
#include "logging.h"
#include "module.internal.h"
#include "node.h"
#include "path.h"
#include "selector.h"
#include "variant.h"
#include "node.h"

#include "common/snarf.h"
#include "macros.h"
#include "common/attrs.h"
#include "common/assure.h"
#include "zbuffer.h"
#include "common/hash.h"

static xapi formula_create(formula ** restrict rv)
{
  enter;

  formula * fml = 0;

  fatal(xmalloc, &fml, sizeof(*fml));

  *rv = fml;
  fml = 0;

finally:
  fatal(formula_xfree, fml);
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
    wfree(fml->abspath);
    formula_value_free(fml->path);
    formula_value_free(fml->envs);
    formula_value_free(fml->args);
  }
  free(fml);

  finally : coda;
}

xapi formula_node_parse(node * restrict fml_node)
{
  enter;

  char * text = 0;
  size_t text_len;
  formula * fml = 0;
  formula_parser * formula_parser = 0;

  char path[512];

  if(!fml_node->not_parsed)
    goto XAPI_FINALLY;

  node_kind_set(fml_node, VERTEX_FML);
  fatal(formula_create, &fml);
  fatal(formula_parser_create, &formula_parser);

  // absolute path, for exec
  node_get_absolute_path(fml_node, path, sizeof(path));
  fatal(ixstrdup, &fml->abspath, path);

  fatal(snarfs, &text, &text_len, fml->abspath);
  if(text)
  {
    fatal(formula_parser_parse, formula_parser, text, text_len, path, fml);
  }

  logf(L_MODULE, "parsed formula @ %s", path);

  fml->fml_node = fml_node;
  fml_node->self_fml = fml;
  fml_node->not_parsed = 0;
  fml = 0;

finally:
  wfree(text);
  fatal(formula_xfree, fml);
  fatal(formula_parser_xfree, formula_parser);
coda;
}
