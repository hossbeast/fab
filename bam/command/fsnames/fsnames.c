/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "bam/client.h"
#include "bam/ipc.h"

#include "fsnames.h"
#include "params.h"


//
// fsname
//

static void fsname_usage(command * restrict cmd)
{
  printf(
"fsname options (none)\n"
  );
}

//
// fsnames
//

static void fsnames_usage(command * restrict cmd)
{
  printf(
"\n"
"fsnames options (none)\n"
  );
}

static void fsname_build(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("union")
          { LIST
            { MAPPING
              STRING("lookup")
              { SET
                { MAPPING
                  STRING("path")
                  LOADF("//fsnames/%.*s", cmd->target_len, cmd->target);
                }
              }
            }
            { MAPPING
              STRING("lookup")
              { SET
                { MAPPING
                  STRING("path")
                  LOADF("//fsnames-by/name/%.*s", cmd->target_len, cmd->target);
                }
              }
            }
          }
        }
        /* limit to vertices of the matching type ; not really needed */
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING
              STRING("direction")
              STRING("up")
            }
            { MAPPING
              STRING("graph")
              STRING("special")
            }
            { MAPPING
              STRING("min-distance")
              UINT(0)
            }
            { MAPPING
              STRING("max-distance")
              UINT(0)
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void fsname_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->identify_id_0 = command_post(cmd, client, fsname_build);

  message_no_error_list[0] = cmd->identify_id_0;
  message_no_error_list_len = 1;
}

static void fsname_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0) {
      (void)!write(1, msg->text, msg->size);
      cmd->any = true;
    }
  } else {
    RTA;
  }
}

static void fsname_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

typeof(fsname_command) fsname_command = {
    name : "fsname"
  , usage : fsname_usage
  , connected : fsname_connected
  , process : fsname_process
  , disconnected : fsname_disconnected
  , prefix : "//fsnames"
};

typeof(fsnames_command) fsnames_command = {
    name : "fsnames"
  , usage : fsnames_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//fsnames"
};
