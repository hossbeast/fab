/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "yyutil/parser.h"

typedef struct events_parser {
  yyu_parser yyu;

  /* state */
  char _per_event_state;

  char *strp;
  uint16_t *str_lenp;
  size_t str_sz;
  size_t str_z;

  // (returns)
  int level;

  char type[64];
  uint16_t type_len;

  char id[64];
  uint16_t id_len;

  char path[PATH_MAX];
  uint16_t path_len;

  char runtime[128];
  uint16_t runtime_len;

  struct {
    char id[64];
    uint16_t id_len;
    char path[PATH_MAX];
    uint16_t path_len;
  } attachment;

  struct {
    char id[64];
    uint16_t id_len;
    char path[PATH_MAX];
    uint16_t path_len;
  } target;

  struct {
    char id[64];
    uint16_t id_len;
    char path[PATH_MAX];
    uint16_t path_len;
  } binding;

  struct {
    char id[64];
    uint16_t id_len;
    char path[PATH_MAX];
    uint16_t path_len;
  } hit;

  struct {
    char id[64];
    uint16_t id_len;
    char path[PATH_MAX];
    uint16_t path_len;
  } miss;
} events_parser;
