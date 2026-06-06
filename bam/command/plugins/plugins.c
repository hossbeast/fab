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

static void plugin_stdin_usage(command * restrict cmd)
{
  printf(
"plugin stdin options (none)\n"
  );
}

static void plugin_stdout_usage(command * restrict cmd)
{
  printf(
"plugin stdout options (none)\n"
  );
}

static void plugin_stderr_usage(command * restrict cmd)
{
  printf(
"plugin stderr options (none)\n"
  );
}

static void plugin_auxout_usage(command * restrict cmd)
{
  printf(
"plugin auxout options (none)\n"
  );
}

static void plugin_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              if(cmd->id_len) {
                LOADF("//bm/plugins/%.*s", cmd->id_len, cmd->id)
              } else {
                LOADF("//bm/plugins-by/name/%.*s", cmd->target_len, cmd->target);
              }
            }
          }
        }
        { MAPPING STRING("traverse")
          { SET
            { MAPPING STRING("direction")
              STRING("up")
            }
            { MAPPING STRING("graph")
              STRING("special")
            }
            { MAPPING STRING("visit")
              STRING(cmd->visit)
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

static void plugin_detail_build(command * restrict cmd, bacon_writer * restrict builder_writer, const char * restrict name)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/plugins/%.*s/%s", cmd->id_len, cmd->id, name)
            }
          }
        }
      }
    }
    STRING("tell")
  }
}

static void plugin_stdin_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  plugin_detail_build(cmd, builder_writer, "stdin");
}

static void plugin_stdout_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  plugin_detail_build(cmd, builder_writer, "stdout");
}

static void plugin_stderr_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  plugin_detail_build(cmd, builder_writer, "stderr");
}

static void plugin_auxout_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  plugin_detail_build(cmd, builder_writer, "auxout");
}

static void plugin_connected(command * restrict cmd, bam_client * restrict client)
{
	command_post(cmd, client, plugin_build);
}

static void plugin_stdin_connected(command * restrict cmd, bam_client * restrict client)
{
	command_post(cmd, client, plugin_stdin_build);
}

static void plugin_stdout_connected(command * restrict cmd, bam_client * restrict client)
{
	command_post(cmd, client, plugin_stdout_build);
}

static void plugin_stderr_connected(command * restrict cmd, bam_client * restrict client)
{
	command_post(cmd, client, plugin_stderr_build);
}

static void plugin_auxout_connected(command * restrict cmd, bam_client * restrict client)
{
	command_post(cmd, client, plugin_auxout_build);
}

static void plugin_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
	if(msg->type == BAMIPC_MSG_RESPONSE) {
		g_params.shutdown = true;
		return;
	}

	RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
	(void)!write(1, msg->text, msg->len);
	cmd->any = true;
}

static void plugin_detail_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
	if(msg->type == BAMIPC_MSG_RESPONSE) {
		g_params.shutdown = true;
		return;
	}

	RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  if(cmd->any) {
    (void)!write(1, "\n", 1);
  }
	(void)!write(1, msg->text, msg->len);
	cmd->any = true;
}

static void disconnected(command * restrict cmd)
{
	if(cmd->any) {
    (void)!write(1, "\n", 1);
	}
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

static void plugins_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING
              STRING("pattern")
              STRING("//bm/plugins")
            }
          }
        }
        /* listing of module directories */
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
        /* upwards to the modules themselves */
        { MAPPING STRING("traverse")
          { SET
            { MAPPING
              STRING("nofollow")
              BOOL(false) // follow links
            }
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
              UINT(1)
            }
            { MAPPING
              STRING("max-distance")
              UINT(1)
            }
          }
        }
      }
    }
    STRING("list")
  }
}

static void plugins_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->list_id = command_post(cmd, client, plugins_build);
}

typeof(plugins_command) plugins_command = {
    name : "plugins"
  , usage : plugins_usage
  , connected : plugins_connected
  , process : special_list_process
  , disconnected : disconnected
  , path : "//bm/plugins"
  , visit : "plugin"
};

typeof(plugin_command) plugin_command = {
    name : "plugin"
  , usage : plugin_usage
  , connected : plugin_connected
  , process : plugin_process
  , disconnected : special_describe_disconnected
  , prefix : "//bm/plugins"
  , visit : "plugin"
};

typeof(plugin_stdin_command) plugin_stdin_command = {
    name : "plugin-stdin"
  , usage : plugin_stdin_usage
  , connected : plugin_stdin_connected
  , process : plugin_detail_process
};

typeof(plugin_stdout_command) plugin_stdout_command = {
    name : "plugin-stdout"
  , usage : plugin_stdout_usage
  , connected : plugin_stdout_connected
  , process : plugin_detail_process
};

typeof(plugin_stderr_command) plugin_stderr_command = {
    name : "plugin-stderr"
  , usage : plugin_stderr_usage
  , connected : plugin_stderr_connected
  , process : plugin_detail_process
};

typeof(plugin_auxout_command) plugin_auxout_command = {
    name : "plugin-auxout"
  , usage : plugin_auxout_usage
  , connected : plugin_auxout_connected
  , process : plugin_detail_process
};
