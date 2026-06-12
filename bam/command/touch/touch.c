/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include "bam/client.h"
#include "bam/ipc.h"
#include "narrator/fixed.h"
#include "bacon/builder.h"

#include "touch.h"
#include "command.h"
#include "params.h"

static uint64_t requestid;
struct touch_args touch_args = {
    .recursive = true           /* defaults */
  , .cross_module = true
  , .cross_fstree = true
  , .min_distance = 0
  , .max_distance = UINT16_MAX
  , .nofollow = false
  , .primary = false
  , .hidden = true
};
static struct touch_args *args = &touch_args;
static bool any;

//
// static
//

static void usage(command * restrict cmd)
{
  printf(
"bam touch - invalidate nodes\n"
"\n"
"usage: bam touch <path>... [option]...\n"
" <path>                         select nodes to visit\n"
"if no targets are specified, touch primary files across all modules\n"
"\n"
"touch options\n"
" --hidden           (default) visit files not yet created\n"
" --no-hidden\n"
" --follow           (default) follow symlinks\n"
" --no-follow                  visit, dont follow, symlinks\n"
" --recursive | -R             traverse directories recursively\n"
" --no-recursive\n"
"\n"
"recursive options\n"
" --primary                    select only primary files\n"
" --no-primary    (default)    select even non-primary files\n"
" --cross-module  (default)    traverse across module boundaries\n"
" --no-cross-module\n"
" --cross-fstree  (default)    traverse across cross fstree boundaries\n"
" --no-cross-fstree\n"
" --min-distance <uint16>      min visit distance from subject (default = 0)\n"
"  --min-depth                 (alias)\n"
" --max-distance <uint16>      max visit distance from subject (default = MAX)\n"
"  --max-depth                 (alias)\n"
  );
}

//
// build
//

static void connected(command * restrict cmd, bam_client * restrict client)
{
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;
  narrator *N;
  narrator_fixed fixed;
  bool primary;
  bool nofollow;
  int x;

  msg = client_tx(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = ++client->msgid;

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, N);

  primary = args->primary;
  nofollow = args->nofollow;
  if(args->targets_len == 0) {
    primary = true;
    nofollow = true;
  }

  { LIST
    { MAPPING STRING("select")
      { LIST
        /* if no targets specified, start from all modules anchor dir */
        if(args->targets_len == 0) {
          { MAPPING STRING("search")
            { SET
              { MAPPING STRING("pattern")
                bacon_writer_push_bytes(builder_writer);
                narrator_says(builder_writer->N, "//bm/modules/*/anchor+(/)");
                bacon_writer_pop_bytes(builder_writer);
              }
            }
          }
        }
        for(x = 0; x < args->targets_len; x++)
        { MAPPING STRING("path")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              narrator_sayw(builder_writer->N, args->targets[x], args->targets_lens[x]);
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
        if(args->recursive) {
          { MAPPING STRING("traverse")
            { SET
              { MAPPING STRING("graph")
                STRING("fs")
              }
              { MAPPING STRING("cross-module")
                BOOL(args->cross_module)
              }
              { MAPPING STRING("cross-fstree")
                BOOL(args->cross_fstree)
              }
              { MAPPING STRING("min-distance")
                UINT(args->min_distance)
              }
              { MAPPING STRING("max-distance")
                UINT(args->max_distance)
              }
              if(primary) {
                { MAPPING STRING("visit")
                  STRING("primary")
                }
              }
              { MAPPING STRING("nofollow")
                BOOL(nofollow)
              }
              { MAPPING STRING("hidden")
                BOOL(args->hidden)
              }
            }
          }
        }
      }
    }
    STRING("invalidate")
  }

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(N, 0);
  narrator_sayc(N, 0);

  msg->len = fixed.l;
  client_publish(client, msg);
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->id != requestid) {
    return;
  }

  if(msg->type == BAMIPC_MSG_RESPONSE) {
    g_params.shutdown = true;
    return;
  }

  RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
  if(msg->code != 0) {
    return;
  }

  if(any) {
    (void)!write(1, "\n", 1);
  }
  (void)!write(1, msg->text, msg->len);
  any = true;
}

static void disconnected(command * restrict cmd)
{
  if(any) {
    (void)!write(1, "\n", 1);
  }
}

struct command touch_command = {
    usage : usage
  , name : "touch"
  , connected : connected
  , process : process
  , disconnected : disconnected
};
