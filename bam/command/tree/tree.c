/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "common/color.h"
#include "bam/client.h"
#include "bam/ipc.h"
#include "bam/list.h"
#include "bam/events.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "tree.h"
#include "command.h"
#include "params.h"
#include "info.h"
#include "params.h"
#include "zcat.h"

struct tree_args tree_args = {
    graph : "fs"
  , graph_len : 2
  , direction : "down"
  , direction_len : 4
};
static struct tree_args *args = &tree_args;
static bool upwards;
static bool second;
static uint64_t requestid;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"ls options\n"
" -g <graph-type>           show this graph\n"
" --down                    downwards from the subject node\n"
" --up                      upwards from the subject node\n"
" --labels        (default) show labels\n"
" --paths                   show full paths\n"
"\n"
"graph types\n"
" fs              (default) filesystem tree\n"
" dirtree                   directories subset of filesystem tree\n"
" modtree                   modules subset of filesystem tree\n"
" depends                   dependency graph\n"
" imports                   module imports graph\n"
" uses                      module uses graph\n"
" requires                  module requires graph\n"
  );
}

//
// build
//

static void post_request(bam_client * restrict client)
{
  bamipc_message * msg;
  bead target;
  const char *dir;
  const char *order;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;

  target = args->target;
  if(target.len == 0) {
    target = (bead){ ".", 1 };
  }

  dir = "down";
  order = "preorder";
  if(upwards) {
    dir = "up";
    order = "postorder";
  }

  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    if(g_args.context_module_id_len) {
      MAPPING
      STRING("context")
      { SET
        { MAPPING
          STRING("module")
          BYTES(g_args.context_module_id, g_args.context_module_id_len)
        }
      }
    }
    { MAPPING
      STRING("select")
      { LIST
        { MAPPING
          STRING("lookup")
          { SET
            { MAPPING
              STRING("path")
              BYTES(target.s, target.len)
            }
            { MAPPING
              STRING("nofollow")
              BOOL(args->nofollow)
            }
            { MAPPING
              STRING("hidden")
              BOOL(args->hidden)
            }
          }
        }
        { MAPPING
          STRING("traverse")
          { SET
            { MAPPING
              STRING("nofollow")
              BOOL(true)
            }
            { MAPPING
              STRING("direction")
              STRING(dir)
            }
            { MAPPING
              STRING("order")
              STRING(order)
            }
            { MAPPING
              STRING("graph")
              BYTES(args->graph, args->graph_len)
            }
            if(args->visit) {
              MAPPING
              STRING("visit")
              BYTES(args->visit, args->visit_len)
            }
            { MAPPING
              STRING("min-distance")
              UINT(args->min_distance)
            }
            { MAPPING
              STRING("max-distance")
              UINT(args->max_distance)
            }
          }
        }
      }
    }
    STRING("tree-list")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->size = fixed.l;
  client_post(client, msg);
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  upwards = false;
  if(args->both) {
    upwards = true;
  } else if(memncmp(args->direction, args->direction_len, MMS("up")) == 0) {
    upwards = true;
  }

  post_request(client);
}

static void fsent_show(const bam_list_item * restrict fsent, bool show_paths)
{
  int x;

  for(x = 0; x < fsent->distance; x++) {
    dprintf(1, "|-- ");
  }

  if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("dir")) == 0) {
    (void)!write(1, YELLOW);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-dir")) == 0) {
    (void)!write(1, YELLOW);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-link")) == 0) {
    (void)!write(1, BOLD_RED);
  }

  if(show_paths) {
    dprintf(1, "%.*s", fsent->path_len, fsent->path);
  } else {
    dprintf(1, "%.*s", fsent->label_len, fsent->label);
  }

  if(fsent->ref_len) {
    dprintf(1, " -> %.*s", fsent->ref_len, fsent->ref);
  } else {
    dprintf(1, " - %.*s %.*s"
      , (int)fsent->type_len, fsent->type
      , (int)fsent->state_len, fsent->state
    );
  }

  if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("dir")) == 0) {
    (void)!write(1, NOCOLOR);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-dir")) == 0) {
    (void)!write(1, NOCOLOR);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-link")) == 0) {
    (void)!write(1, NOCOLOR);
  }

  dprintf(1, "\n");
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  size_t z;
  bam_list_item item;

  RUNTIME_ASSERT(msg->id == requestid);
  if(msg->code != 0) {
    goto end;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE && args->both && !second) {
    upwards = false;
    second = true;
    post_request(client);
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    goto end;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);

  z = 0;
  z += descriptor_type_unmarshal(&item, &descriptor_bam_list_item, msg->text, msg->size);
  RUNTIME_ASSERT(z == msg->size);

  fsent_show(&item, tree_args.show_paths);
  return;

end:
  g_params.shutdown = true;
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

void tree_init(uint32_t flags)
{
  /* setup defaults */
  tree_args.min_distance = 0;
  tree_args.max_distance = UINT16_MAX;

  if((flags & TREE_LS)) {
    tree_args.min_distance = 0;
    tree_args.max_distance = 1;
  }
}

//
// public
//

struct command tree_command = {
    usage : usage
  , attached : attached
  , connected : connected
  , process : process
};
