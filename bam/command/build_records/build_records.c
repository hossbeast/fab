/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"

#include "build_records.h"
#include "params.h"

//
// build
//

static void build_record_usage(command * restrict cmd)
{
  printf(
"build-record options (none)\n"
  );
}

static void build_record_identify_by_order(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/build-records-by/order/%05u", cmd->build_record_order)
            }
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }
    STRING("identify")
  }
}

static void build_record_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/build-records/%.*s", cmd->build_record_id_len, cmd->build_record_id)
            }
          }
        }
        /* above selector selects the build_record shadow directory */
        { MAPPING STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("visit")
              STRING("record")
            }
            { MAPPING STRING("min-distance")
              UINT(0)
            }
            { MAPPING STRING("max-distance")
              UINT(0)
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void build_record_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->build_record_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, build_record_identify_by_order);
    return;
  }

  cmd->describe_id = command_post(cmd, client, build_record_describe);
}

static void build_record_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->identify_id_1 && msg->id != cmd->describe_id) {
    RTA; // wtf
  }
  if(msg->code != 0) {
    goto end;
  }

  if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->build_record_id_len = command_identify_result_parse(cmd->build_record_id, sizeof(cmd->build_record_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->build_record_id_len == 0) {
      /* lookup failure */
      if(cmd->build_record_id_len) {
        dprintf(2, "build_record not found : %.*s\n", (int)cmd->build_record_id_len, cmd->build_record_id);
      } else if(cmd->build_record_order != UINT16_MAX) {
        dprintf(2, "build_record not found : %05u\n", cmd->build_record_order);
      }
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, build_record_describe);
    }
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESULT) {
    if(cmd->any) {
      (void)!write(1, "\n", 1);
    }
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESPONSE) {
    goto end;
  } else {
    RTA;
  }
  return;

end:
  g_params.shutdown = true;
}

static void build_record_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

//
// builds
//

static void build_records_usage(command * restrict cmd)
{
  printf(
"\n"
"build-records options (none)\n"
  );
}

typeof(build_record_command) build_record_command = {
    name : "build-record"
  , usage : build_record_usage
  , connected : build_record_connected
  , process : build_record_process
  , disconnected : build_record_disconnected
};

typeof(build_records_command) build_records_command = {
    name : "build-records"
  , usage : build_records_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/build-records"
};
