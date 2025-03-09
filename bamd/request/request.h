/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct narrator;
struct bacon_writer;

typedef struct request {
  llist commands;           // struct command
} request;

void request_init(request * restrict req)
  __attribute__((nonnull));

void request_destroy(request * restrict req)
  __attribute__((nonnull));

void request_say(request * restrict req, struct narrator * restrict N)
  __attribute__((nonnull));

void request_cleanup(void);

void request_describe(request * const restrict req, struct bacon_writer * const restrict builder_writer)
  __attribute__((nonnull));

void request_show(request * restrict req)
  __attribute__((nonnull));
