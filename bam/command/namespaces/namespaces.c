/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"

#include "namespaces.h"
#include "params.h"

//
// namespace
//

static void namespace_usage(command * restrict cmd)
{
  printf(
"namespace options (none)\n"
  );
}

void namespace_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/namespaces-by/name/%.*s", cmd->namespace_name_len, cmd->namespace_name)
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

static void namespace_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/namespaces/%.*s", cmd->namespace_id_len, cmd->namespace_id)
            }
          }
        }
        /* above selector selects the namespace shadow directory */
        { MAPPING STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("min-distance")
              UINT(1)
            }
            { MAPPING STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->namespace_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, namespace_identify_by_name);
    return;
  }

  cmd->describe_id = command_post(cmd, client, namespace_describe);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->identify_id_1 && msg->id != cmd->describe_id) {
    RTA; // wtf
  }
  if(msg->code != 0) {
    goto end;
  }

  if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->namespace_id_len = command_identify_result_parse(cmd->namespace_id, sizeof(cmd->namespace_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->namespace_id_len == 0) {
      /* lookup failure */
      if(cmd->namespace_id_len) {
        dprintf(2, "namespace not found : %.*s\n", (int)cmd->namespace_id_len, cmd->namespace_id);
      } else if(cmd->namespace_name_len) {
        dprintf(2, "namespace not found : %.*s\n", (int)cmd->namespace_name_len, cmd->namespace_name);
      }
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, namespace_describe);
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

static void disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

//
// namespaces
//

static void list_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              STRING("//bm/namespaces")
            }
          }
        }
        /* listing of namespace directories */
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING
              STRING("nofollow")
              BOOL(false) // follow links
            }
            { MAPPING
              STRING("direction")
              STRING("down")
            }
            { MAPPING
              STRING("graph")
              STRING("fs")
            }
            { MAPPING
              STRING("min-distance")
              UINT(1)
            }
            { MAPPING
              STRING("max-distance")
              UINT(1)
            }
          }
        }
        /* upwards to the namespace objects */
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("min-distance")
              UINT(1)
            }
            { MAPPING STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }
    STRING("list")
  }
}

static void list_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->list_id = command_post(cmd, client, list_build);
}

static void namespaces_usage(command * restrict cmd)
{
  printf(
"\n"
"namespaces options (none)\n"
  );
}

typeof(namespace_command) namespace_command = {
    name : "namespace"
  , usage : namespace_usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};

typeof(namespaces_command) namespaces_command = {
    name : "namespaces"
  , usage : namespaces_usage
  , connected : list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
};
