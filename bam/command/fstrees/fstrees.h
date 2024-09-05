/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "special.h"
#include "args.h"

extern struct {
  command cmd;

  /* arguments */
  char *target;        // path to resolve the module of */
  uint16_t target_len;
  char fstree_id[40];
  uint16_t fstree_id_len;

  /* state */
  uint64_t identify_id;
  uint64_t describe_id;
  bool any;
} z;

extern struct command fstrees_command;
extern struct command fstree_command;
