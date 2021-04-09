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
#include <string.h>

#include "xapi.h"
#include "common/attrs.h"
#include "fab/client.h"
#include "fab/describe.h"
#include "fab/fsent.h"
#include "fab/ipc.h"
#include "fab/list.h"
#include "fab/stats.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/xsignal.h"

#include "explorer.h"
#include "area.h"
#include "args.h"
#include "client_thread.h"
#include "display.h"
#include "params.h"

#include "zbuffer.h"

static const char *graph = "depends";

/* items shown in winup */
static fab_list_item up_items[128];
static uint16_t up_items_len;
static uint16_t up_items_count;

/* items shown in windown */
static fab_list_item down_items[128];
static uint16_t down_items_len;
static uint16_t down_items_count;

/* stats shown in winnode */
static fab_var_stats stats_var;
static fab_formula_stats stats_formula;
static fab_module_file_stats stats_module_file;
static fab_module_stats stats_module;
static fab_fsent_stats stats_fsent;

/* the item shown in winnode */
static fab_describe_item node;
static fabipc_message *describe_msgs[2];
static uint8_t describe_msg_index;

/* path for the active node */
static const char *lookup_path;
static uint16_t lookup_pattern_len;

/* areas of the ui */
static area winpath;
static area wingraph;
static area winnode;
static area winproperties;
static area winmode;
static area winup;
static area windown;

/* which of winup / winnode / windown is selected */
static uint8_t selected_win = 1;
static uint8_t selected_num[3];

/* identifiers for each of the requests */
static uint32_t up_request_id;
static uint32_t down_request_id;
static uint32_t stats_request_id;
static uint32_t describe_request_id;

/* autoinc request id counter */
static uint32_t msgid;

static xapi up_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "path");
          if(lookup_pattern_len > 0 && lookup_path[0] != '.' && lookup_path[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_path, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_bytes, writer, lookup_path, lookup_pattern_len);
          }
        fatal(value_writer_pop_mapping, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "traverse");
          fatal(value_writer_push_set, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "direction");
              fatal(value_writer_string, writer, "up");
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "graph");
              fatal(value_writer_string, writer, graph);
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "min-distance");
              fatal(value_writer_string, writer, "1");
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "max-distance");
              fatal(value_writer_string, writer, "1");
            fatal(value_writer_pop_mapping, writer);
          fatal(value_writer_pop_set, writer);
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "list");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

static xapi down_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "path");
          if(lookup_pattern_len > 0 && lookup_path[0] != '.' && lookup_path[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_path, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_bytes, writer, lookup_path, lookup_pattern_len);
          }
          fatal(value_writer_pop_mapping, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "traverse");
          fatal(value_writer_push_set, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "direction");
              fatal(value_writer_string, writer, "down");
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "graph");
              fatal(value_writer_string, writer, graph);
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "min-distance");
              fatal(value_writer_string, writer, "1");
            fatal(value_writer_pop_mapping, writer);
            fatal(value_writer_push_mapping, writer);
              fatal(value_writer_string, writer, "max-distance");
              fatal(value_writer_string, writer, "1");
            fatal(value_writer_pop_mapping, writer);
          fatal(value_writer_pop_set, writer);
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "list");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

static xapi stats_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "path");
          if(lookup_pattern_len > 0 && lookup_path[0] != '.' && lookup_path[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_path, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_bytes, writer, lookup_path, lookup_pattern_len);
          }
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "stats-read");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

static xapi describe_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "path");
          if(lookup_pattern_len > 0 && lookup_path[0] != '.' && lookup_path[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_path, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_bytes, writer, lookup_path, lookup_pattern_len);
          }
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "describe");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

/*
 * post new requests - called on the client thread
 */
static xapi redrive(fab_client * restrict client)
{
  enter;

  narrator_fixed nstor;
  value_writer writer;
  narrator * request_narrator;
  uint32_t message_len;
  fabipc_message *msg;

  value_writer_init(&writer);

  // upward nodes
  up_items_len = up_items_count = 0;
  msg = fab_client_produce(client);
  up_request_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(up_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fatal(client_thread_post, client, msg);

  // downward nodes
  down_items_len = down_items_count = 0;
  msg = fab_client_produce(client);
  down_request_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(down_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fatal(client_thread_post, client, msg);

  // selected node stats
  msg = fab_client_produce(client);
  stats_request_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(stats_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fatal(client_thread_post, client, msg);

  // selected node details
  msg = fab_client_produce(client);
  describe_request_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(describe_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fatal(client_thread_post, client, msg);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

/*
 * process received messages - called on the client thread
 */
static xapi rebind(fab_client * restrict client, fabipc_message * restrict msg)
{
  enter;

  size_t z;
  void *src;
  size_t sz;

  /* the response message is delivered after all of the result messages */
  if(msg->type == FABIPC_MSG_RESPONSE)
  {
    if(msg->id == describe_request_id)
    {
      /* flip */
      describe_msg_index ^= 1;

      /* now re-draw */
      fatal(xtgkill, g_params.pid, g_params.thread_ui, SIGUSR1);

      /* release pages associated with the previous display */
      if(describe_msgs[!describe_msg_index]) {
        fab_client_release(client, describe_msgs[!describe_msg_index]);
        describe_msgs[!describe_msg_index] = 0;
      }
    }

    goto XAPI_FINALLY;
  }

  /* process the result messages */
  if(msg->id == up_request_id)
  {
    if(up_items_len < (sizeof(up_items) / sizeof(*up_items))) {
      descriptor_type_unmarshal(&up_items[up_items_len++], &descriptor_fab_list_item, msg->text, msg->size);
    }
    up_items_count++;
  }
  else if(msg->id == down_request_id)
  {
    if(down_items_len < (sizeof(down_items) / sizeof(*down_items))) {
      descriptor_type_unmarshal(&down_items[down_items_len++], &descriptor_fab_list_item, msg->text, msg->size);
    }
    down_items_count++;
  }
  else if(msg->id == stats_request_id)
  {
    src = msg->text;
    sz = msg->size;
    z = 0;
    z += descriptor_type_unmarshal(&stats_fsent, &descriptor_fab_fsent_stats, src + z, sz - z);
    if(stats_fsent.type == FAB_FSENT_TYPE_MODULE_DIR)
    {
      z += descriptor_type_unmarshal(&stats_module, &descriptor_fab_module_stats, src + z, sz - z);
    }
    else if(stats_fsent.type == FAB_FSENT_TYPE_MODULE_FILE)
    {
      z += descriptor_type_unmarshal(&stats_module_file, &descriptor_fab_module_file_stats, src + z, sz - z);
    }
    else if(stats_fsent.type == FAB_FSENT_TYPE_FORMULA_FILE)
    {
      z += descriptor_type_unmarshal(&stats_formula, &descriptor_fab_formula_stats, src + z, sz - z);
    }
    else if(stats_fsent.type == FAB_FSENT_TYPE_VAR_FILE)
    {
      z += descriptor_type_unmarshal(&stats_var, &descriptor_fab_var_stats, src + z, sz - z);
    }
  }
  else if(msg->id == describe_request_id)
  {
    describe_msgs[!describe_msg_index] = msg;
    descriptor_type_unmarshal(&node, &descriptor_fab_describe_item, msg->text, msg->size);
  }

  finally : coda;
}

static xapi setup()
{
  enter;

  uint16_t centery = LINES / 2;

  // top-left pinned
  fatal(area_init, &winpath, 3, 100, 0, 0);
  wattrset(winpath.body.w, COLOR_PAIR(1) | A_NORMAL);
  wattrset(winpath.border.w, COLOR_PAIR(2) | A_DIM);

  // top-right pinned
  fatal(area_init, &wingraph, 3, COLS - 100, 0, 100);
  wattrset(wingraph.body.w, COLOR_PAIR(1) | A_NORMAL);
  wattrset(wingraph.border.w, COLOR_PAIR(2) | A_DIM);

  // up-and-left of label
  fatal(area_init, &winup, (LINES / 2) - 4, 70, 3, 2);
  wattrset(winup.footer.w, COLOR_PAIR(1) | A_DIM);

  // vertically centered, pinned to and offset from left edge
  fatal(area_init, &winnode, 3, 70, centery - 1, 4);

  // down-and-right of label
  fatal(area_init, &windown, (LINES / 2) - 5, 70, (LINES / 2) + 2, 6);
  wattrset(windown.footer.w, COLOR_PAIR(1) | A_DIM);

  // right-side pinned
  fatal(area_init, &winproperties, LINES - 6, COLS - 70 - 6, 3, 76);
  wattrset(winproperties.body.w, COLOR_PAIR(1) | A_NORMAL);

  // bottom pinned
  fatal(area_init, &winmode, 3, COLS, LINES - 3, 0);
  wattrset(winmode.body.w, COLOR_PAIR(1) | A_NORMAL);
  wattrset(winmode.border.w, COLOR_PAIR(2) | A_DIM);

  finally : coda;
}

static void print_item(region *reg, int winnum, fab_list_item * restrict items, int x)
{
  if(selected_num[winnum] == x) {
    wattrset(reg->w, COLOR_PAIR(3) | A_BOLD);
  } else {
    wattrset(reg->w, COLOR_PAIR(1) | A_NORMAL);
  }

  if(items[x].type & FAB_FSENT_SHADOW) {
    wprintw(reg->w, "%.*s", (int)items[x].abspath_len, items[x].abspath);
  } else {
    wprintw(reg->w, "%.*s", (int)items[x].label_len, items[x].label);
  }

  wattrset(reg->w, COLOR_PAIR(1) | A_DIM);
  region_print_rightjust(reg, " %s\n", attrs16_name_byvalue(fab_fsent_type_attrs, items[x].type));
}

/* called on the ui thread */
static xapi redraw()
{
  enter;

  int x;
  descriptor_field *member;
  const char *str;
  uint16_t len;

  // top-left pinned
  fatal(wborder, winpath.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(winpath.border.w);

  werase(winpath.body.w);
  wprintw(winpath.body.w, "%.*s", node.abspath_len, node.abspath);
  wrefresh(winpath.body.w);

  // top-right pinned
  fatal(wborder, wingraph.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(wingraph.border.w);

  werase(wingraph.body.w);
  if(strcmp(graph, "depends") == 0) {
    wattrset(wingraph.body.w, COLOR_PAIR(1) | A_BOLD);
  } else {
    wattrset(wingraph.body.w, COLOR_PAIR(1) | A_DIM);
  }
  wprintw(wingraph.body.w, "depends");
  wprintw(wingraph.body.w, "  ");
  if(strcmp(graph, "fs") == 0) {
    wattrset(wingraph.body.w, COLOR_PAIR(1) | A_BOLD);
  } else {
    wattrset(wingraph.body.w, COLOR_PAIR(1) | A_DIM);
  }
  wprintw(wingraph.body.w, "fs");
  wattrset(wingraph.body.w, COLOR_PAIR(1) | A_DIM);
  wrefresh(wingraph.body.w);

  // up-and-left of label
  if(selected_win == 0) {
      wattrset(winup.border.w, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(winup.border.w, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, winup.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(winup.border.w);

  werase(winup.body.w);
  wattrset(winup.body.w, COLOR_PAIR(1) | A_NORMAL);
  for(x = 0; x < up_items_len; x++)
  {
    print_item(&winup.body, 0, up_items, x);
  }
  wrefresh(winup.body.w);

  werase(winup.footer.w);
  region_print_rightjust(&winup.footer, "showing %d of %d", up_items_len, up_items_count);
  wrefresh(winup.footer.w);

  // vertically centered, pinned to and offset from the left edge
  if(selected_win == 1) {
      wattrset(winnode.border.w, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(winnode.border.w, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, winnode.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(winnode.border.w);

  werase(winnode.body.w);
  wattrset(winnode.body.w, COLOR_PAIR(3) | A_BOLD);
  if(node.type & FAB_FSENT_SHADOW) {
    wprintw(winnode.body.w, "%.*s", node.abspath_len, node.abspath);
  } else {
    wprintw(winnode.body.w, "%.*s", node.label_len, node.label);
  }
  wattrset(winnode.body.w, COLOR_PAIR(1) | A_DIM);
  region_print_rightjust(&winnode.body, "%s", attrs16_name_byvalue(fab_fsent_type_attrs, node.type));
  wrefresh(winnode.body.w);

  // down-and-right of label
  if(selected_win == 2) {
      wattrset(windown.border.w, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(windown.border.w, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, windown.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(windown.border.w);

  werase(windown.body.w);
  wattrset(windown.body.w, COLOR_PAIR(1) | A_NORMAL);
  for(x = 0; x < down_items_len; x++)
  {
    print_item(&windown.body, 2, down_items, x);
  }
  wrefresh(windown.body.w);

  werase(windown.footer.w);
  region_print_rightjust(&windown.footer, "showing %d of %d", down_items_len, down_items_count);
  wrefresh(windown.footer.w);

  // right-side pinned
  wattrset(winproperties.border.w, COLOR_PAIR(2) | A_DIM);
  fatal(wborder, winproperties.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(winproperties.border.w);

  werase(winproperties.body.w);
  if(node.abspath_len) {
    wprintw(winproperties.body.w, "%s : %s\n", "type", attrs16_name_byvalue(fab_fsent_type_attrs, node.type));
    wprintw(winproperties.body.w, "%s : %s\n", "state", attrs16_name_byvalue(fab_fsent_state_attrs, node.state));

    for(x = 3; x < descriptor_fab_describe_item.members_len; x++)
    {
      member = descriptor_fab_describe_item.members[x];
      str = node.s[member->offset / sizeof(str)];
      len = node.u16[member->len_offset / sizeof(len)];
      wprintw(winproperties.body.w, "%.*s : %.*s\n", (int)member->name_len, member->name, (int)len, str);
    }
  }
  wprintw(winproperties.body.w, "\n");

  /* stats */
  for(x = 3; x < descriptor_fab_fsent_stats.members_len; x++)
  {
    member = descriptor_fab_fsent_stats.members[x];
    if(member->size == 8) {
      wprintw(winproperties.body.w, "%.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats_fsent.u64[member->offset / 8]);
    } else if(member->size == 4) {
      wprintw(winproperties.body.w, "%.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats_fsent.u32[member->offset / 4]);
    } else if(member->size == 2) {
      wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_fsent.u16[member->offset / 2]);
    } else if(member->size == 1) {
      wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_fsent.u8[member->offset / 1]);
    }
  }
  if(stats_fsent.type == FAB_FSENT_TYPE_MODULE_DIR)
  {
    for(x = 0; x < descriptor_fab_module_stats.members_len; x++)
    {
      member = descriptor_fab_module_stats.members[x];
      if(member->size == 8) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats_module.u64[member->offset / 8]);
      } else if(member->size == 4) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats_module.u32[member->offset / 4]);
      } else if(member->size == 2) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_module.u16[member->offset / 2]);
      } else if(member->size == 1) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_module.u8[member->offset / 1]);
      }
    }
  }
  else if(stats_fsent.type == FAB_FSENT_TYPE_MODULE_FILE)
  {
    for(x = 0; x < descriptor_fab_module_file_stats.members_len; x++)
    {
      member = descriptor_fab_module_file_stats.members[x];
      if(member->size == 8) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats_module_file.u64[member->offset / 8]);
      } else if(member->size == 4) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats_module_file.u32[member->offset / 4]);
      } else if(member->size == 2) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_module_file.u16[member->offset / 2]);
      } else if(member->size == 1) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_module_file.u8[member->offset / 1]);
      }
    }
  }
  else if(stats_fsent.type == FAB_FSENT_TYPE_FORMULA_FILE)
  {
    for(x = 0; x < descriptor_fab_formula_stats.members_len; x++)
    {
      member = descriptor_fab_formula_stats.members[x];
      if(member->size == 8) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats_formula.u64[member->offset / 8]);
      } else if(member->size == 4) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats_formula.u32[member->offset / 4]);
      } else if(member->size == 2) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_formula.u16[member->offset / 2]);
      } else if(member->size == 1) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_formula.u8[member->offset / 1]);
      }
    }
  }
  else if(stats_fsent.type == FAB_FSENT_TYPE_VAR_FILE)
  {
    for(x = 0; x < descriptor_fab_var_stats.members_len; x++)
    {
      member = descriptor_fab_var_stats.members[x];
      if(member->size == 8) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu64"\n", (int)member->name_len, member->name, stats_var.u64[member->offset / 8]);
      } else if(member->size == 4) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu32"\n", (int)member->name_len, member->name, stats_var.u32[member->offset / 4]);
      } else if(member->size == 2) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_var.u16[member->offset / 2]);
      } else if(member->size == 1) {
        wprintw(winproperties.body.w, "%.*s : %"PRIu16"\n", (int)member->name_len, member->name, stats_var.u8[member->offset / 1]);
      }
    }
  }

  wrefresh(winproperties.body.w);

  // bottom pinned
  fatal(wborder, winmode.border.w, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(winmode.border.w);

  werase(winmode.body.w);
  wattrset(winmode.body.w, COLOR_PAIR(1) | A_BOLD);
  wprintw(winmode.body.w, "graph explorer");
  wattrset(winmode.body.w, COLOR_PAIR(1) | A_DIM);
  wrefresh(winmode.body.w);

  finally : coda;
}

static void selection_wrap()
{
  if(   (selected_win == 0 && selected_num[0] >= up_items_len)
     || (selected_win == 2 && selected_num[2] >= down_items_len))
  {
    selected_num[selected_win] = 0;
  }
  else if(selected_num[selected_win] < 0)
  {
    if(selected_win == 0) {
      selected_num[selected_win] = MAX(up_items_len - 1, 0);
    } else if(selected_win == 2) {
      selected_num[selected_win] = MAX(down_items_len - 1, 0);
    }
  }
}

static int keypress(int key)
{
  if(key == KEY_UP || key == KEY_DOWN) {
    if(key == KEY_UP) {
      selected_num[selected_win]--;
    } else if (key == KEY_DOWN) {
      selected_num[selected_win]++;
    }

    selection_wrap();
    return 0;
  }
  if (key == '\t') {
    selected_win++;
    selected_win %= 3;

    return 0;
  }
  if(key == KEY_BTAB) {  /* shift + tab */
    if (selected_win == 0) {
      selected_win = 2;
    } else {
      selected_win--;
    }

    return 0;
  }
  if(key == KEY_ENTER) {
    if(selected_win == 0) {
      if(up_items_len == 0) {
        return 0;
      }
      lookup_path = up_items[selected_num[0]].abspath;
      lookup_pattern_len = up_items[selected_num[0]].abspath_len;
    } else if(selected_win == 1) {
      lookup_path = node.abspath;
      lookup_pattern_len = node.abspath_len;
    } else if(selected_win == 2) {
      if(down_items_len == 0) {
        return 0;
      }
      lookup_path = down_items[selected_num[2]].abspath;
      lookup_pattern_len = down_items[selected_num[2]].abspath_len;
    }

    selected_num[0] = 0;
    selected_num[1] = 0;
    selected_num[2] = 0;

    return REDRIVE;
  }
  if(key == KEY_CTL('g')) {
    if(strcmp(graph, "fs") == 0) {
      graph = "depends";
    } else {
      graph = "fs";
    }
    return REDRIVE;
  }

  return EXIT;
}

static display * explorer_display = (display[]) {{
    setup : setup
  , redraw : redraw
  , keypress : keypress
  , redrive : redrive
  , rebind : rebind
}};

xapi explorer_display_switch()
{
  enter;

  lookup_path = g_args.path;
  if((lookup_path = g_args.path)) {
    lookup_pattern_len = strlen(lookup_path);
  } else {
    lookup_path = ".";
    lookup_pattern_len = 1;
    graph = "fs";
  }
  fatal(display_switch, explorer_display);

  finally : coda;
}
