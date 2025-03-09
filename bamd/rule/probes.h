/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct rule;
struct fsent;
struct buildplan_entity;
struct module;
struct rule_run_context;
struct moria_edge;
struct vertices;
struct rule_module_association;

void rule_run_probe(
    const struct rule_run_context * restrict ctx
  , const struct buildplan_entity * restrict bpe
  , bool b
)
  __attribute__((nonnull(1)));

void rule_connect_probe(const struct rule * restrict rule, const struct buildplan_entity * restrict bpe)
  __attribute__((nonnull));

void bind_run_probe(
    const struct rule_module_association * restrict rma
  , const struct fsent * restrict attachment
  , const struct fsent * restrict target
  , const struct fsent * restrict binding
)
  __attribute__((nonnull(1)));

void bind_connect_probe(
    const struct rule * restrict bind
  , const struct module * restrict mod
  , const struct vertices * restrict points
)
  __attribute__((nonnull(1, 2)));
