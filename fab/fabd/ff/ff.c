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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "lorien/path_normalize.h"
#include "narrator.h"

#include "ff.h"
#include "ff_parser.h"
#include "ff_node.h"
#include "params.h"

#include "snarf.h"
#include "macros.h"
#include "fmt.h"
#include "logging.h"
#include "config_cursor.h"
#include "reconfigure.h"
#include "config.h"
#include "value.h"

// special value stored when there is no fabfile at a path, or it is empty
#define FF_NXFILE ((void*)0x1)

static ff_parser * parser;
static map * ff_by_path;

static void ff_ffn_free(void * ffn)
{
  if(ffn != FF_NXFILE)
  {
    ffn_free(ffn);
  }
}

//
// public
//

xapi ff_setup()
{
  enter;

  fatal(ff_parser_create, &parser);
  fatal(map_createx, &ff_by_path, 0, ff_ffn_free, 0);

  finally : coda;
}

xapi ff_cleanup()
{
  enter;

  ff_parser_free(parser);
  fatal(map_xfree, ff_by_path);

  finally : coda;
}

xapi ff_report()
{
  enter;

  finally : coda;
}

xapi ff_load_paths(ff_node ** restrict root, const char * restrict path)
{
  enter;

  char * text = 0;
  ff_node * ffn = 0;

  if((ffn = map_get(ff_by_path, MMS(path))) == 0)
  {
    fatal(usnarfs, &text, 0, path);
    if(text)
      fatal(ff_parser_parse, &parser, MMS(text), "module.fab", &ffn);

    // no file, empty file
    if(ffn == 0)
      ffn = FF_NXFILE;

    logf(L_FF, "loaded ff %s", path);

    fatal(map_set, ff_by_path, MMS(path), ffn);
  }

  if(ffn == FF_NXFILE)
    ffn = 0;

  *root = ffn;

finally:
  wfree(text);

  xapi_infos("path", path);
coda;
}

xapi ff_load_pathf(ff_node ** restrict root, const char * restrict fmt, ...)
{
  enter;

  char path[512];

  va_list va;
  va_start(va, fmt);
  fatal(fmt_apply, path, sizeof(path), fmt, va);

  fatal(ff_load_paths, root, path);

finally:
  va_end(va);
coda;
}
