/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "descriptor.h"
#include "marshal.h"

//
// static
//

size_t descriptor_marshal_int(void * restrict dst, size_t sz, const void * restrict xp, uint8_t xsz)
{
  size_t z;

  z = 0;
  z += marshal_int(dst + z, sz - z, xp, xsz);

  return z;
}

size_t descriptor_marshal_string(void *restrict dst, size_t sz, const void * restrict s, uint16_t slen)
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

static size_t descriptor_marshal_list(void *restrict dst, size_t sz, const descriptor_field * restrict field, const void * restrict object)
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

size_t descriptor_field_marshal(void * restrict dst, size_t sz, const descriptor_field * restrict field, const void * restrict object)
{
  const union {
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

size_t descriptor_marshal_object(
    void * restrict dst
  , size_t sz
  , const descriptor_type * restrict desc
  , const void * restrict object
  , descriptor_marshal_handlers *handlers
)
{
  size_t x;
  size_t z;
  size_t r;
  const descriptor_field *field;

  z = 0;
  z += marshal_u32(dst + z, sz - z, desc->id);

  for(x = 0; x < desc->members_len; x++)
  {
    field = desc->members[x];

    if(handlers) {
      r = handlers->handler(dst + z, sz - z, field, object, handlers);
    } else {
      r = 0;
    }
    if(r == 0) {
      /* default handling */
      r = descriptor_field_marshal(dst + z, sz - z, field, object);
    }
    z += r;
  }

  return z;
}
