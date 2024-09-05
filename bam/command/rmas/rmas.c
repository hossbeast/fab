/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "rmas.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

//
// rma
//

static void rma_usage(command * restrict cmd)
{
  printf(
"rma options (none)\n"
  );
}

//
// rmas
//

static void rmas_usage(command * restrict cmd)
{
  printf(
"\n"
"rmas options (none)\n"
  );
}

//
// module rmas
//

static void module_rmas_usage(command * restrict cmd)
{
  printf(
"\n"
"module rmas options\n"
  );
}

static void module_rmas_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->target_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_dot);
  } else if(memmem(cmd->target, cmd->target_len, MMS("/")) != 0) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  } else {
    cmd->identify_id_0 = command_post(cmd, client, module_command_identify_id);
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  }

  message_no_error_list[0] = cmd->identify_id_0;
  message_no_error_list[1] = cmd->identify_id_1;
  message_no_error_list_len = 2;
}

static void module_rmas_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->size > 10) {
      cmd->module_id_len = command_identify_result_parse(msg, cmd->module_id, sizeof(cmd->module_id));
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->size > 10) {
      cmd->module_id_len = command_identify_result_parse(msg, cmd->module_id, sizeof(cmd->module_id));
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      snprintf(cmd->path, sizeof(cmd->path), "//modules/%.*s/rmas", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->size);
    cmd->any = true;
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
}

//
// public
//

typeof(rma_command) rma_command = {
    name : "rma"
  , usage : rma_usage
  , connected : special_describe_connected
  , process : special_describe_process
  , disconnected : special_describe_disconnected
  , prefix : "//rmas"
  , visit : "rma"
};

typeof(rmas_command) rmas_command = {
    name : "rmas"
  , usage : rmas_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//rmas"
  , visit : "rma"
};

typeof(module_rmas_command) module_rmas_command = {
    name : "rmas"
  , usage : module_rmas_usage
  , connected : module_rmas_connected
  , process : module_rmas_process
  , disconnected : special_list_disconnected
};
