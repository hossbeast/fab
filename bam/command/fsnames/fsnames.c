/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/builder.h"
#include "bam/client.h"
#include "bam/ipc.h"

#include "fsnames.h"
#include "params.h"
#include "namespaces.h"

//
// usage
//

static void fsname_usage(command * restrict cmd)
{
  printf(
"fsname options (none)\n"
  );
}

static void namespace_fsname_usage(command * restrict cmd)
{
  printf(
"namespace fsname options (none)\n"
  );
}

static void fsnames_usage(command * restrict cmd)
{
  printf(
"fsnames options (none)\n"
  );
}

static void namespace_fsnames_usage(command * restrict cmd)
{
  printf(
"namespace fsnames options (none)\n"
  );
}

//
// static
//

static void fsname_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        {
          MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/fsnames/%.*s", cmd->fsname_id_len, cmd->fsname_id);
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void fsname_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->describe_id) {
    /* describe : print the fsname */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

static void fsname_connected(command * restrict cmd, bam_client * restrict client)
{
  cmd->describe_id = command_post(cmd, client, fsname_describe);
}

static void fsname_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

static void namespace_fsname_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/namespaces/%.*s/fsnames-by/name/%.*s", cmd->namespace_id_len, cmd->namespace_id, cmd->fsname_name_len, cmd->fsname_name)
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

static void namespace_fsname_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->namespace_id_len == 0) {
    cmd->identify_id_0 = command_post(cmd, client, namespace_identify_by_name);
  } else if(cmd->fsname_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, namespace_fsname_identify_by_name);
  } else {
    cmd->list_id = command_post(cmd, client, fsname_describe);
  }
}

static void namespace_fsname_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0) {
    /* identify : resolve namespace name -> id */
    if(!command_identify_msg(msg, cmd->namespace_id, sizeof(cmd->namespace_id), &cmd->namespace_id_len)) {
      return;
    }
    if(cmd->namespace_id_len == 0) {
      command_notfound("namespace", cmd->namespace_name, cmd->namespace_name_len);
    } else if(cmd->fsname_id_len == 0) {
      cmd->identify_id_1 = command_post(cmd, client, namespace_fsname_identify_by_name);
    } else {
      cmd->list_id = command_post(cmd, client, fsname_describe);
    }
    return;
  }

  if(msg->id == cmd->identify_id_1) {
    /* identify : resolve fsname name -> id */
    if(!command_identify_msg(msg, cmd->fsname_id, sizeof(cmd->fsname_id), &cmd->fsname_id_len)) {
      return;
    }
    if(cmd->fsname_id_len == 0) {
      command_notfound("fsname", cmd->fsname_name, cmd->fsname_name_len);
      return;
    }
    cmd->list_id = command_post(cmd, client, fsname_describe);
    return;
  }

  if(msg->id == cmd->list_id) {
    /* describe : print the fsname */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

static void namespace_fsnames_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->namespace_id_len == 0) {
    cmd->identify_id = command_post(cmd, client, namespace_identify_by_name);
  } else {
    snprintf(cmd->path, sizeof(cmd->path), "//bm/namespaces/%.*s/fsnames", cmd->namespace_id_len, cmd->namespace_id);
    cmd->list_id = command_post(cmd, client, special_list_build);
  }
}

static void namespace_fsnames_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id) {
    /* identify : resolve namespace name -> id */
    if(!command_identify_msg(msg, cmd->namespace_id, sizeof(cmd->namespace_id), &cmd->namespace_id_len)) {
      return;
    }
    if(cmd->namespace_id_len == 0) {
      command_notfound("namespace", cmd->namespace_name, cmd->namespace_name_len);
      return;
    }
    snprintf(cmd->path, sizeof(cmd->path), "//bm/namespaces/%.*s/fsnames", cmd->namespace_id_len, cmd->namespace_id);
    cmd->list_id = command_post(cmd, client, special_list_build);
    return;
  }

  if(msg->id == cmd->list_id) {
    /* list : print the namespace fsnames */
    command_describe_msg(cmd, msg);
    return;
  }

  RTA;
}

typeof(namespace_fsname_command) fsname_command = {
    name : "fsname"
  , usage : fsname_usage
  , connected : fsname_connected
  , process : fsname_process
  , disconnected : fsname_disconnected
};

typeof(namespace_fsnames_command) fsnames_command = {
    name : "fsnames"
  , usage : fsnames_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/fsnames"
};

typeof(namespace_fsname_command) namespace_fsname_command = {
    name : "namespace fsname"
  , usage : namespace_fsname_usage
  , connected : namespace_fsname_connected
  , process : namespace_fsname_process
  , disconnected : special_list_disconnected
  , prefix : "//bm/fsnames"
};

typeof(namespace_fsnames_command) namespace_fsnames_command = {
    name : "namespace fsnames"
  , usage : namespace_fsnames_usage
  , connected : namespace_fsnames_connected
  , process : namespace_fsnames_process
  , disconnected : special_list_disconnected
};
