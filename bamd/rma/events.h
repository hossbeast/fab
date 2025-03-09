/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct rule_run_context;
struct pattern_search_hit;
struct rma_operation;
struct moria_vertex;
struct fsent;

void rma_event_search(struct rule_run_context * restrict ctx, const struct pattern_search_hit *hit)
  __attribute__((nonnull));

void rma_event_generate(struct rule_run_context * restrict ctx, const struct moria_vertex *hit)
  __attribute__((nonnull));

void rma_event_start(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void rma_event_end(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void rma_event_resolve(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void rma_event_bind(
    struct rule_run_context * restrict ctx
  , const struct fsent * restrict attachment
  , const struct fsent * restrict target
  , const struct fsent * restrict binding
)
  __attribute__((nonnull(1, 2)));

struct rma_operation *rma_operation_push(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

void rma_operation_pop(struct rule_run_context * restrict ctx)
  __attribute__((nonnull));
