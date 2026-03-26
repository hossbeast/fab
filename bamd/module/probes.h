/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct module;
struct ruleset;

void ruleset_rebuild_probe(const struct ruleset * restrict rules)
  __attribute__((nonnull));

void module_rebuild_probe(const struct module * restrict mod)
  __attribute__((nonnull));

void module_reparse_probe(const struct module * restrict mod)
  __attribute__((nonnull));

void module_remake_probe(const struct module * restrict mod)
  __attribute__((nonnull));

void module_reimport_probe(const struct module * restrict mod)
  __attribute__((nonnull));

void module_reload_probe(const struct module * restrict mod)
  __attribute__((nonnull));
