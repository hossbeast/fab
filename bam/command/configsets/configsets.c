/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"
#include "configsets.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"configset options (none)\n"
  );
}

//
// public
//

typeof(configset_command) configset_command = {
    name : "configset"
  , usage : usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//configsets"
  , visit : "configset"
};

typeof(configsets_command) configsets_command = {
    name : "configsets"
  , usage : usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//configsets"
  , visit : "configset"
};
