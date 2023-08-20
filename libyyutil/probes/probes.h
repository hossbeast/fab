/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct yyu_parser;
struct yyu_location;

extern unsigned short yyu_token_semaphore;
extern unsigned short yyu_pushstate_semaphore;
extern unsigned short yyu_popstate_semaphore;

void popstate_probe(const struct yyu_parser * restrict parser, int prev, int next);
void pushstate_probe(const struct yyu_parser * restrict parser, int prev, int next);
void token_probe(
    struct yyu_parser * const restrict xtra
  , int token
  , void * const restrict lval
  , struct yyu_location * const restrict lloc
  , char * restrict text
  , size_t bytes
);
