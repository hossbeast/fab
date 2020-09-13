#include <string.h>

#include "descriptor.h"
#include "zbuffer.h"

//
// static
//

static size_t field_marshal_int(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  size_t z = 0;

  // id
  z += znload_u16(dst + z, sz - z, field->id);

  // data
  z += znload_int(dst + z, sz - z, object + field->offset, field->size);

  return z;
}

static size_t field_marshal_string(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  size_t z = 0;
  uint16_t len;
  union {
    uint8_t u8[0];
    uint16_t u16[0];
  } *lens = object;

  // id
  z += znload_u16(dst + z, sz - z, field->id);

  // length
  len = lens->u16[field->len_offset / 2];
  z += znload_u16(dst + z, sz - z, len);

  // data
  z += memncpy(dst + z, sz - z, &lens[field->offset], len);

  return z;
}

static size_t field_marshal_string_list(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  RUNTIME_ASSERT(0);

  return 0;
}

//
// API
//

size_t API descriptor_field_marshal(void *restrict dst, size_t sz, descriptor_field * restrict field, void * restrict object)
{
  if(field->code == DESCRIPTOR_INT || field->code == DESCRIPTOR_UINT) {
    return field_marshal_int(dst, sz, field, object);
  }

  else if(field->code == DESCRIPTOR_STRING) {
    return field_marshal_string(dst, sz, field, object);
  }

  else if(field->code == DESCRIPTOR_STRING_LIST) {
    return field_marshal_string_list(dst, sz, field, object);
  }

  return 0;
}
