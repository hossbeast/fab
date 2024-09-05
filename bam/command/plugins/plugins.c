/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "plugins.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

//
// plugin
//

static void plugin_usage(command * restrict cmd)
{
  printf(
"plugin options (none)\n"
  );
}

//
// plugins
//

static void plugins_usage(command * restrict cmd)
{
  printf(
"\n"
"plugins options (none)\n"
  );
}

typeof(plugin_command) plugin_command = {
    name : "plugin"
  , usage : plugin_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//plugins"
  , visit : "plugin"
};

typeof(plugins_command) plugins_command = {
    name : "plugins"
  , usage : plugins_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//plugins"
  , visit : "plugin"
};
