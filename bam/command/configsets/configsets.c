/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "configsets.h"

//
// static
//

static void configsets_usage(command * restrict cmd)
{
  printf(
"bam configsets - list configsets\n"
"\n"
"usage: bam configsets\n"
  );
}

static void configset_usage(command * restrict cmd)
{
  printf(
"bam configset - show configset metadata\n"
"\n"
"usage: bam configset (<id>|<path>)\n"
" <id>                         configset id\n"
" <path>                       path to configset file\n"
  );
}

//
// public
//

typeof(configset_command) configset_command = {
    name : "configset"
  , usage : configset_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//bm/configsets"
  , visit : "configset"
};

typeof(configsets_command) configsets_command = {
    name : "configsets"
  , usage : configsets_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/configsets"
  , visit : "configset"
};
