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
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"

#include "value.h"
#include "value/value_store.h"

#include "internal.h"
#include "config.h"
#include "config_parser.h"

#include "snarf.h"

#define restrict __restrict

#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"
#define USER_CONFIG_PATH      "~/.fabconfig"
#define PROJECT_CONFIG_PATH   ".fabconfig"

static config_parser * parser;
static value_store * valstore;
static value * config;

/// apply
//
//
//
static xapi apply(value * dst, value * src)
{
  enter;

  finally : coda;
}

//
// public
//

xapi config_setup()
{
  enter;

  fatal(config_parser_create, &parser);

  finally : coda;
}

void config_teardown()
{
  config_parser_free(parser);
  value_store_free(valstore);
}

xapi config_load()
{
  enter;

  value_store * store = 0;
  value * eff = 0;
  value * cfg = 0;
  char * text = 0;
  size_t textl = 0;

  // new storage
  fatal(value_store_create, &store);

  // system
  fatal(snarfs, SYSTEM_CONFIG_PATH, &text, &textl);
  fatal(config_parser_parse, &parser, &store, text, textl, &cfg);
  eff = cfg;

  // user
  ifree(&text);
  fatal(snarfs, USER_CONFIG_PATH, &text, &textl);
  fatal(config_parser_parse, &parser, &store, text, textl, &cfg);
  fatal(apply, eff, cfg);

  // project
  ifree(&text);
  fatal(snarfs, PROJECT_CONFIG_PATH, &text, &textl);
  fatal(config_parser_parse, &parser, &store, text, textl, &cfg);
  fatal(apply, eff, cfg);

  // replace the current config
  value_store_ifree(&valstore);
  valstore = store;
  store = 0;
  config = eff;

  // log the new effective config

finally:
  free(text);
  value_store_free(store);
coda;
}

xapi config_apply(const char * const restrict src, size_t len)
{
  enter;

  // integrate into current config
  value * cfg = 0;
  fatal(config_parser_parse, &parser, &valstore, src, len, &cfg);
  fatal(apply, config, cfg);

  // log the new effective config

  finally : coda;
}

xapi config_query(const char * const restrict query, struct value ** const restrict val)
{
  enter;

  finally : coda;
}
