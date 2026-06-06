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
  , graph_len : sizeof("fs") - 1
  , direction : "down"
  , direction_len : sizeof("down") - 1
  , nofollow : true
};
static struct tree_args *args = &tree_args;
static bool upwards;
static bool second;
static uint64_t list_id_0;
static uint64_t list_id_1;
static const char *tree_targets[64];
static uint16_t tree_targets_lens[64];
static uint16_t tree_targets_len;
static uint16_t tree_targets_x;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"ls/tree positional arguments\n"
" <path>                       select the subject node to start listings from\n"
"\n"
"ls/tree options\n"
" --graph | -g <graph-type>    graph to traverse\n"
" --visit | -v <visit-type>\n"
" --down             (default) from the subject node\n"
" --up                         upwards from the subject node\n"
" --both                       both upwards and downwards\n"
" --labels           (default) show label of visited nodes\n"
" --paths                      show full path of visited nodes\n"
" --min-distance <uint16>      min visit distance from subject (default = 0)\n"
"  --min-depth                 (alias)\n"
" --max-distance <uint16>      max visit distance from subject (default = MAX)\n"
"  --min-depth                 (alias)\n"
" --special                    also visit nodes attached via the specials graph\n"
" --no-special       (default) dont traverse into the specials graph\n"
" -s                           also visit nodes attached via the specials graph\n"
" -S                           only visit nodes attached via the specials graph\n"
" --hidden           (default) visit files not yet created\n"
" --no-hidden\n"
" --follow                     follow symlinks\n"
" --no-follow        (default) visit, dont follow, symlinks\n"
"\n"
"graph types\n"
" fs                 (default) filesystem tree\n"
" depends                      dependency graph\n"
" special                      specials graph\n"
" records                      formula records\n"
" uses                         module -> ruleset graph\n"
"\n"
"visit types (node type)\n"
" bpe\n"
" client\n"
" configset\n"
" directory\n"
" formula\n"
" fstree\n"
" module\n"
" plugin\n"
" record\n"
" rma\n"
" rule\n"
" ruleset\n"
" varset\n"
"\n"
"visit types (node status)\n"
" primary                      visit only primary files\n"
" invalid                      visit only invalid nodes\n"
);
}

static void post_request(bam_client * restrict client)
{
  bamipc_message * msg;
  const char *dir;
  const char *order;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  int x;

  dir = "down";
  order = "preorder";
  if(upwards) {
    dir = "up";
    order = "postorder";
  }

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  if(!second) {
    list_id_0 = msg->id = ++client->msgid;
  } else {
    list_id_1 = msg->id = ++client->msgid;
  }

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  { LIST
    command_context(builder_writer);
    { MAPPING STRING("select")
      { LIST
        for(x = 0; x < tree_targets_len; x++)
        {
          { MAPPING STRING("path")
            { SET
              { MAPPING STRING("pattern")
                bacon_writer_push_bytes(builder_writer);
                narrator_sayw(builder_writer->N, tree_targets[x], tree_targets_lens[x]);
                bacon_writer_pop_bytes(builder_writer);
              }
              { MAPPING STRING("nofollow")
                BOOL(args->nofollow)
              }
              { MAPPING STRING("hidden")
                BOOL(args->hidden)
              }
            }
          }
        }
        if(args->special == 0) {
          { MAPPING STRING("traverse")
            { SET
              { MAPPING STRING("nofollow")
                BOOL(args->nofollow)
              }
              { MAPPING STRING("hidden")
                BOOL(args->hidden)
              }
              { MAPPING STRING("direction")
                STRING(dir)
              }
              { MAPPING STRING("order")
                STRING(order)
              }
              { MAPPING STRING("graph")
                BYTES(args->graph, args->graph_len)
              }
              if(args->visit) {
                MAPPING STRING("visit")
                BYTES(args->visit, args->visit_len)
              }
              { MAPPING STRING("min-distance")
                UINT(args->min_distance)
              }
              { MAPPING STRING("max-distance")
                UINT(args->max_distance)
              }
            }
          }
        } else {
          { MAPPING
            STRING("traverse")
            { SET
              { MAPPING STRING("direction")
                STRING("up")
              }
              { MAPPING STRING("graph")
                STRING("special")
              }
              if(args->special == 1) {
                { MAPPING STRING("min-distance") UINT(0) }
                { MAPPING STRING("max-distance") UINT(1) }
              } else {
                { MAPPING STRING("min-distance") UINT(1) }
                { MAPPING STRING("max-distance") UINT(1) }
              }
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

  msg->len = fixed.l;
  client_publish(client, msg);
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  memcpy(tree_targets, args->targets, sizeof(tree_targets));
  memcpy(tree_targets_lens, args->targets_lens, sizeof(tree_targets_lens));
  if((tree_targets_len = args->targets_len) == 0) {
    tree_targets[0] = ".";
    tree_targets_lens[0] = 1;
    tree_targets_len = 1;
  }

  upwards = false;
  if(args->both) {
    upwards = true;
  } else if(memncmp(args->direction, args->direction_len, MMS("--up")) == 0) {
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
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("symlink")) == 0) {
    (void)!write(1, BOLD_RED);
  }

  if(show_paths) {
    dprintf(1, "%.*s", fsent->path_len, fsent->path);
  } else {
    dprintf(1, "%.*s", fsent->label_len, fsent->label);
  }

  if(memncmp(fsent->type, fsent->type_len, MMS("shadow-link"))) {
    /* shadow-links are not resolved so their state is not relevant */
    dprintf(1, " - %.*s %.*s"
      , (int)fsent->type_len, fsent->type
      , (int)fsent->state_len, fsent->state
    );
  }

  if(fsent->ref_len) {
    dprintf(1, " -> %.*s", fsent->ref_len, fsent->ref);
  }

  if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("dir")) == 0) {
    (void)!write(1, NOCOLOR);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-dir")) == 0) {
    (void)!write(1, NOCOLOR);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("shadow-link")) == 0) {
    (void)!write(1, NOCOLOR);
  } else if(g_args.no_color == 0 && memncmp(fsent->type, fsent->type_len, MMS("symlink")) == 0) {
    (void)!write(1, NOCOLOR);
  }

  dprintf(1, "\n");
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  size_t z;
  bam_list_item item;

  RUNTIME_ASSERT(msg->id == list_id_0 || msg->id == list_id_1);
  if(msg->code != 0) {
    if(msg->code == BAMIPC_MSG_ENOENT) {
      dprintf(2, "%-10s %.*s\n", "ENOENT", tree_targets_lens[tree_targets_x], tree_targets[tree_targets_x]);
    } else {
      dprintf(2, "%-10s %.*s\n", attrs32_name_byvalue(&bamipc_msg_code_attrs, msg->code), msg->len, msg->text);
    }
    goto end;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    if(args->both && !second) {
      upwards = false;
      second = true;
      post_request(client);
      return;
    }

    goto end;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  RUNTIME_ASSERT(msg->code == 0);

  z = 0;
  z += descriptor_type_unmarshal(&item, &descriptor_bam_list_item, msg->text, msg->len);
  RUNTIME_ASSERT(z == msg->len);

  fsent_show(&item, tree_args.show_paths);
  return;

end:
  g_params.shutdown = true;
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
  , name : "tree"
  , connected : connected
  , process : process

  /* defaults */
  , hidden : true
  , nofollow : true
};
