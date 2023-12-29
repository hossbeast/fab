/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>

#include <inttypes.h>
#include <unistd.h>
#include <wait.h>

#include "common/assure.h"
#include "common/color.h"
#include "common/hash.h"
#include "bam/build_slot.h"
#include "bam/client.h"
#include "bam/events.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "build.h"
#include "args.h"
#include "command.h"
#include "params.h"

static uint64_t requestid;
static uint64_t eventsubid;
bool build_was_failed_slot;

struct build_args build_args = {
  mode : 't'
};
static struct build_args *args = &build_args;

typedef struct build_slot {
  bam_build_slot_info info;

  /* slots can be chained together for execs with very large lists of strings */
  struct build_slot *next;

  /* buffer which info members point into */
  char info_buf[16384];

  bam_build_slot_results results;
  uint32_t stdout_len;
  uint32_t stderr_len;
  uint32_t auxout_len;

  union {
    llist lln;
    char persisted;
  };

  char *stdout_text;
  size_t stdout_alloc;

  char *stderr_text;
  size_t stderr_alloc;

  char *auxout_text;
  size_t auxout_alloc;
} build_slot;

/* first so that build_slot_cmp works with just a pid */
STATIC_ASSERT(offsetof(build_slot, info) == 0);

static llist slots_freelist = LLIST_INITIALIZER(slots_freelist);
static hashtable * build_slots_bypid;

//
// static
//

static uint32_t build_slot_hash(uint32_t h, const void * e)
{
  const build_slot * bs = e;

  return hash32(h, &bs->info.pid, sizeof(bs->info.pid));
}

static int build_slot_cmp(const void * A, const void * B)
{
  const build_slot * Abs = A;
  const build_slot * Bbs = B;

  return INTCMP(Abs->info.pid, Bbs->info.pid);
}

static void build_slot_release(void *e)
{
  build_slot *bs = e;
  build_slot *next;

  while(bs)
  {
    next = bs->next;
    llist_append(&slots_freelist, &bs->lln);
    bs = next;
  }
}

static void connected(command * restrict cmd, bam_client * restrict client)
{
  narrator * request_narrator;
  narrator_fixed nstor;
  bamipc_message * msg;
  bacon_writer writer, *builder_writer;

  if(args->targets_len)
  {
    args->success.show_path = true;
    args->success.show_arguments = true;
    args->success.show_command = true;
    args->success.show_cwd = true;
    args->success.show_sources = true;
    args->success.show_targets = true;
    args->success.show_environment = true;
    args->success.show_status = true;
    args->success.show_stdout = true;
    args->success.show_stderr = true;
    args->success.show_auxout = true;
  }

  /* subscribe to relevant events */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_FORMULA_EXEC_FORKED
    | BAMIPC_EVENT_FORMULA_EXEC_STDOUT
    | BAMIPC_EVENT_FORMULA_EXEC_STDERR
    | BAMIPC_EVENT_FORMULA_EXEC_AUXOUT
    | BAMIPC_EVENT_FORMULA_EXEC_WAITED
    ;
  client_post(client, msg);

  /* send the request */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_REQUEST;
  requestid = msg->id = PID_AS_MSGID(getpid());

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, request_narrator);

  /* re-configure goals */
  build_command_collate_goals(builder_writer, false);

  STRING("reconcile")

  /* invalidate targets */
  build_command_collate_invalidations(builder_writer);

  /* build the targets */
  STRING("run")

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  narrator_sayc(request_narrator, 0);
  narrator_sayc(request_narrator, 0);

  msg->size = nstor.l;
  client_post(client, msg);
}

static void slot_alloc(build_slot ** slotp, uint32_t pid)
{
  build_slot *bs;

  if((bs = llist_shift(&slots_freelist, typeof(*bs), lln)) == 0)
  {
    xmalloc(&bs, sizeof(*bs));
  }
  else
  {
    memset(bs, 0, offsetof(typeof(*bs), persisted));
  }

  bs->info.pid = pid;

  *slotp = bs;
}

static void slot_free(build_slot *bs)
{
  if(bs)
  {
    free(bs->stdout_text);
    free(bs->stderr_text);
    free(bs->auxout_text);
  }

  free(bs);
}

static void slot_print(build_slot *bs_zero)
{
  narrator *N = g_narrator_stdout;
  bam_build_slot_results *results;
  bam_build_string str;
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
  int y;
  int lines;
  size_t z;
  build_slot *bs;

  results = &bs_zero->results;
  exit = 0;
  sig = 0;
  core = false;
  if(WIFEXITED(results->status))
  {
    exit = WEXITSTATUS(results->status);
  }
  else if(WIFSIGNALED(results->status))
  {
    sig = WTERMSIG(results->status);
    core = WCOREDUMP(results->status);
  }

  success = true;
  if(results->status != 0 || results->stderr_total > 0) {
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

  /* first arg is the program name */
  for(bs = bs_zero; bs; bs = bs->next)
  {
    if(bs->info.arg_list_len == 0) {
      continue;
    }

    descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.arg_list, bs->info.arg_list_size);
    xsayf("%2d %.*s -> ", bs->info.stage, (int)str.text_len, str.text);
    break;
  }

  y = 0;
  for(bs = bs_zero; bs; bs = bs->next)
  {
    z = 0;
    for(x = 0; x < bs->info.target_list_len; x++)
    {
      z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.target_list + z, bs->info.target_list_size - z);
      if(y) {
        xsays(", ");
      }
      xsayw(str.text, str.text_len);
      y++;
    }
    RUNTIME_ASSERT((z + 6) == bs->info.target_list_size);
  }
  xsays("\n");

  if((success && args->success.show_path) || (!success && args->error.show_path))
  {
    xsayf(" rule %.*s\n", (int)bs_zero->info.rule_id_len, bs_zero->info.rule_id);
    xsayf(" formula %.*s\n", (int)bs_zero->info.formula_abspath_len, bs_zero->info.formula_abspath);
    xsayf(" path %.*s\n", (int)bs_zero->info.path_len, bs_zero->info.path);
  }
  if((success && args->success.show_command) || (!success && args->error.show_command))
  {
    y = 0;
    for(bs = bs_zero; bs; bs = bs->next)
    {
      z = 0;
      for(x = 0; x < bs->info.arg_list_len; x++)
      {
        z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.arg_list + z, bs->info.arg_list_size - z);
        if(y) {
          xsays(" ");
        }
        xsayw(str.text, str.text_len);
        y++;
      }
      RUNTIME_ASSERT((z + 6) == bs->info.arg_list_size);
    }
    xsays("\n");
  }
  if((success && args->success.show_environment) || (!success && args->error.show_environment))
  {
    xsayf(" envp %2d\n", bs_zero->info.env_list_count);
    y = 0;
    for(bs = bs_zero; bs; bs = bs->next)
    {
      z = 0;
      for(x = 0; x < bs->info.env_list_len; x++)
      {
        z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.env_list + z, bs->info.env_list_size - z);
        xsayf("  [%2d] %.*s\n", y, (int)str.text_len, str.text);
        y++;
      }
      RUNTIME_ASSERT((z + 6) == bs->info.env_list_size);
    }
  }
  if((success && args->success.show_arguments) || (!success && args->error.show_arguments))
  {
    xsayf(" argv %2d\n", bs_zero->info.arg_list_count);
    y = 0;
    for(bs = bs_zero; bs; bs = bs->next)
    {
      z = 0;
      for(x = 0; x < bs->info.arg_list_len; x++)
      {
        z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.arg_list + z, bs->info.arg_list_size - z);
        xsayf("  [%2d] %.*s\n", y, (int)str.text_len, str.text);
        y++;
      }
      RUNTIME_ASSERT((z + 6) == bs->info.arg_list_size);
    }
  }
  if((success && args->success.show_cwd) || (!success && args->error.show_cwd))
  {
    xsayf(" cwd %.*s\n", (int)bs_zero->info.pwd_len, bs_zero->info.pwd);
  }
  if((success && args->success.show_sources) || (!success && args->error.show_sources))
  {
    xsayf(" sources %2d\n", bs_zero->info.source_list_count);
    y = 0;
    for(bs = bs_zero; bs; bs = bs->next)
    {
      z = 0;
      for(x = 0; x < bs->info.source_list_len; x++)
      {
        z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.source_list + z, bs->info.source_list_size - z);
        xsayf("  [%2d] %.*s\n", y, (int)str.text_len, str.text);
        y++;
      }
      RUNTIME_ASSERT((z + 6) == bs->info.source_list_size);
    }
  }
  if((success && args->success.show_targets) || (!success && args->error.show_targets))
  {
    xsayf(" targets %2d\n", bs_zero->info.target_list_count);
    y = 0;
    for(bs = bs_zero; bs; bs = bs->next)
    {
      z = 0;
      for(x = 0; x < bs->info.target_list_len; x++)
      {
        z += descriptor_type_unmarshal(&str, &descriptor_bam_build_string, bs->info.target_list + z, bs->info.target_list_size - z);
        xsayf("  [%2d] %.*s\n", y, (int)str.text_len, str.text);
        y++;
      }
      RUNTIME_ASSERT((z + 6) == bs->info.target_list_size);
    }
  }
  if((success && args->success.show_status) || (!success && args->error.show_status))
  {
    xsayf(" status %d exit %d signal %d core %s\n", results->status, exit, sig, core ? "true" : "false");
  }
  if(color.len) {
    write(1, NOCOLOR);
  }

  if((success && args->success.show_stdout) || (!success && args->error.show_stdout))
  {
    blen = bs_zero->stdout_len;
    if(args->success.show_stdout_limit_bytes > 0)
    {
      blen = MIN(blen, args->success.show_stdout_limit_bytes);
    }

    if(args->success.show_stdout_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs_zero->stdout_text[x] == '\n')
        {
          if(lines++ > args->success.show_stdout_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((results->stdout_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" stdout - showing %"PRIu16" of %"PRIu32" bytes\n", blen, results->stdout_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs_zero->stdout_text, blen);
        xsays("\n");
      }
    }
  }
  if((success && args->success.show_stderr) || (!success && args->error.show_stderr))
  {
    blen = bs_zero->stderr_len;
    if(args->success.show_stderr_limit_bytes > 0)
    {
      blen = MIN(blen, args->success.show_stderr_limit_bytes);
    }

    if(args->success.show_stderr_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs_zero->stderr_text[x] == '\n')
        {
          if(lines++ > args->success.show_stderr_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((results->stderr_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" stderr - showing %"PRIu16" of %"PRIu32" bytes\n", blen, results->stderr_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs_zero->stderr_text, blen);
        xsays("\n");
      }
    }
  }
  if((success && args->success.show_auxout) || (!success && args->error.show_auxout))
  {
    blen = bs_zero->auxout_len;
    if(args->success.show_auxout_limit_bytes > 0)
    {
      blen = MIN(blen, args->success.show_auxout_limit_bytes);
    }

    if(args->success.show_auxout_limit_lines > 0)
    {
      lines = 0;
      for(x = 0; x < blen; x++)
      {
        if(bs_zero->auxout_text[x] == '\n')
        {
          if(lines++ > args->success.show_auxout_limit_lines)
            break;
        }
      }

      blen = MIN(blen, x);
    }

    if(blen || !success)
    {
      if((results->auxout_total > blen) || !success)
      {
        if(color.len) {
          write(1, color.b, color.len);
        }
        xsayf(" auxout - showing %"PRIu16" of %"PRIu32" bytes\n", blen, results->auxout_total);
        if(color.len) {
          write(1, NOCOLOR);
        }
      }
      if(blen)
      {
        xsays("\n");
        xsayw(bs_zero->auxout_text, blen);
        xsays("\n");
      }
    }
  }
}

static void process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
  if(msg->type == BAMIPC_MSG_RESPONSE)
  {
    if(msg->id == requestid) {
      g_params.shutdown = true;
    } else {
      RUNTIME_ASSERT(msg->id == eventsubid);
    }
  }
  else if(msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE)
  {
    /* printed in main */
  }
  else if(msg->type == BAMIPC_MSG_EVENTS)
  {
    build_command_process_event(client, msg);
  }
}

static void attached(command * restrict cmd, bam_client * restrict client)
{

}

void build_command_collate_invalidations(bacon_writer * restrict builder_writer)
{
  struct build_target *arg;
  int x;

  if(g_args.invalidate) {
    STRING("global-invalidate");
    return;
  }

  /* transitive targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 't') {
      break;
    }
  }

  if(x < args->targets_len)
  {
    { MAPPING
      STRING("select")
      { LIST
        for(x = 0; x < args->targets_len; x++)
        {
          arg = &args->targets[x];
          if(arg->mode != 't') {
            continue;
          }
          { MAPPING
            STRING("lookup")
            { SET
              { MAPPING
                STRING("path")
                BYTES(arg->s, (int)arg->sl)
              }
            }
          }
        }
      }
    }
    STRING("invalidate")
  }

  /* direct targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 'x') {
      break;
    }
  }

  if(x < args->targets_len)
  {
    { MAPPING
      STRING("select")
      { LIST
        for(x = 0; x < args->targets_len; x++)
        {
          arg = &args->targets[x];
          if(arg->mode != 'x') {
            continue;
          }
          { MAPPING
            STRING("lookup")
            { SET
              { MAPPING
                STRING("path")
                BYTES(arg->s, (int)arg->sl)
              }
            }
          }
        }
      }
    }
    STRING("invalidate")
  }
}

void build_command_collate_goals(bacon_writer * restrict builder_writer, bool reconcile)
{
  int x;
  struct build_target *arg;

  MAPPING
  STRING("goals")
  { SET
    if(reconcile) {
      STRING("reconcile");
    }

    /* transitive targets */
    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode == 't') {
        break;
      }
    }

    if(x < args->targets_len)
    {
      MAPPING
      STRING("target-transitive")
      { LIST
        for(x = 0; x < args->targets_len; x++)
        {
          arg = &args->targets[x];
          if(arg->mode != 't') {
            continue;
          }
          { MAPPING
            STRING("lookup")
            { SET
              { MAPPING
                STRING("path")
                BYTES(arg->s, (int)arg->sl)
              }
            }
          }
        }
      }
    }

    /* direct targets */
    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode == 'x') {
        break;
      }
    }

    if(x < args->targets_len)
    {
      MAPPING
      STRING("target-direct")
      { LIST
        for(x = 0; x < args->targets_len; x++)
        {
          arg = &args->targets[x];
          if(arg->mode != 't') {
            continue;
          }
          { MAPPING
            STRING("lookup")
            { SET
              { MAPPING
                STRING("path")
                BYTES(arg->s, (int)arg->sl)
              }
            }
          }
        }
      }
    }

    STRING("build")
  }
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

void build_command_process_event(bam_client * restrict client, bamipc_message * restrict msg)
{
  build_slot *bs;
  size_t __attribute__((unused)) z;
  uint32_t pid;
  hashtable_search_context ctx = {};
  bam_build_slot_info key;
  bool r;

  ctx.entry = &key;
  pid = key.pid = msg->id;
  r = hashtable_search(build_slots_bypid, &ctx);
  if(msg->evtype == BAMIPC_EVENT_FORMULA_EXEC_FORKED)
  {
    if(r)
    {
      bs = hashtable_fetch(build_slots_bypid, &ctx);
      while(bs->next) {
        bs = bs->next;
      }

      slot_alloc(&bs->next, msg->id);
      bs = bs->next;
    } else {
      slot_alloc(&bs, msg->id);
      ctx.entry = bs;
      hashtable_set(build_slots_bypid, &ctx);
    }

    RUNTIME_ASSERT(msg->size <= sizeof(bs->info_buf));
    memcpy(bs->info_buf, msg->text, msg->size);

    z = descriptor_type_unmarshal(&bs->info, &descriptor_bam_build_slot_info, bs->info_buf, msg->size);
    RUNTIME_ASSERT(z == msg->size);
  }
  else
  {
    if(!r) {
      fprintf(stderr, "unknown pid %"PRIu32" 0x%016x head %5u\n", pid, pid, containerof(msg, bamipc_page, msg)->head);
      return;
    }
    bs = hashtable_fetch(build_slots_bypid, &ctx);

    if(msg->evtype == BAMIPC_EVENT_FORMULA_EXEC_STDOUT)
    {
      assure(&bs->stdout_text, 1, bs->stdout_len + msg->size, &bs->stdout_alloc);
      memcpy(bs->stdout_text + bs->stdout_len, msg->text, msg->size);
      bs->stdout_len += msg->size;
    }
    else if(msg->evtype == BAMIPC_EVENT_FORMULA_EXEC_STDERR)
    {
      assure(&bs->stderr_text, 1, bs->stderr_len + msg->size, &bs->stderr_alloc);
      memcpy(bs->stderr_text + bs->stderr_len, msg->text, msg->size);
      bs->stderr_len += msg->size;
    }
    else if(msg->evtype == BAMIPC_EVENT_FORMULA_EXEC_AUXOUT)
    {
      assure(&bs->auxout_text, 1, bs->auxout_len + msg->size, &bs->auxout_alloc);
      memcpy(bs->auxout_text + bs->auxout_len, msg->text, msg->size);
      bs->auxout_len += msg->size;
    }
    else if(msg->evtype == BAMIPC_EVENT_FORMULA_EXEC_WAITED)
    {
      z = descriptor_type_unmarshal(&bs->results, &descriptor_bam_build_slot_results, msg->text, msg->size);
      RUNTIME_ASSERT(z == msg->size);

      build_was_failed_slot |= bs->results.status != 0;
      build_was_failed_slot |= bs->results.stderr_total > 0;

      slot_print(bs);
      hashtable_remove(build_slots_bypid, &ctx);
    }
  }
}

static void disconnected(command * restrict cmd) { }

//
// public
//

void build_command_setup()
{
  hashtable_createx(
      &build_slots_bypid
    , 256
    , build_slot_hash
    , build_slot_cmp
    , build_slot_release
  );
}

void build_command_cleanup()
{
  build_slot *bs;
  llist *lln;

  hashtable_free(build_slots_bypid);

  llist_foreach_safe(&slots_freelist, bs, lln, lln) {
    slot_free(bs);
  }
}

command build_command = {
    name : "build"
  , attached : attached
  , usage : build_command_usage
  , connected : connected
  , process : process
  , disconnected : disconnected
};
