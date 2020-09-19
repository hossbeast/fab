/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <inttypes.h>
#include <getopt.h>
#include <signal.h>
#include <wait.h>

#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "narrator.h"
#include "narrator/units.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "logger/arguments.h"
#include "yyutil/box.h"

#include "build.h"
#include "args.h"
#include "command.h"
#include "errtab/MAIN.errtab.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "config.internal.h"
#include "fab/build_slot.h"
#include "fab/build_slot.desc.h"

#include "common/attrs.h"
#include "macros.h"
#include "params.h"
#include "common/hash.h"
#include "common/assure.h"
#include "common/color.h"

struct build_config {
  struct show_settings {
    bool show_path;
    bool show_arguments;
    bool show_command;
    bool show_cwd;
    bool show_sources;
    bool show_targets;
    bool show_environment;
    bool show_status;

    bool show_stdout;
    int16_t show_stdout_limit_lines;
    int16_t show_stdout_limit_bytes;

    bool show_stderr;
    int16_t show_stderr_limit_lines;
    int16_t show_stderr_limit_bytes;

    bool show_auxout;
    int16_t show_auxout_limit_lines;
    int16_t show_auxout_limit_bytes;
  } error;

  struct show_settings success;
} build_config;

static struct {
  list * targets;
} args;

typedef struct build_slot {
  fab_build_slot base;
  llist lln;

  char *stdout_text;
  size_t stdout_alloc;
  uint32_t stdout_len;

  char *stderr_text;
  size_t stderr_alloc;
  uint32_t stderr_len;

  char *auxout_text;
  size_t auxout_alloc;
  uint32_t auxout_len;
} build_slot;

static llist slots_freelist = LLIST_INITIALIZER(slots_freelist);
static hashtable * build_slots_bypid;

//
// static
//

static uint32_t build_slot_hash(uint32_t h, const void * e, size_t el)
{
  const build_slot * const * bsp = e;

  return hash32(h, &(*bsp)->base.pid, sizeof((*bsp)->base.pid));
}

static int build_slot_cmp(const void * A, size_t Al, const void * B, size_t Bl)
{
  const build_slot * const * Absp = A;
  const build_slot * const * Bbsp = B;

  return INTCMP((*Absp)->base.pid, (*Bbsp)->base.pid);
}

static uint32_t build_slot_key_hash(uint32_t h, const void *key, size_t el)
{
  return hash32(h, key, el);
}

static int build_slot_key_cmp(const void * A, const void *key, size_t keysz)
{
  const build_slot * const * Absp = A;
  const uint32_t * pidp = key;

  return INTCMP((*Absp)->base.pid, *pidp);
}

static void build_slot_release(void *e)
{
  build_slot **bsp = e;
  build_slot *bs = *bsp;

//printf("[%8d] RELEASE %p\n", bs->base.pid, bs);
  llist_append(&slots_freelist, bs, lln);
}

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;
  uint32_t tail;

  /* subscribe to relevant events */
  msg = fab_client_produce(client, &tail);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->attrs = 0
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_FORKED - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDOUT - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_STDERR - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_AUXOUT - 1))
    | (1 << (FABIPC_EVENT_FORMULA_EXEC_WAITED - 1))
    | (1 << (FABIPC_EVENT_STDOUT - 1))
    | (1 << (FABIPC_EVENT_STDERR - 1))
    ;
  fab_client_post(client, tail);

  /* send the request */
  msg = fab_client_produce(client, &tail);
  msg->type = FABIPC_MSG_REQUEST;

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(build_command_request_collate, request_narrator);
  fatal(narrator_xsays, request_narrator, " run");

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;

  fab_client_post(client, tail);

  finally : coda;
}

static xapi slot_alloc(build_slot ** slotp, int pid)
{
  enter;

  build_slot *bs;

  if((bs = llist_shift(&slots_freelist, typeof(*bs), lln)) == 0)
  {
    fatal(xmalloc, &bs, sizeof(*bs));
  }
  else
  {
    bs->base.status = 0;
    bs->base.stage = 0;
    bs->base.path_len = 0;
    bs->base.pwd_len = 0;
    bs->base.args_len = 0;
    bs->base.envs_len = 0;
    bs->base.sources_len = 0;
    bs->base.targets_len = 0;
    bs->base.stdout_total = 0;
    bs->base.stderr_total = 0;
    bs->base.auxout_total = 0;
    bs->stdout_len = 0;
    bs->stderr_len = 0;
    bs->auxout_len = 0;
  }

  bs->base.pid = pid;

  *slotp = bs;

  finally : coda;
}

static void slot_free(build_slot *bs)
{
  if(bs)
  {
    wfree(bs->stdout_text);
    wfree(bs->stderr_text);
    wfree(bs->auxout_text);
  }

  free(bs);
}

static xapi slot_print(build_slot *bs)
{
  enter;

  narrator *N = g_narrator_stdout;
  fab_build_slot *slot = &bs->base;
  int exit;
  int sig;
  bool core;
  bool success;
  struct {
    char *b;
    size_t len;
  } color;
  uint16_t blen;
  int x;
  int lines;
  uint16_t yo;

  exit = 0;
  sig = 0;
  core = false;
  if(WIFEXITED(slot->status))
  {
    exit = WEXITSTATUS(slot->status);
  }
  else if(WIFSIGNALED(slot->status))
  {
    sig = WTERMSIG(slot->status);
    core = WCOREDUMP(slot->status);
  }

  success = true;
  if(slot->status != 0 || slot->stderr_total > 0) {
    success = false;
  }

  color = (typeof(color)){ 0 };
  if(success)
    color = (typeof(color)){ GREEN };
  else
    color = (typeof(color)){ RED };

  if(color.len) {
    write(1, color.b, color.len);
  }

  xsayf("%2d %.*s -> ", slot->stage, (int)slot->args->len[0], &slot->args->s[2]);
  yo = 0;
  for(x = 0; x < slot->targets_len; x++)
  {
    if(x) {
      xsays(", ");
    }
    xsayw(&slot->targets->s[yo+2], slot->targets->len[yo/2]);
    yo += slot->targets->len[yo/2] + 2;
    yo += (yo & 1);
  }
  xsays("\n");

  if((success && build_config.success.show_path) || (!success && build_config.error.show_path))
  {
    xsayf(" path %.*s\n", (int)slot->path_len, slot->path);
  }
  if((success && build_config.success.show_command) || (!success && build_config.error.show_command))
  {
    yo = 0;
    for(x = 0; x < slot->args_len; x++)
    {
      if(x) {
        xsays(" ");
      }
      xsayw(&slot->args->s[yo+2], slot->args->len[yo/2]);
      yo += slot->args->len[yo/2] + 2;
      yo += (yo & 1);
    }
    xsays("\n");
  }
  if((success && build_config.success.show_environment) || (!success && build_config.error.show_environment))
  {
    xsayf(" envp %2d\n", slot->envs_len);
    yo = 0;
    for(x = 0; x < slot->envs_len; x++)
    {
      xsayf("  [%2d] %.*s\n", x, (int)slot->envs->len[yo/2], &slot->envs->s[yo+2]);
      yo += slot->envs->len[yo/2] + 2;
      yo += (yo & 1);
    }
  }
  if((success && build_config.success.show_arguments) || (!success && build_config.error.show_arguments))
  {
    xsayf(" argv %2d\n", slot->args_len);
    yo = 0;
    for(x = 0; x < slot->args_len; x++)
    {
      xsayf(" [%2d] %.*s\n", x, (int)slot->args->len[yo/2], &slot->args->s[yo+2]);
      yo += slot->args->len[yo/2] + 2;
      yo += (yo & 1);
    }
  }
  if((success && build_config.success.show_cwd) || (!success && build_config.error.show_cwd))
  {
    xsayf(" cwd %.*s\n", (int)slot->pwd_len, slot->pwd);
  }
  if((success && build_config.success.show_sources) || (!success && build_config.error.show_sources))
  {
    xsayf(" sources %2d\n", 1);
    xsayf("  [%2d] \n", slot->sources_len);
    yo = 0;
    for(x = 0; x < slot->sources_len; x++)
    {
      xsayf(" %.*s\n", (int)slot->sources->len[yo/2], &slot->sources->s[yo+2]);
      yo += slot->sources->len[yo/2] + 2;
      yo += (yo & 1);
    }
  }
  if((success && build_config.success.show_targets) || (!success && build_config.error.show_targets))
  {
    xsayf(" targets %2d\n", 1);
    xsayf("  [%2d] \n", slot->targets_len);
    yo = 0;
    for(x = 0; x < slot->targets_len; x++)
    {
      xsayf(" %.*s\n", (int)slot->targets->len[yo/2], &slot->targets->s[yo+2]);
      yo += slot->targets->len[yo/2] + 2;
      yo += (yo & 1);
    }
  }
  if((success && build_config.success.show_status) || (!success && build_config.error.show_status))
  {
    xsayf(" status %d exit %d signal %d core %s\n", slot->status, exit, sig, core ? "true" : "false");
  }
  if(color.len) {
    write(1, NOCOLOR);
  }

  if((success && build_config.success.show_stdout) || (!success && build_config.error.show_stdout))
  {
    blen = bs->stdout_len;
    if(build_config.success.show_stdout_limit_bytes > 0)
    {
      blen = MIN(blen, build_config.success.show_stdout_limit_bytes);
    }

    if(build_config.success.show_stdout_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs->stdout_text[x] == '\n')
        {
          if(lines++ > build_config.success.show_stdout_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((slot->stdout_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" stdout - showing %"PRIu16" of %"PRIu32" bytes\n", blen, slot->stdout_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs->stdout_text, blen);
        xsays("\n");
      }
    }
  }
  if((success && build_config.success.show_stderr) || (!success && build_config.error.show_stderr))
  {
    blen = bs->stderr_len;
    if(build_config.success.show_stderr_limit_bytes > 0)
    {
      blen = MIN(blen, build_config.success.show_stderr_limit_bytes);
    }

    if(build_config.success.show_stderr_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs->stderr_text[x] == '\n')
        {
          if(lines++ > build_config.success.show_stderr_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((slot->stderr_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" stderr - showing %"PRIu16" of %"PRIu32" bytes\n", blen, slot->stderr_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs->stderr_text, blen);
        xsays("\n");
      }
    }
  }
  if((success && build_config.success.show_auxout) || (!success && build_config.error.show_auxout))
  {
    blen = bs->auxout_len;
    if(build_config.success.show_auxout_limit_bytes > 0)
    {
      blen = MIN(blen, build_config.success.show_auxout_limit_bytes);
    }

    if(build_config.success.show_auxout_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs->auxout_text[x] == '\n')
        {
          if(lines++ > build_config.success.show_auxout_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((slot->auxout_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" auxout - showing %"PRIu16" of %"PRIu32" bytes\n", blen, slot->auxout_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs->auxout_text, blen);
        xsays("\n");
      }
    }
  }

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_RESPONSE)
  {
    g_params.shutdown = true;
  }
  else if(msg->type == FABIPC_MSG_EVENTS)
  {
    fatal(build_command_process_event, client, msg);
  }

  finally : coda;
}

//
// build
//

static void reconfigure_show_settings(const struct config_formula_show_settings * restrict settings, struct show_settings * restrict config)
{
  box_bool_setif(settings->show_path, &config->show_path);
  box_bool_setif(settings->show_command, &config->show_command);
  box_bool_setif(settings->show_cwd, &config->show_cwd);
  box_bool_setif(settings->show_arguments, &config->show_arguments);
  box_bool_setif(settings->show_sources, &config->show_sources);
  box_bool_setif(settings->show_targets, &config->show_targets);
  box_bool_setif(settings->show_environment, &config->show_environment);
  box_bool_setif(settings->show_status, &config->show_status);
  box_bool_setif(settings->show_stdout, &config->show_stdout);
  box_int16_setif(settings->show_stdout_limit_bytes, &config->show_stdout_limit_bytes);
  box_int16_setif(settings->show_stdout_limit_lines, &config->show_stdout_limit_lines);
  box_bool_setif(settings->show_stderr, &config->show_stderr);
  box_int16_setif(settings->show_stderr_limit_bytes, &config->show_stderr_limit_bytes);
  box_int16_setif(settings->show_stderr_limit_lines, &config->show_stderr_limit_lines);
  box_bool_setif(settings->show_auxout, &config->show_auxout);
  box_int16_setif(settings->show_auxout_limit_bytes, &config->show_auxout_limit_bytes);
  box_int16_setif(settings->show_auxout_limit_lines, &config->show_auxout_limit_lines);
}

static void reconfigure(const config * restrict cfg, struct build_config * restrict config)
{
  reconfigure_show_settings(&cfg->formula.error, &config->error);
  reconfigure_show_settings(&cfg->formula.success, &config->success);
}

//
// internal
//

void build_command_usage(command * restrict cmd)
{
  printf(
"build options\n"
" -t                  transitive build targets\n"
" +t        (default) subsequent non-options are -t <arg>\n"
" -x                  direct build targets\n"
" +x                  subsequent non-options are -t <arg>\n"
  );
}

xapi build_command_args_parse(command * restrict cmd, int argc, char ** restrict argv)
{
  enter;

  int x;
  pstring s = { 0 };
  int mode = 't';    // transitive by default
  int nextmode = 0;
  int longindex;

  const struct option longopts[] = {
      { }
  };

  const char *switches =
    // no-argument switches
    "tx"

    // with-argument switches
    ""
  ;

  fatal(list_createx, &args.targets, 0, 0, wfree, 0);

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(argc, argv, switches, longopts, &longindex)) != -1)
  {
    if(x == 0)
    {
      // longopts
    }
    else if(x == '?')
    {
      if(optopt)
      {
        failf(MAIN_BADARGS, "unknown switch", "-%c", optopt);
      }
      else
      {
        fails(MAIN_BADARGS, "unknown argument", argv[optind-1]);
      }
    }
    else if(x == 't')
    {
      nextmode = x;
    }
    else if(x == 'x')
    {
      nextmode = x;
    }
    else
    {
      if(optarg[0] == '-' && optarg[1] == 't')
        nextmode = 't';
      else if(optarg[0] == '+' && optarg[1] == 't')
        mode = 't';
      else if(optarg[0] == '-' && optarg[1] == 'x')
        nextmode = 'x';
      else if(optarg[0] == '+' && optarg[1] == 'x')
        mode = 'x';
      else
      {
        fatal(psinit, &s);
        if(nextmode)
          fatal(pscatc, &s, nextmode);
        else
          fatal(pscatc, &s, mode);
        fatal(pscats, &s, optarg);
        fatal(list_push, args.targets, s.s, 0);
        memset(&s, 0, sizeof(s));
        nextmode = 0;
      }
    }
  }

  // options following --
  for(; optind < argc; optind++)
  {
    fatal(psinit, &s);
    fatal(pscatc, &s, mode);
    fatal(pscats, &s, argv[optind]);
    fatal(list_push, args.targets, s.s, 0);
    memset(&s, 0, sizeof(s));
  }

finally:
  fatal(psdestroy, &s);
coda;
}

xapi build_command_request_collate(narrator * restrict N)
{
  enter;

  int x;
  char *arg;

  if(g_args.invalidate) {
    fatal(narrator_xsays, N, " global-invalidate");
  }

  /* transitive targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 't')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, N, ""
" select : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
" invalidate "
    );
  }

  /* direct targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 'x')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, N, ""
" select : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 'x')
        continue;

      fatal(narrator_xsayf, N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
"invalidate "
    );
  }

  fatal(narrator_xsays, N, ""
" goals : {"
  );

  /* transitive targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 't')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, N, ""
" target-transitive : ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
    );
  }

  /* direct targets */
  for(x = 0; x < args.targets->size; x++)
  {
    arg = list_get(args.targets, x);
    if(arg[0] == 'x')
      break;
  }

  if(x < args.targets->size)
  {
    fatal(narrator_xsays, N, ""
" target-direct: ["
    );

    for(x = 0; x < args.targets->size; x++)
    {
      arg = list_get(args.targets, x);
      if(arg[0] != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" pattern : %s", &arg[1]
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
    );
  }

  fatal(narrator_xsayf, N, ""
" build }"
  );

  finally : coda;
}

xapi build_command_process_event(fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  build_slot *bs, **bsp;

  if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_FORKED)
  {
    fatal(slot_alloc, &bs, msg->id);
    fatal(hashtable_put, build_slots_bypid, &bs);

    descriptor_type_unmarshal(bs, &descriptor_fab_build_slot, msg->text, msg->size);
  }
  else if(msg->evtype == FABIPC_EVENT_STDOUT)
  {
    write(1, msg->text, msg->size);
  }
  else if(msg->evtype == FABIPC_EVENT_STDERR)
  {
    write(2, msg->text, msg->size);
  }
  else
  {
    bsp = hashtable_search(build_slots_bypid, (void*)&msg->id, sizeof(msg->id), build_slot_key_hash, build_slot_key_cmp);
    bs = *bsp;

    if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_STDOUT)
    {
      fatal(assure, &bs->stdout_text, 1, bs->stdout_len + msg->size, &bs->stdout_alloc);
      memcpy(bs->stdout_text + bs->stdout_len, msg->text, msg->size);
      bs->stdout_len += msg->size;
    }
    else if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_STDERR)
    {
      fatal(assure, &bs->stderr_text, 1, bs->stderr_len + msg->size, &bs->stderr_alloc);
      memcpy(bs->stderr_text + bs->stderr_len, msg->text, msg->size);
      bs->stderr_len += msg->size;
    }
    else if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_AUXOUT)
    {
      fatal(assure, &bs->auxout_text, 1, bs->auxout_len + msg->size, &bs->auxout_alloc);
      memcpy(bs->auxout_text + bs->auxout_len, msg->text, msg->size);
      bs->auxout_len += msg->size;
    }
    else if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_WAITED)
    {
      descriptor_type_unmarshal(bs, &descriptor_fab_build_slot, msg->text, msg->size);

      fatal(slot_print, bs);

//printf("[%8d] DELETE %p\n", bs->base.pid, bs);
      hashtable_delete(build_slots_bypid, &bs);
    }
  }

  finally : coda;
}

//
// public
//

xapi build_command_setup()
{
  enter;

  fatal(hashtable_createx
    , &build_slots_bypid
    , sizeof(build_slot*)
    , 256
    , build_slot_hash
    , build_slot_cmp
    , build_slot_release
    , 0
  );

  finally : coda;
}

xapi build_command_reconfigure(config * restrict cfg)
{
  enter;

  reconfigure(cfg, &build_config);

  finally : coda;
}

xapi build_command_cleanup()
{
  enter;

  build_slot *bs;
  llist *lln;

  fatal(list_xfree, args.targets);
  fatal(hashtable_xfree, build_slots_bypid);

  llist_foreach_safe(&slots_freelist, bs, lln, lln) {
//printf("[%8d] FREE %p\n", bs->base.pid, bs);
    slot_free(bs);
  }

  finally : coda;
}

command build_command = {
    name : "build"
  , args_parse : build_command_args_parse
  , usage : build_command_usage
  , connected : connected
  , process : process
};
