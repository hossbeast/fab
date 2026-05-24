/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"

#include "value.h"
#include "writer.h"


static inline void say(const bacon_value * const restrict val, narrator * const restrict N, bool flat)
{
  bacon_writer lwriter;
  bacon_writer * writer = 0;

  if(val == 0)
  {
    xsays("-none-");
  }
  else
  {
    bacon_writer_init(&lwriter);
    writer = &lwriter;
    writer->flat = flat;
    bacon_writer_open(writer, N);
    bacon_writer_value(writer, val);
    bacon_writer_close(writer);
    bacon_writer_destroy(writer);
  }
}

void bacon_value_say(const bacon_value * const restrict val, narrator * const restrict N)
{
  say(val, N, false);
}

void bacon_value_say_flat(const bacon_value * const restrict val, narrator * const restrict N)
{
  say(val, N, true);
}

void bacon_value_show(const bacon_value * const restrict val)
{
  say(val, g_narrator_stdout, false);
  printf("\n");
}
