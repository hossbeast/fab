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

#include <curses.h>
#include <stdlib.h>

#include "fab/client.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/xshm.h"

#include "explorer.h"
#include "xcurses.h"
#include "args.h"
#include "display.h"

#include "zbuffer.h"

#define PROP_ABSDIR 0
#define PROP_NAME   1
#define PROP_TYPE   2

static char up_text[4096];
static uint16_t up_text_len;
static char *up_labels[128];          // pointers into up_text
static uint16_t up_labels_lens[128];
static uint16_t up_labels_len;

static char down_text[4096];
static uint16_t down_text_len;
static char *down_labels[128];        // pointers into down_text
static uint16_t down_labels_lens[128];
static uint16_t down_labels_len;

static char describe_text[4096];
static char *properties[8];           // pointers into describe_text
static uint16_t properties_lens[8];

static int selected_win = 1;
static int selected_num[3] = { -1, -1, -1 };

static const char *lookup_pattern;
static uint16_t lookup_pattern_len;

static WINDOW *win_path;
static WINDOW *win2;
static WINDOW *win_label;
static WINDOW *win_properties;
static WINDOW *win5;
static WINDOW *win_up;
static WINDOW *win_down;

static uint32_t msgid;
static uint32_t strong_up_id;
static uint32_t strong_down_id;
static uint32_t describe_id;

static xapi strong_up_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          if(lookup_pattern_len > 0 && lookup_pattern[0] != '.' && lookup_pattern[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_pattern, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_string, writer, lookup_pattern);
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
              fatal(value_writer_string, writer, "relation");
              fatal(value_writer_string, writer, "strong");
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

static xapi strong_down_request(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          if(lookup_pattern_len > 0 && lookup_pattern[0] != '.' && lookup_pattern[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_pattern, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_string, writer, lookup_pattern);
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
              fatal(value_writer_string, writer, "relation");
              fatal(value_writer_string, writer, "strong");
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

static xapi describe(value_writer * restrict writer)
{
  enter;

  char space[256];
  uint16_t z;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          if(lookup_pattern_len > 0 && lookup_pattern[0] != '.' && lookup_pattern[0] != '/') {
            z = 0;
            z += znloads(space + z, sizeof(space) - z, "./");
            z += znloadw(space + z, sizeof(space) - z, lookup_pattern, lookup_pattern_len);
            fatal(value_writer_bytes, writer, space, z);
          } else {
            fatal(value_writer_string, writer, lookup_pattern);
          }
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "describe");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

/*
 * request new data - called on the client thread
 */
static xapi redrive(fab_client * restrict client)
{
  enter;

  narrator_fixed nstor;
  value_writer writer;
  narrator * request_narrator;
  uint32_t message_len;
  fabipc_message *msg;

#if 0
  label = 0;
  const char *label;
  uint16_t label_len;
  size_t z;

  label = 0;
  if(selected_win == 0) {
    label = up_labels[selected_num[0]];
    label_len = up_labels_lens[selected_num[0]];
  } else if(selected_win == 2) {
    label = down_labels[selected_num[2]];
    label_len = down_labels_lens[selected_num[2]];
  }

  if(label)
  {
    z = 0;
    if(label[0] != '.' && label[0] != '/')
    {
      z += znloads(lookup_pattern + z, sizeof(lookup_pattern) - z, "./");
    }
    z += znloadw(lookup_pattern + z, sizeof(lookup_pattern) - z, label, label_len);
    lookup_pattern[z] = 0;
  }
#endif

  value_writer_init(&writer);

  // upwards dependencies
  up_labels_len = 0;
  msg = fab_client_produce(client, 0);
  strong_up_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(strong_up_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fab_client_post(client);

  // downwards dependencies
  down_labels_len = 0;
  msg = fab_client_produce(client, 0);
  strong_down_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(strong_down_request, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fab_client_post(client);

  // node details
  describe_text[0] = 0;
  msg = fab_client_produce(client, 0);
  describe_id = msg->id = ++msgid;
  request_narrator = narrator_fixed_init(&nstor, msg->text, 0xfff);
  fatal(value_writer_open, &writer, request_narrator);
  fatal(describe, &writer);
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l;
  msg->size = message_len;
  msg->type = FABIPC_MSG_REQUEST;
  fab_client_post(client);

printf("redrive : up %u down %u describe %u\n", strong_up_id, strong_down_id, describe_id);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

static void parse_describe(char *text)
{
  char *s0, *s;
  uint16_t len;

  s0 = text;
  s = s0;
  while(*s)
  {
    if(*s == '\n')
    {
      len = s - s0;
      if(len > 9 && memncmp(s0, 9, MMS("absdir : ")) == 0)
      {
        properties[PROP_ABSDIR] = s0 + 9;
        properties_lens[PROP_ABSDIR] = len - 9;
      }
      else if(len > 7 && memncmp(s0, 7, MMS("name : ")) == 0)
      {
        properties[PROP_NAME] = s0 + 7;
        properties_lens[PROP_NAME] = len - 7;
      }
      else if(len > 7 && memncmp(s0, 7, MMS("type : ")) == 0)
      {
        properties[PROP_TYPE] = s0 + 7;
        properties_lens[PROP_TYPE] = len - 7;
      }

      s0 = s + 1;
      while(*s0 == ' ')
        s0++;
      s = s0;
    }
    s++;
  }
}

static void rebind(const fabipc_message * restrict msg)
{
//printf("rebind %u sz %hu text %.*s\n", msg->id, msg->size, (int)msg->size + 1, msg->text);

  if(msg->id == strong_up_id)
  {
    if(msg->size > (sizeof(up_text) - up_text_len))
      return;
    if(up_labels_len == (sizeof(up_labels) / sizeof(*up_labels)))
      return;

    up_labels[up_labels_len] = up_text + up_text_len;
    memcpy(up_text + up_text_len, msg->text, msg->size);
    up_labels_lens[up_labels_len] = msg->size;
    up_text_len += msg->size;
    up_labels_len++;
  }
  else if(msg->id == strong_down_id)
  {
    if(msg->size > (sizeof(down_text) - down_text_len))
      return;
    if(down_labels_len == (sizeof(down_labels) / sizeof(*down_labels)))
      return;

    down_labels[down_labels_len] = down_text + down_text_len;
    memcpy(down_text + down_text_len, msg->text, msg->size);
    down_labels_lens[down_labels_len] = msg->size;
    down_text_len += msg->size;
    down_labels_len++;
  }
  else if(msg->id == describe_id)
  {
    if(msg->size > sizeof(describe_text))
      return;

    memcpy(describe_text, msg->text, msg->size);
    describe_text[msg->size] = 0;
    parse_describe(describe_text);
  }
}

static xapi setup()
{
  enter;

//  uint16_t centerx = COLS / 2;
  uint16_t centery = LINES / 2;

// newwin(y extent, x extent, y location, x location)

  // top-left pinned
  fatal(xnewwin, &win_path, 3, 100, 0, 0);

  // top-right pinned
  fatal(xnewwin, &win2, 3, COLS - 100, 0, 100);

  // up-and-left of label
  fatal(xnewwin, &win_up, (LINES / 2) - 4, 70, 3, 2);

  // vertically centered, pinned to and offset from left edge
  fatal(xnewwin, &win_label, 3, 70, centery - 1, 4);

  // down-and-right of label
  fatal(xnewwin, &win_down, (LINES / 2) - 5, 70, (LINES / 2) + 2, 6);

  // right-side pinned
  fatal(xnewwin, &win_properties, LINES - 6, COLS - 70 - 6, 3, 76);

  // bottom pinned
  fatal(xnewwin, &win5, 3, COLS, LINES - 3, 0);

  finally : coda;
}

static xapi draw()
{
  enter;

  int x;

  // top-left pinned
  werase(win_path);
  wattrset(win_path, COLOR_PAIR(1) | A_NORMAL);
  mvwprintw(win_path, 1, 1, "%.*s", (int)properties_lens[PROP_ABSDIR], properties[PROP_ABSDIR]);
  wattrset(win_path, COLOR_PAIR(2) | A_DIM);
  fatal(wborder, win_path, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win_path);

  // top-right pinned
  werase(win2);
  wattrset(win2, COLOR_PAIR(1) | A_NORMAL);
  mvwprintw(win2, 1, 1, "filesystem * dependency * imports");
  fatal(wborder, win2, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win2);

  // up-and-left of label
  werase(win_up);
  wattrset(win_up, COLOR_PAIR(1) | A_NORMAL);
  wmove(win_up, 1, 1);
  wprintw(win_up, "list : [\n");
  for(x = 0; x < up_labels_len; x++)
  {
    if(selected_num[0] == x) {
      wattrset(win_up, COLOR_PAIR(3) | A_BOLD);
      wprintw(win_up, "   %.*s\n", up_labels_lens[x], up_labels[x]);
      wattrset(win_up, COLOR_PAIR(1) | A_NORMAL);
    } else {
      wprintw(win_up, "   %.*s\n", up_labels_lens[x], up_labels[x]);
    }
  }
  wprintw(win_up, " ]\n");
  wattrset(win_up, COLOR_PAIR(2) | A_DIM);
  if(selected_win == 0) {
      wattrset(win_up, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(win_up, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, win_up, 0, 0, 0, 0, 0, 0, 0, 0);
  wattrset(win_up, COLOR_PAIR(1) | A_NORMAL);
  wrefresh(win_up);

  // vertically centered, pinned to and offset from left edge
  werase(win_label);
  wattrset(win_label, COLOR_PAIR(1) | A_NORMAL);
  mvwprintw(win_label, 1, 1, "%.*s %.*s"
    , (int)properties_lens[PROP_NAME]
    , properties[PROP_NAME]
    , (int)properties_lens[PROP_TYPE]
    , properties[PROP_TYPE]
  );
  if(selected_win == 1) {
      wattrset(win_label, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(win_label, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, win_label, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win_label);

  // down-and-right of label
  werase(win_down);
  wattrset(win_down, COLOR_PAIR(1) | A_NORMAL);
  wmove(win_down, 1, 1);
  wprintw(win_down, "list : [\n");
  for(x = 0; x < down_labels_len; x++)
  {
    if(selected_num[2] == x) {
      wattrset(win_down, COLOR_PAIR(3) | A_BOLD);
      wprintw(win_down, "   %.*s\n", down_labels_lens[x], down_labels[x]);
      wattrset(win_down, COLOR_PAIR(1) | A_NORMAL);
    } else {
      wprintw(win_down, "   %.*s\n", down_labels_lens[x], down_labels[x]);
    }
  }
  wprintw(win_down, " ]\n");
  if(selected_win == 2) {
      wattrset(win_down, COLOR_PAIR(1) | A_BOLD);
  } else {
      wattrset(win_down, COLOR_PAIR(1) | A_DIM);
  }
  fatal(wborder, win_down, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win_down);

  // right-side pinned
  werase(win_properties);
  wattrset(win_properties, COLOR_PAIR(1) | A_NORMAL);
  mvwprintw(win_properties, 1, 1, describe_text);
  wattrset(win_properties, COLOR_PAIR(2) | A_DIM);
  fatal(wborder, win_properties, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win_properties);

  // bottom pinned
  werase(win5);
  wattrset(win5, COLOR_PAIR(1) | A_NORMAL);
  mvwprintw(win5, 1, 1, "graph explorer * build status * statistics");
  wattrset(win5, COLOR_PAIR(2) | A_DIM);
  fatal(wborder, win5, 0, 0, 0, 0, 0, 0, 0, 0);
  wrefresh(win5);

  finally : coda;
}

static int keypress(int key)
{
  if(key == KEY_UP || key == KEY_DOWN) {
    if(key == KEY_UP) {
      selected_num[selected_win]--;
    } else if (key == KEY_DOWN) {
      selected_num[selected_win]++;
    }

    if(   (selected_win == 0 && selected_num[0] >= up_labels_len)
       || (selected_win == 2 && selected_num[2] >= down_labels_len))
    {
      selected_num[selected_win] = 0;
    }
    else if(selected_num[selected_win] < 0)
    {
      if(selected_win == 0) {
        selected_num[selected_win] = MAX(up_labels_len - 1, 0);
      } else if(selected_win == 2) {
        selected_num[selected_win] = MAX(down_labels_len - 1, 0);
      }
    }
  } else if (key == '\t') {
    selected_win++;
    selected_win %= 3;
  } else if(key == KEY_BTAB) {  /* shift + tab */
    if (selected_win == 0) {
      selected_win = 2;
    } else {
      selected_win--;
    }
  } else if(key == KEY_ENTER) {
    lookup_pattern = 0;
    lookup_pattern_len = 0;
    if(selected_win == 0) {
      lookup_pattern = up_labels[selected_num[0]];
      lookup_pattern_len = up_labels_lens[selected_num[0]];
    } else if(selected_win == 2) {
      lookup_pattern = down_labels[selected_num[2]];
      lookup_pattern_len = down_labels_lens[selected_num[2]];
    }

    return REBIND;
  } else {
    return EXIT;
  }

  return 0;
}

static display * explorer_display = (display[]) {{
    setup : setup
  , draw : draw
  , keypress : keypress
  , redrive : redrive
  , rebind : rebind
}};

xapi explorer_display_switch()
{
  enter;

  lookup_pattern = g_args.lookup;
  if(lookup_pattern) {
    lookup_pattern_len = strlen(lookup_pattern);
  }
  fatal(display_switch, explorer_display);

  finally : coda;
}
