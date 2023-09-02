/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "builder.h"

void bacon_builder_pop_set(bacon_writer ** restrict writerp) { bacon_writer_pop_set(*writerp); }
void bacon_builder_pop_mapping(bacon_writer ** restrict writerp) { bacon_writer_pop_mapping(*writerp); }
void bacon_builder_pop_list(bacon_writer ** restrict writerp) { bacon_writer_pop_list(*writerp); }
