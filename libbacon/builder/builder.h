/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "bacon/writer.h"

void bacon_builder_pop_set(bacon_writer ** restrict writerp)
  __attribute__((nonnull));

void bacon_builder_pop_mapping(bacon_writer ** restrict writerp)
  __attribute__((nonnull));

void bacon_builder_pop_list(bacon_writer ** restrict writerp)
  __attribute__((nonnull));

struct bacon_builder_if_mapping {
  struct bacon_writer *builder_writer;
  bool b;
};

bool bacon_builder_push_set_if_mapping(bacon_writer * restrict)
  __attribute__((nonnull));

void bacon_builder_pop_set_if_mapping(struct bacon_builder_if_mapping * restrict rec)
  __attribute__((nonnull));

#define _CONCAT(x, y) __CONCAT(x, y)
#define __CONCAT(x, y) x ## y

#define SET \
  __attribute__((cleanup(bacon_builder_pop_set))) bacon_writer *_CONCAT(__builder_set_, __LINE__) = builder_writer; \
  bacon_writer_push_set(builder_writer);

#define MAPPING \
  __attribute__((cleanup(bacon_builder_pop_mapping))) bacon_writer *_CONCAT(__builder_mapping_, __LINE__) = builder_writer; \
  bacon_writer_push_mapping(builder_writer);

#define LIST \
  __attribute__((cleanup(bacon_builder_pop_list))) bacon_writer *_CONCAT(__builder_list_, __LINE__) = builder_writer; \
  bacon_writer_push_list(builder_writer);

#define SET_IF_MAPPING \
  __attribute__((cleanup(bacon_builder_pop_set_if_mapping))) struct bacon_builder_if_mapping _CONCAT(__builder_if_mapping, __LINE__) = { \
    builder_writer, bacon_builder_push_set_if_mapping(builder_writer) }; \

#define BYTES(x, y) bacon_writer_bytes(builder_writer, x, y);
#define STRING(x) bacon_writer_string(builder_writer, x);
#define LOADF(fmt, ...) bacon_writef(builder_writer, fmt, ##__VA_ARGS__);
#define INT(x) bacon_writer_int(builder_writer, x);
#define UINT(x) bacon_writer_uint(builder_writer, x);
#define BOOL(x) bacon_writer_bool(builder_writer, !!(x));
#define HEX(x) bacon_writer_hex(builder_writer, (uint64_t)x);
#define HEX64(x) bacon_writer_hex64(builder_writer, (uint64_t)x);
#define HEX256(x) bacon_writef(builder_writer, "0x%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64, x[0], x[1], x[2], x[3]);
#define HEX512(x) bacon_writef(builder_writer                                                           \
  , "0x%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64  \
  , x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]                                                      \
);
#define FLOAT(x) LOADF("%.6lf", x);
#define VARIABLEW(s, l) bacon_writer_variable(builder_writer, s, l);

#define COMMENTF(fmt, ...) bacon_writer_commentf(builder_writer, fmt, ##__VA_ARGS__);
#define COMMENTS(s) bacon_writer_comments(builder_writer, s);
