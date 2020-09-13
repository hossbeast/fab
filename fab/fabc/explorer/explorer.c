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
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xlinux/xshm.h"

#include "explorer.h"
#include "xcurses.h"
#include "args.h"

#include "zbuffer.h"

#define PROP_ABSDIR 0
#define PROP_NAME   1
#define PROP_TYPE   2

FILE *fp;

static char up_text[4096];
static char *up_labels[128];          // pointers into up_text
static uint16_t up_labels_lens[128];
static uint16_t up_labels_len;

static char down_text[4096];
static char *down_labels[128];        // pointers into down_text
static uint16_t down_labels_lens[128];
static uint16_t down_labels_len;

static char describe_text[4096];
static char *properties[8];           // pointers into describe_text
static uint16_t properties_lens[8];

static char *abspath;             // pointer into describe_text
static uint16_t abspath_len;

static int selected_win = 1;
static int selected_num[3] = { -1, -1, -1 };

static char lookup_pattern[256];

static WINDOW *win_path;
static WINDOW *win2;
static WINDOW *win_label;
static WINDOW *win_properties;
static WINDOW *win5;
static WINDOW *win_up;
static WINDOW *win_down;

static xapi windows_setup()
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

static xapi strong_up_request(value_writer * restrict writer)
{
  enter;

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          fatal(value_writer_string, writer, lookup_pattern);
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

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          fatal(value_writer_string, writer, lookup_pattern);
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

  fatal(value_writer_push_list, writer);
    fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "select");
      fatal(value_writer_push_list, writer);
        fatal(value_writer_push_mapping, writer);
          fatal(value_writer_string, writer, "pattern");
          fatal(value_writer_string, writer, lookup_pattern);
        fatal(value_writer_pop_mapping, writer);
      fatal(value_writer_pop_list, writer);
    fatal(value_writer_pop_mapping, writer);
    fatal(value_writer_string, writer, "describe");
  fatal(value_writer_pop_list, writer);

  finally : coda;
}

static xapi make_request(fab_client * restrict client, xapi (*render)(value_writer * restrict writer), void **response_shm)
{
  enter;

  narrator_fixed nstor;
  value_writer writer;
  narrator * request_narrator;
  uint32_t message_len;
  void * request_shm = 0;
  void * shmaddr;

  value_writer_init(&writer);

  fatal(fab_client_prepare_request_shm, client, &request_shm);

  // save a spot for the message length
  request_narrator = narrator_fixed_init(&nstor, request_shm, 0xfff);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0xde, 0xad, 0xbe, 0xef }, 4);
  fatal(value_writer_open, &writer, request_narrator);

  fatal(render, &writer);

  // stitch up the message length
  fatal(value_writer_close, &writer);
  fatal(narrator_xsayw, request_narrator, (char[]) { 0x00, 0x00 }, 2);
  message_len = nstor.l - 4;
  fatal(narrator_xseek, request_narrator, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xsayw, request_narrator, &message_len, sizeof(message_len));

  /* perform request/response */
  shmaddr = request_shm;
  request_shm = 0;
  fatal(fab_client_make_request, client, shmaddr, response_shm);

finally:
  fatal(value_writer_destroy, &writer);
  fatal(xshmdt, request_shm);
coda;
}

static inline void unwrap_list(const char **sp, uint16_t *lp)
{
  const char *s = *sp;
  uint16_t l = *lp;

  // leading whitespace
  while(*s == ' ' || *s == '\n') {
    s++;
    l--;
  }

  // trailing whitespace
  while(s[l - 1] == ' ' || s[l - 1] == '\n') {
    l--;
  }

  // outermost list
  s++;
  l--;
  l--;

  // leading whitespace
  while(*s == ' ' || *s == '\n') {
    s++;
    l--;
  }

  // trailing whitespace
  while(s[l - 1] == ' ' || s[l - 1] == '\n') {
    l--;
  }

  *sp = s;
  *lp = l;
}

static inline void count_list(char *text, char ** items, uint16_t *items_lens, uint16_t *items_len)
{
  char *s0, *s;
  uint16_t n;

  // skip to the second line
  s0 = text;
  while(*s0 != '\n')
    s0++;

  // trim leading whitespace
  s0++;
  while(*s0 == ' ')
    s0++;

  s = s0;
  n = 0;
  while(*s)
  {
    if(*s == '\n')
    {
      items[n] = s0;
      items_lens[n] = s - s0;
      n++;

      s0 = s + 1;
      while(*s0 == ' ')
        s0++;
      s = s0;
    }
    else
    {
      s++;
    }
  }

  *items_len = n;
}

static inline void parse_describe(char *text, char **abspathp, uint16_t *abspath_lenp)
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

/*
 * retrieve node data
 */
static xapi getdata(fab_client * restrict client)
{
  enter;

  void * response_shm = 0;
  void * shmaddr;
  fab_message *msg;
  const char *text;
  uint16_t len;

  // upwards
  fatal(make_request, client, strong_up_request, &response_shm);

  // consume the response
  msg = response_shm;
  text = msg->text;
  len = msg->len;
  unwrap_list(&text, &len);
  memcpy(up_text, text, len);
  up_text[len] = 0;
  count_list(up_text, up_labels, up_labels_lens, &up_labels_len);

  // release the response
  shmaddr = response_shm;
  response_shm = 0;
  fatal(fab_client_release_response, client, shmaddr);

  // downwards
  fatal(make_request, client, strong_down_request, &response_shm);

  // consume the response
  msg = response_shm;
  text = msg->text;
  len = msg->len;
  unwrap_list(&text, &len);
  memcpy(down_text, text, len);
  down_text[len] = 0;
  count_list(down_text, down_labels, down_labels_lens, &down_labels_len);

fprintf(fp, "'%s'\n", down_text);
fprintf(fp, ">>>>>\n");
int x;
for(x = 0; x < down_labels_len; x++) {
  fprintf(fp, "Z%.*sQ\n", (int)down_labels_lens[x], down_labels[x]);
}
fprintf(fp, "%d items\n", down_labels_len);

  // release the response
  shmaddr = response_shm;
  response_shm = 0;
  fatal(fab_client_release_response, client, shmaddr);

  // downwards
  fatal(make_request, client, describe, &response_shm);

  // consume the response
  msg = response_shm;
  text = msg->text;
  len = msg->len;
  unwrap_list(&text, &len);
  memcpy(describe_text, text, len);
  describe_text[len] = 0;
  parse_describe(describe_text, &abspath, &abspath_len);

  // release the response
  shmaddr = response_shm;
  response_shm = 0;
  fatal(fab_client_release_response, client, shmaddr);

finally:
  fatal(xshmdt, response_shm);
coda;
}

xapi explorer_main(fab_client * restrict client)
{
  enter;

  int key;
  int x;
  bool done;
  bool rebind;
  char *label;
  uint16_t label_len;
  size_t z;

fp = fopen("/tmp/a", "w");
setvbuf(fp, NULL, _IONBF, 0);

  // initial lookup pattern
  x = MIN(sizeof(lookup_pattern) - 1, strlen(g_args.lookup));
  memcpy(lookup_pattern, g_args.lookup, x);
  lookup_pattern[x] = 0;

  fatal(getdata, client);
  fatal(windows_setup);

  done = false;
  rebind = false;
  while(!done)
  {
    if(rebind)
    {
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

      fatal(getdata, client);
      rebind = false;
      selected_win = 1;
      selected_num[0] = -1;
      selected_num[2] = -1;
    }

    while(!done && !rebind)
    {
      fatal(draw);

      key = getch();
fprintf(fp, " 0x%04x %s\n", key, key_name(key));

      switch (key)
      {
        case KEY_DOWN:
          selected_num[selected_win]++;
          break;
        case KEY_UP:
          selected_num[selected_win]--;
          break;
        case '\t':
          selected_win++;
          selected_win %= 3;
fprintf(fp, "win %d\n", selected_win);
          break;
        case KEY_BTAB:  /* shift+tab */
          if (selected_win == 0) {
            selected_win = 2;
          } else {
            selected_win--;
          }
          break;
fprintf(fp, "win %d\n", selected_win);
        case KEY_ENTER:
        case '\r':
        case '\n':
          rebind = true;
          break;
        default:
          done = true;
      }
    }
  }

  endwin();

  finally : coda;
}

#if 0
#endif

#if 0

  while(1);

//  nonl();
//  intrflush(stdscr, false);
//  keypad(stdscr, true);

  int x;
  for(x = 0; x < 50; x++)
  {
    mvaddch(x + 0, 0, 'a');
    mvaddch(x + 1, 10, 'a');
    mvaddch(x + 2, 20, 'a');
    mvaddch(x + 3, 30, 'a');
    mvaddch(x + 4, 40, 'a');
    mvaddch(x + 5, 50, 'a');
    refresh();
    sleep(1);
  }

  while(1);
#endif
