/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "bam/ipc.h"
#include "bam/client.h"
#include "bam/events.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "common/parseint.h"

#include "rmas.h"
#include "info.h"
#include "params.h"
#include "event_parser/parser.h"
#include "zcat.h"

/* flex and bison do not agree on these names */
#define YYSTYPE yyu_lval
#define YYLTYPE yyu_location

#include "events.lex.h"
#include "events.states.h"
#include "events.tab.h"
#include "events.tokens.h"

struct rma_run_args rma_run_args = {
    .show_paths = true
  , .show_ids = false
};

static uint64_t eventsubid;

static YYU_VTABLE(events_vtable, events, events);

static events_parser parser;

static void __attribute__((constructor)) setup(void)
{
  yyu_parser_init(&parser.yyu, &events_vtable);
  yyu_parser_init_tokens(&parser.yyu, events_token_table, events_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&parser.yyu, events_numstates, events_statenumbers, events_statenames);
}

//
// rma
//

static void rma_usage(command * restrict cmd)
{
  printf(
"rma options (none)\n"
  );
}

static void module_rma_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/modules/%.*s/rmas-by/name/%.*s", cmd->module_id_len, cmd->module_id, cmd->rma_name_len, cmd->rma_name)
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

static void rma_identify_by_name(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rmas-by/name/%.*s", cmd->rma_name_len, cmd->rma_name)
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

static void rma_describe(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rmas/%.*s", cmd->rma_id_len, cmd->rma_id)
            }
            /* above selector selects shadow link to the rma */
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }
    STRING("describe")
  }
}

static void rma_run(struct command * restrict cmd, bacon_writer * restrict builder_writer)
{
  { LIST
    { MAPPING STRING("select")
      { LIST
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              LOADF("//bm/rmas/%.*s", cmd->rma_id_len, cmd->rma_id)
            }
            { MAPPING STRING("nofollow")
              BOOL(false)
            }
          }
        }
      }
    }
    STRING("invalidate")
    STRING("reconcile")
  }
}

static void module_rma_run_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message *msg;

  /* subscribe to relevant events */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_SYSTEM_STATE_CHANGE  // reconcile errors
    | BAMIPC_EVENT_RMA_START
    | BAMIPC_EVENT_RMA_END
    | BAMIPC_EVENT_RMA_SEARCH
    | BAMIPC_EVENT_RMA_GENERATE
    | BAMIPC_EVENT_RMA_RESOLVE
    | BAMIPC_EVENT_RMA_BIND
    ;
  client_publish(client, msg);

  module_command_connected(cmd, client);
}

static void rma_run_connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message *msg;

  /* subscribe to relevant events */
  msg = client_tx(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_SYSTEM_STATE_CHANGE  // reconcile errors
    | BAMIPC_EVENT_RMA_START
    | BAMIPC_EVENT_RMA_END
    | BAMIPC_EVENT_RMA_SEARCH
    | BAMIPC_EVENT_RMA_GENERATE
    | BAMIPC_EVENT_RMA_RESOLVE
    | BAMIPC_EVENT_RMA_BIND
    ;
  client_publish(client, msg);

  if(cmd->rma_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, rma_identify_by_name);
  } else {
    cmd->run_id = command_post(cmd, client, rma_run);
  }
}

static void rma_run_event_process(const bamipc_message * restrict msg)
{
  int r;

  if((r = yyu_parse(&parser.yyu, msg->text, msg->len + 2, "-yyu-", YYU_INPLACE, 0, 0))) {
    fprintf(stderr, "%.*s\n", parser.yyu.error_len, parser.yyu.error_str);
    return;
  }

  printf("%*s%-3u %-15.*s"
    , parser.level, ""
    , parser.level
    , parser.type_len, parser.type
  );
  if(memncmp(parser.type, parser.type_len, MMS("rma-start")) == 0) {
    printf(" %.*s %.*s"
      , parser.id_len, parser.id
      , parser.path_len, parser.path
    );
  } else if(memncmp(parser.type, parser.type_len, MMS("rma-end")) == 0) {
    printf(" in %.*s", parser.runtime_len, parser.runtime);
  } else if(memncmp(parser.type, parser.type_len, MMS("rma-generate")) == 0) {
    if(rma_run_args.show_ids) {
      printf(" %.*s", parser.id_len, parser.id);
    }
    if(rma_run_args.show_paths) {
      printf(" %.*s", parser.path_len, parser.path);
    }
  } else if(memncmp(parser.type, parser.type_len, MMS("rma-search")) == 0) {
    if(rma_run_args.show_ids) {
      printf(" %.*s", parser.id_len, parser.id);
    }
    if(rma_run_args.show_paths) {
      printf(" %.*s", parser.path_len, parser.path);
    }
  } else if(memncmp(parser.type, parser.type_len, MMS("rma-resolve")) == 0) {
    if(parser.hit.id_len) {
      printf(" %4s %.*s", "hit", parser.hit.path_len, parser.hit.path);
    } else {
      printf(" %4s %.*s", "miss", parser.miss.path_len, parser.miss.path);
    }
  } else if(memncmp(parser.type, parser.type_len, MMS("rma-bind")) == 0) {
    if(rma_run_args.show_ids) {
      printf(" %.*s", parser.target.id_len, parser.target.id);
    }
    if(rma_run_args.show_paths) {
      printf(" %.*s", parser.target.path_len, parser.target.path);
    }
    printf(" -> ");
    if(rma_run_args.show_ids) {
      printf(" %.*s", parser.attachment.id_len, parser.attachment.id);
    }
    if(rma_run_args.show_paths) {
      printf(" %.*s", parser.attachment.path_len, parser.attachment.path);
    }
  }
  printf("\n");
}

static void rma_run_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->code != 0) {
    g_params.shutdown = true;
    return;
  }

  if(msg->type == BAMIPC_MSG_EVENTS) {
    switch(msg->evtype) {
      case BAMIPC_EVENT_RMA_START:
      case BAMIPC_EVENT_RMA_SEARCH:
      case BAMIPC_EVENT_RMA_GENERATE:
      case BAMIPC_EVENT_RMA_END:
      case BAMIPC_EVENT_RMA_RESOLVE:
      case BAMIPC_EVENT_RMA_BIND:
        rma_run_event_process(msg);
    }

    if(msg->evtype == BAMIPC_EVENT_RMA_END) {
      g_params.shutdown = true;
    }
  } else if(msg->id == eventsubid && msg->type == BAMIPC_MSG_RESULT) {
    /* eventsub */
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->rma_id_len = command_identify_result_parse(cmd->rma_id, sizeof(cmd->rma_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->rma_id_len == 0) {
      /* lookup failure */
      dprintf(2, "rma not found : %.*s\n", (int)cmd->rma_name_len, cmd->rma_name);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->run_id = command_post(cmd, client, rma_run);
    }
  } else if(msg->id == cmd->run_id) {
    if(cmd->any) {
      (void)!write(1, "\n", 1);
    }
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else {
    RTA;
  }
}

static void rma_connected(command * restrict cmd, bam_client * restrict client)
{
  if(cmd->rma_id_len == 0) {
    cmd->identify_id_1 = command_post(cmd, client, rma_identify_by_name);
    return;
  }

  cmd->describe_id = command_post(cmd, client, rma_describe);
}

static void rma_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != cmd->identify_id_1 && msg->id != cmd->describe_id) {
    RTA; // wtf
  }
  if(msg->code != 0) {
    goto end;
  }

  if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->rma_id_len = command_identify_result_parse(cmd->rma_id, sizeof(cmd->rma_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->rma_id_len == 0) {
      /* lookup failure */
      if(cmd->rma_id_len) {
        dprintf(2, "rma not found : %.*s\n", (int)cmd->rma_id_len, cmd->rma_id);
      } else if(cmd->rma_name_len) {
        dprintf(2, "rma not found : %.*s\n", (int)cmd->rma_name_len, cmd->rma_name);
      }
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, rma_describe);
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

static void rma_disconnected(command * restrict cmd)
{
  if(cmd->any) {
    printf("\n");
  }
}

//
// rma run
//

static void rma_run_usage(command * restrict cmd)
{
  printf(
"rma run options (none)\n"
  );
}

//
// rmas
//

static void rmas_usage(command * restrict cmd)
{
  printf(
"rmas options (none)\n"
  );
}

//
// module rma
//

static void module_rma_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->code != 0) {
    g_params.shutdown = true;
    return;
  }

  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found : %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else if(cmd->rma_id_len == 0) {
      cmd->identify_id_1 = command_post(cmd, client, module_rma_identify_by_name);
    } else {
      cmd->describe_id = command_post(cmd, client, rma_describe);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->rma_id_len = command_identify_result_parse(cmd->rma_id, sizeof(cmd->rma_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->rma_id_len == 0) {
      /* lookup failure */
      dprintf(2, "rma not found : %.*s\n", (int)cmd->rma_name_len, cmd->rma_name);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->describe_id = command_post(cmd, client, rma_describe);
    }
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESULT) {
    if(cmd->any) {
      (void)!write(1, "\n", 1);
    }
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->describe_id && msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
  } else {
    RTA;
  }
}

static void module_rma_usage(command * restrict cmd)
{
  printf(
"module rma options (none)\n"
  );
}

//
// module rma run
//

static void module_rma_run_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->code != 0) {
    g_params.shutdown = true;
    return;
  }

  if(msg->type == BAMIPC_MSG_EVENTS) {
    switch(msg->evtype) {
      case BAMIPC_EVENT_RMA_START:
      case BAMIPC_EVENT_RMA_SEARCH:
      case BAMIPC_EVENT_RMA_GENERATE:
      case BAMIPC_EVENT_RMA_END:
      case BAMIPC_EVENT_RMA_RESOLVE:
      case BAMIPC_EVENT_RMA_BIND:
        rma_run_event_process(msg);
    }

    if(msg->evtype == BAMIPC_EVENT_RMA_END) {
      g_params.shutdown = true;
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found : %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else if(cmd->rma_id_len == 0) {
      cmd->identify_id_1 = command_post(cmd, client, module_rma_identify_by_name);
    } else {
      cmd->run_id = command_post(cmd, client, rma_run);
    }
  } else if(msg->id == eventsubid && msg->type == BAMIPC_MSG_RESULT) {
    /* eventsub */
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->rma_id_len = command_identify_result_parse(cmd->rma_id, sizeof(cmd->rma_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->rma_id_len == 0) {
      /* lookup failure */
      dprintf(2, "rma not found : %.*s\n", (int)cmd->rma_name_len, cmd->rma_name);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      cmd->run_id = command_post(cmd, client, rma_run);
    }
  } else if(msg->id == cmd->run_id && msg->type == BAMIPC_MSG_RESULT) {
    if(cmd->any) {
      (void)!write(1, "\n", 1);
    }
    (void)!write(1, msg->text, msg->len);
    cmd->any = true;
  } else if(msg->id == cmd->run_id && msg->type == BAMIPC_MSG_RESPONSE) {
    /* end with rma-end event */
  } else {
    RTA;
  }
}

static void module_rma_run_usage(command * restrict cmd)
{
  printf(
"module rma options (none)\n"
  );
}

//
// module rmas
//

static void module_rmas_usage(command * restrict cmd)
{
  printf(
"module rmas options\n"
  );
}

static void module_rmas_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESULT) {
    if(msg->code == 0 && msg->len > 10) {
      cmd->module_id_len = command_identify_result_parse(cmd->module_id, sizeof(cmd->module_id), msg);
    }
  } else if(msg->id == cmd->identify_id_0 && msg->type == BAMIPC_MSG_RESPONSE) {
    cmd->identify_id_1 = command_post(cmd, client, module_command_identify_target);
  } else if(msg->id == cmd->identify_id_1 && msg->type == BAMIPC_MSG_RESPONSE) {
    if(cmd->module_id_len == 0) {
      /* lookup failure */
      dprintf(2, "module not found %.*s\n", (int)cmd->target_len, cmd->target);
      g_params.waserror = true;
      g_params.shutdown = true;
    } else {
      snprintf(cmd->path, sizeof(cmd->path), "//bm/modules/%.*s/rmas", cmd->module_id_len, cmd->module_id);
      cmd->list_id = command_post(cmd, client, special_list_build);
    }
  } else if(msg->id == cmd->list_id && msg->type == BAMIPC_MSG_RESULT) {
    (void)!write(1, msg->text, msg->len);
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
  , connected : rma_connected
  , process : rma_process
  , disconnected : rma_disconnected
};

typeof(rma_run_command) rma_run_command = {
    name : "rma run"
  , usage : rma_run_usage
  , connected : rma_run_connected
  , process : rma_run_process
  , disconnected : rma_disconnected
};

typeof(rmas_command) rmas_command = {
    name : "rmas"
  , usage : rmas_usage
  , connected : special_list_connected
  , process : special_list_process
  , disconnected : special_list_disconnected
  , path : "//bm/rmas"
  , visit : "rma"
};

/* by module */

typeof(rma_command) module_rma_command = {
    name : "rma"
  , usage : module_rma_usage
  , connected : module_command_connected    // first, identify the module
  , process : module_rma_process
  , disconnected : rma_disconnected
};

typeof(module_rma_run_command) module_rma_run_command = {
    name : "rma run"
  , usage : module_rma_run_usage
  , connected : module_rma_run_connected    // first, identify the module
  , process : module_rma_run_process
  , disconnected : rma_disconnected
};

typeof(module_rmas_command) module_rmas_command = {
    name : "rmas"
  , usage : module_rmas_usage
  , connected : module_command_connected    // first, identify the module
  , process : module_rmas_process
  , disconnected : special_list_disconnected
};
