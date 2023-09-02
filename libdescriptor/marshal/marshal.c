/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "descriptor.h"
#include "marshal.h"

//
// static
//

size_t descriptor_marshal_int(void *restrict dst, size_t sz, void * restrict xp, uint8_t xsz)
{
  size_t z;

  z = 0;
  z += marshal_int(dst + z, sz - z, xp, xsz);

  return z;
}

size_t descriptor_marshal_string(void *restrict dst, size_t sz, void * restrict s, uint16_t slen)
{
  size_t z;

  /* string-blob
   *  (2) length of string : n
   *  (n) string data
   */

  z = 0;
  z += marshal_u16(dst + z, sz - z, slen);

  RUNTIME_ASSERT((sz - z) >= slen);
  memcpy(dst + z, s, slen);
  z += slen;

  return z;
}

static size_t descriptor_marshal_list(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  /* list-blob
   *  (4) size of list-blob : 4 + 2 + n
   *  (2) length of list
   *  (n) blob data
   */

  RUNTIME_ASSERT(0);
  return 0;
}

//
// API
//

size_t API descriptor_field_marshal(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  union {
    uint8_t u8[0];
    uint16_t u16[0];
  } *lens = object;

  if(field->code == DESCRIPTOR_INT || field->code == DESCRIPTOR_UINT) {
    return descriptor_marshal_int(dst, sz, &lens->u8[field->offset], field->size);
  }

  else if(field->code == DESCRIPTOR_STRING) {
    return descriptor_marshal_string(dst, sz, &lens->u8[field->offset], lens->u16[field->len_offset / 2]);
  }

  else if(field->code == DESCRIPTOR_LIST) {
    return descriptor_marshal_list(dst, sz, field, object);
  }

  RTA;
  return 0;
}
