/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#define DESCRIPTOR_CODE_TABLE                 \
  DEF(DESCRIPTOR_INT      , 1)                \
  DEF(DESCRIPTOR_UINT     , 2)                \
  DEF(DESCRIPTOR_STRING   , 3) /* char* */    \
  DEF(DESCRIPTOR_LIST     , 4)                \

typedef enum descriptor_code {
#undef DEF
#define DEF(x, y) x = y,
DESCRIPTOR_CODE_TABLE
} descriptor_code;

typedef struct descriptor_field {
  const char *name;
  uint8_t name_len;
  uint16_t code;
  uint16_t offset;
  union {
    uint8_t size;             // integer
    uint32_t len_offset;      // string, list
  };
  uint32_t size_offset;       // list
} descriptor_field;

typedef struct descriptor_type {
  uint32_t id;
  const char *name;
  uint8_t name_len;
  uint16_t members_len;
  descriptor_field **members;
} descriptor_type;

/*
 * write a field to a buffer
 */
size_t descriptor_field_marshal(void *restrict dst, size_t sz, descriptor_field *restrict desc, void *restrict object)
  __attribute__((nonnull));

size_t descriptor_marshal_int(void *restrict dst, size_t sz, void * restrict x, uint8_t xsz)
  __attribute__((nonnull));

size_t descriptor_marshal_string(void *restrict dst, size_t sz, void * restrict s, uint16_t slen)
  __attribute__((nonnull));

/*
 * populate an object from marshaled data
 */
size_t descriptor_type_unmarshal(void *restrict object, descriptor_type *restrict desc, void *restrict src, size_t sz)
  __attribute__((nonnull));
