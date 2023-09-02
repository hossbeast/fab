/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct moria_edge;
struct moria_parser;
struct moria_vertex;

typedef struct moria_operations_vtable {
  /* called at the start of operations_perform */
  void (*setup)(struct moria_parser * restrict)
    __attribute__((nonnull));

  /* called at the end of operations_perform */
  void (*cleanup)(struct moria_parser * restrict)
    __attribute__((nonnull));

  void (*connect)(
      struct moria_parser * restrict parser
    , struct moria_vertex * restrict B
    , struct moria_vertex * restrict A
    , uint32_t attrs
    , struct moria_edge ** restrict e
  )
    __attribute__((nonnull(1, 2, 3)));

  void (*hyperconnect)(
      struct moria_parser * restrict parser
    , struct moria_vertex ** restrict Blist
    , uint16_t Blen
    , struct moria_vertex ** restrict Alist
    , uint16_t Alen
    , uint32_t attrs
    , struct moria_edge ** restrict e
  )
    __attribute__((nonnull(1)));

  void (*disconnect)(
      struct moria_parser * restrict parser
    , struct moria_edge * restrict e
  )
    __attribute__((nonnull));

  void (*create_vertex)(
      struct moria_parser * restrict parser
    , struct moria_vertex ** restrict v
    , uint32_t attrs
    , uint8_t opattrs
    , const char * restrict label
    , uint16_t label_len
  )
    __attribute__((nonnull));

  void (*invalidate_vertex)(
      struct moria_parser * restrict parser
    , struct moria_vertex * restrict v
  )
    __attribute__((nonnull));

  void (*refresh_vertex)(
      struct moria_parser * restrict parser
    , struct moria_vertex * restrict v
  )
    __attribute__((nonnull));
} moria_operations_vtable;

void moria_parser_setup(struct moria_parser * restrict p)
  __attribute__((nonnull));

void moria_parser_cleanup(struct moria_parser * restrict p)
  __attribute__((nonnull));

void moria_parser_connect(
    struct moria_parser * restrict p
  , struct moria_vertex * restrict B
  , struct moria_vertex * restrict A
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1, 2, 3)));

void moria_parser_hyperconnect(
    struct moria_parser * restrict p
  , struct moria_vertex ** restrict Blist
  , uint16_t Blen
  , struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1)));

/* graph parser */
void moria_parser_create_vertex(
    struct moria_parser * restrict p
  , struct moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

extern struct moria_operations_vtable * moria_operations_dispatch;
