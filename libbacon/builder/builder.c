/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "builder.h"

void bacon_builder_pop_set(bacon_writer ** restrict writerp)
{
  bacon_writer_pop_set(*writerp);
}

void bacon_builder_pop_mapping(bacon_writer ** restrict writerp)
{
  bacon_writer_pop_mapping(*writerp);
}

void bacon_builder_pop_list(bacon_writer ** restrict writerp)
{
  bacon_writer_pop_list(*writerp);
}

bool bacon_builder_push_set_if_mapping(bacon_writer * restrict builder_writer)
{
  if(!(bacon_writer_is_mapping(builder_writer))) {
    return false;
  }

  bacon_writer_push_set(builder_writer);
  return true;
}

void bacon_builder_pop_set_if_mapping(struct bacon_builder_if_mapping * restrict rec)
{
  if(!rec->b) {
    return;
  }

  bacon_writer_pop_set(rec->builder_writer);
}
