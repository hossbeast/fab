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
#include <unistd.h>
#include <wait.h>

#include "xlinux/KERNEL.errtab.h"
#include "common/assure.h"
#include "common/color.h"
#include "common/hash.h"
#include "fab/build_slot.h"
#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "xlinux/xstdlib.h"

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
  /* slots can be chained together for execs with very large lists of strings */
  struct build_slot *next;

  /* buffer which info members point into */
  char info_buf[16384];

  fab_build_slot_info info;
  fab_build_slot_results results;
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

static llist slots_freelist = LLIST_INITIALIZER(slots_freelist);
static hashtable * build_slots_bypid;

//
// static
//

static uint32_t build_slot_hash(uint32_t h, const void * e, size_t el)
{
  const build_slot * const * bsp = e;

  return hash32(h, &(*bsp)->info.pid, sizeof((*bsp)->info.pid));
}

static int build_slot_cmp(const void * A, size_t Al, const void * B, size_t Bl)
{
  const build_slot * const * Absp = A;
  const build_slot * const * Bbsp = B;

  return INTCMP((*Absp)->info.pid, (*Bbsp)->info.pid);
}

static uint32_t build_slot_key_hash(uint32_t h, const void *key, size_t el)
{
  return hash32(h, key, el);
}

static int build_slot_key_cmp(const void * A, const void *key, size_t keysz)
{
  const build_slot * const * Absp = A;
  const uint32_t * pidp = key;

  return INTCMP((*Absp)->info.pid, *pidp);
}

static void build_slot_release(void *e)
{
  build_slot **bsp = e;
  build_slot *bs = *bsp;
  build_slot *next;

  while(bs)
  {
    next = bs->next;
    llist_append(&slots_freelist, bs, lln);
    bs = next;
  }
}

static xapi connected(command * restrict cmd, fab_client * restrict client)
{
  enter;

  narrator * request_narrator;
  narrator_fixed nstor;
  fabipc_message * msg;

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
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | FABIPC_EVENT_FORMULA_EXEC_FORKED
    | FABIPC_EVENT_FORMULA_EXEC_STDOUT
    | FABIPC_EVENT_FORMULA_EXEC_STDERR
    | FABIPC_EVENT_FORMULA_EXEC_AUXOUT
    | FABIPC_EVENT_FORMULA_EXEC_WAITED
    ;
  client_post(client, msg);

  /* send the request */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_REQUEST;
  requestid = msg->id = PID_AS_MSGID(getpid());

  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);

  /* re-configure goals */
  fatal(build_command_collate_goals, request_narrator, false);

  fatal(narrator_xsayf, request_narrator, " reconcile ");

  /* invalidate targets */
  fatal(build_command_collate_invalidations, request_narrator);

  /* build the targets */
  fatal(narrator_xsayf, request_narrator, " run");

  // two terminating null bytes
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  msg->size = nstor.l;

  client_post(client, msg);

  finally : coda;
}

static xapi slot_alloc(build_slot ** slotp, uint32_t pid)
{
  enter;

  build_slot *bs;

  if((bs = llist_shift(&slots_freelist, typeof(*bs), lln)) == 0)
  {
    fatal(xmalloc, &bs, sizeof(*bs));
  }
  else
  {
    memset(bs, 0, offsetof(typeof(*bs), persisted));
  }

  bs->info.pid = pid;

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

static xapi slot_print(build_slot *bs_zero)
{
  enter;

  narrator *N = g_narrator_stdout;
  fab_build_slot_results *results;
  fab_build_string str;
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

    descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.arg_list, bs->info.arg_list_size);
    xsayf("%2d %.*s -> ", bs->info.stage, (int)str.text_len, str.text);
    break;
  }

  y = 0;
  for(bs = bs_zero; bs; bs = bs->next)
  {
    z = 0;
    for(x = 0; x < bs->info.target_list_len; x++)
    {
      z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.target_list + z, bs->info.target_list_size - z);
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
        z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.arg_list + z, bs->info.arg_list_size - z);
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
        z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.env_list + z, bs->info.env_list_size - z);
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
        z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.arg_list + z, bs->info.arg_list_size - z);
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
        z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.source_list + z, bs->info.source_list_size - z);
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
        z += descriptor_type_unmarshal(&str, &descriptor_fab_build_string, bs->info.target_list + z, bs->info.target_list_size - z);
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

  finally : coda;
}

static xapi process(command * restrict cmd, fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  if(msg->type == FABIPC_MSG_RESPONSE)
  {
    if(msg->id == requestid) {
      g_params.shutdown = true;
    } else {
      RUNTIME_ASSERT(msg->id == eventsubid);
    }
  }
  else if(msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE) { }
  else if(msg->type == FABIPC_MSG_EVENTS)
  {
    fatal(build_command_process_event, client, msg);
  }

  finally : coda;
}

xapi build_command_collate_invalidations(narrator * restrict N)
{
  enter;

  struct build_target *arg;
  int x;

  if(g_args.invalidate) {
    fatal(narrator_xsays, N, " global-invalidate");
    goto XAPI_FINALLY;
  }

  /* transitive targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 't')
      break;
  }

  if(x < args->targets_len)
  {
    fatal(narrator_xsays, N, ""
" select : ["
    );

    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" path : \"%.*s\"", (int)arg->sl, arg->s
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
" invalidate "
    );
  }

  /* direct targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 'x')
      break;
  }

  if(x < args->targets_len)
  {
    fatal(narrator_xsays, N, ""
" select : ["
    );

    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode != 'x')
        continue;

      fatal(narrator_xsayf, N, ""
" path : \"%.*s\"", (int)arg->sl, arg->s
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
"invalidate "
    );
  }

  finally : coda;
}

xapi build_command_collate_goals(narrator * restrict N, bool reconcile)
{
  enter;

  int x;
  struct build_target *arg;

  fatal(narrator_xsays, N, ""
" goals : { "
  );

  if(reconcile)
  {
    fatal(narrator_xsays, N, "reconcile ");
  }

  /* transitive targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 't')
      break;
  }

  if(x < args->targets_len)
  {
    fatal(narrator_xsays, N, ""
" target-transitive : ["
    );

    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" path : \"%.*s\"", arg->sl, arg->s
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
    );
  }

  /* direct targets */
  for(x = 0; x < args->targets_len; x++)
  {
    arg = &args->targets[x];
    if(arg->mode == 'x')
      break;
  }

  if(x < args->targets_len)
  {
    fatal(narrator_xsays, N, ""
" target-direct: ["
    );

    for(x = 0; x < args->targets_len; x++)
    {
      arg = &args->targets[x];
      if(arg->mode != 't')
        continue;

      fatal(narrator_xsayf, N, ""
" path : \"%.*s\"", (int)arg->sl, arg->s
      );
    }

    fatal(narrator_xsays, N, ""
" ] "
    );
  }

  fatal(narrator_xsays, N, " build }");

  finally : coda;
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

xapi build_command_process_event(fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  build_slot *bs, **bsp;
  size_t __attribute__((unused)) z;
  uint32_t pid;

  if(msg->evtype == FABIPC_EVENT_FORMULA_EXEC_FORKED)
  {
    pid = msg->id;
    bsp = hashtable_search(build_slots_bypid, (void*)&pid, sizeof(pid), build_slot_key_hash, build_slot_key_cmp);
    if(bsp) {
      bs = *bsp;

      while(bs->next) {
        bs = bs->next;
      }

      fatal(slot_alloc, &bs->next, msg->id);
      bs = bs->next;
    } else {
      fatal(slot_alloc, &bs, msg->id);
      fatal(hashtable_put, build_slots_bypid, &bs);
    }

    RUNTIME_ASSERT(msg->size <= sizeof(bs->info_buf));
    memcpy(bs->info_buf, msg->text, msg->size);

    z = descriptor_type_unmarshal(&bs->info, &descriptor_fab_build_slot_info, bs->info_buf, msg->size);
    RUNTIME_ASSERT(z == msg->size);
  }
  else
  {
    pid = msg->id;
    bsp = hashtable_search(build_slots_bypid, (void*)&pid, sizeof(pid), build_slot_key_hash, build_slot_key_cmp);
    if(!bsp) {
      fprintf(stderr, "unknown pid %"PRIu32" 0x%016x head %5u\n", pid, pid, containerof(msg, fabipc_page, msg)->head);
      goto XAPI_FINALLY;
    }
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
      z = descriptor_type_unmarshal(&bs->results, &descriptor_fab_build_slot_results, msg->text, msg->size);
      RUNTIME_ASSERT(z == msg->size);

      build_was_failed_slot |= bs->results.status != 0;
      build_was_failed_slot |= bs->results.stderr_total > 0;

      fatal(slot_print, bs);
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

xapi build_command_cleanup()
{
  enter;

  build_slot *bs;
  llist *lln;

  fatal(hashtable_xfree, build_slots_bypid);

  llist_foreach_safe(&slots_freelist, bs, lln, lln) {
    slot_free(bs);
  }

  finally : coda;
}

command build_command = {
    name : "build"
  , usage : build_command_usage
  , connected : connected
  , process : process
};
