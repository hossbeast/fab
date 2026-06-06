/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/ipc.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "show.h"
#include "args.h"
#include "command.h"
#include "info.h"
#include "params.h"

#include "zcat.h"

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"show positional args\n"
" <path>                target to show\n"
"\n"
"show options\n"
" stdin                 show stdin from the record\n"
" stdout                show stdout from the record\n"
" stderr                show stderr from the record\n"
" auxout                show auxout from the record\n"
  );
}

static void record_identify(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    command_context(builder_writer);
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              narrator_sayw(builder_writer->N, cmd->target, cmd->target_len);
              bacon_writer_pop_bytes(builder_writer);
            }
            { MAPPING STRING("nofollow")
              BOOL(cmd->nofollow)
            }
            { MAPPING STRING("hidden")
              BOOL(cmd->hidden)
            }
          }
        }
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            if(cmd->special == 0) {
              { MAPPING STRING("min-distance") UINT(0) }
              { MAPPING STRING("max-distance") UINT(0) }
            } else if(cmd->special == 1) {
              { MAPPING STRING("min-distance") UINT(0) }
              { MAPPING STRING("max-distance") UINT(1) }
            } else {
              { MAPPING STRING("min-distance") UINT(1) }
              { MAPPING STRING("max-distance") UINT(1) }
            }
          }
        }
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("down")
            }
            { MAPPING STRING("graph")
              STRING("records")
            }
            { MAPPING STRING("min-distance") UINT(2) }
            { MAPPING STRING("max-distance") UINT(2) }
          }
        }
        { MAPPING
          STRING("subset")
          { SET
            { MAPPING STRING("sort")
              { LIST
                { MAPPING STRING("order") STRING("ascending") }
                STRING("forktime")
              }
            }
            { MAPPING STRING("limit") UINT(1) }
          }
        }
      }
    }
    STRING("identify")
  }
}

static void record_tell(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  if(cmd->show_detail) {
    { LIST
      { MAPPING
        STRING("select")
        { LIST
          { MAPPING
            STRING("path")
            { SET
              { MAPPING
                STRING("pattern")
                LOADF("//bm/formula-records/%.*s/%.*s", cmd->id_len, cmd->id, (int)cmd->show_detail_len, cmd->show_detail);
              }
            }
          }
        }
      }
      STRING("tell")
    }
  } else {
    { LIST
      { MAPPING
        STRING("select")
        { LIST
          { MAPPING
            STRING("path")
            { SET
              { MAPPING
                STRING("pattern")
                LOADF("//bm/formula-records/%.*s", cmd->id_len, cmd->id)
              }
            }
          }
          { MAPPING
            STRING("traverse")
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
}
void command_show_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->identify_id_0 = command_post(cmd, client, record_identify);
}

void command_show_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->id_len = command_identify_result_parse(cmd->id, sizeof(cmd->id), msg);
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->id_len == 0) {
      dprintf(2, "no build record for found %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, record_tell);
    }
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
}

void command_show_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    (void)!write(1, "\n", 1);
  }
}

//
// public
//

typeof(show_command) show_command = {
    usage : usage
  , name : "show"
  , connected : command_show_connected
  , process : command_show_process
  , disconnected : command_show_disconnected

  /* defaults */
  , hidden : true
  , nofollow : true
};
