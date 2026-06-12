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

/*
 * The namespace command runs in up to two phases. When invoked with a name rather than
 * an id, the name is first resolved to an id (identify). The namespace is then looked
 * up by id and printed (describe). Each request elicits zero or more RESULT messages
 * carrying output, followed by exactly one RESPONSE message marking completion.
 */
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
  if(msg->id == cmd->identify_id_1) {
    /* identify : resolve namespace name -> id */
    if(!command_identify_msg(msg, cmd->namespace_id, sizeof(cmd->namespace_id), &cmd->namespace_id_len)) {
      return;
    }
    if(cmd->namespace_id_len == 0) {
      command_notfound("namespace", cmd->namespace_name, cmd->namespace_name_len);
      return;
    }
    cmd->describe_id = command_post(cmd, client, namespace_describe);
    return;
  }

  if(msg->id == cmd->describe_id) {
    /* describe : print the namespace */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
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
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              STRING("//bm/namespaces")
            }
          }
        }
        /* downwards to the per-namespace shadow directories */
        { MAPPING STRING("traverse")
          { SET
            { MAPPING STRING("nofollow")
              BOOL(false) // follow links
            }
            { MAPPING STRING("direction")
              STRING("down")
            }
            { MAPPING STRING("graph")
              STRING("fs")
            }
            { MAPPING STRING("min-distance")
              UINT(1)
            }
            { MAPPING STRING("max-distance")
              UINT(1)
            }
          }
        }
        /* upwards to the namespace objects themselves */
        { MAPPING STRING("traverse")
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
