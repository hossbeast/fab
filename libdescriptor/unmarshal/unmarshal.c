#include <string.h>


#include "descriptor.h"
#include "zbuffer.h"

//
// static
//

static void field_unmarshal_int(void *restrict object, descriptor_field *restrict field, void *restrict src, size_t sz, size_t *restrict z)
{
  (*z) += ((*z) & (field->size - 1));
  memcpy(object + field->offset, src + (*z), field->size);
  (*z) += field->size;
}

static size_t field_unmarshal_string(void *restrict object, descriptor_field *restrict field, void *restrict buf, size_t sz)
{
  union {
    uint8_t u8[0];
    uint16_t u16[0];
  } *src = buf;
  union {
    uint16_t u16[0];
    uint8_t *s[0];
  } *dst = object;

  size_t z;
  uint16_t len;

  z = 0;

  // length
  len = src->u16[z / sizeof(*src->u16)];
//printf("  got len %hu @ %p\n", len, &src->u16[z / sizeof(*src->u16)]);
  dst->u16[field->len_offset / sizeof(*dst->u16)] = len;
  z += sizeof(len);

  // data
  dst->s[field->offset / sizeof(*dst->s)] = &src->u8[z];
  z += len;

  return z;
}

static size_t field_unmarshal_string_list(void *restrict object, descriptor_field *restrict field, void *restrict buf, size_t sz)
{
  size_t z;
  uint16_t size;
  uint16_t len;
  union {
    uint8_t u8[0];
    uint16_t u16[0];
  } *src = buf;
  union {
    uint16_t u16[0];
    void *p[0];
  } *dst = object;

  z = 0;

  // list size
  z += (z & 1);
  size = src->u16[z / sizeof(*src->u16)];
//printf("  got size %hu @ %p\n", size, &src->u16[z / sizeof(*src->u16)]);
  z += sizeof(size);

  // list length - number of elements in the list
  len = src->u16[z / sizeof(*src->u16)];
//printf("  got len %hu @ %p\n", len, &src->u16[z / sizeof(*src->u16)]);
  dst->u16[field->len_offset / sizeof(*dst->u16)] = len;
//printf("  set count at %d\n", field->len_offset);
  z += sizeof(len);

  dst->p[field->offset / sizeof(*dst->p)] = &src->u8[z];
  z += size;

//  for(x = 0; x < count; x++)
//  {
//    // length
//    z += (z & 1);
//    len = src->u16[z / sizeof(*src->u16)];
////printf("  [%d] got len %hu @ %p\n", x, len, &src->u16[z / sizeof(*src->u16)]);
//    dst->u16[field->offset / sizeof(*dst->u16)] = len;
////printf("  set len at %d\n", field->lens_offset);
//    z += sizeof(len);
//
//    // data
//    dst->u8[field->offset] = &src->u8[z];
//    z += len;
//  }

  return z;
}

//
// API
//

size_t API descriptor_type_unmarshal(void *restrict object, descriptor_type *restrict desc, void *restrict src, size_t sz)
{
  uint16_t field_id;
  descriptor_field *field;
  size_t z;
  union {
    uint16_t u16[0];
  } *lens = src;

  z = 0;

  while(z < sz)
  {
    z += (z & 1);
    field_id = lens->u16[z / 2];
    RUNTIME_ASSERT(field_id <= desc->members_len);
//    if(field_id > desc->members_len) {
//printf("+0x%08zx %8p ID %u NOT FOUND\n", z, src + z, field_id);
//      return -1;
//    }
//printf("+0x%08zx %8p field %u\n", z, src + z, field_id);
    z += sizeof(field_id);

    field = desc->members[field_id - 1];
    if(field->code == 1 || field->code == 2)
    {
      field_unmarshal_int(object, field, src, sz, &z);
    }
    else if(field->code == 3)
    {
      z += field_unmarshal_string(object, field, src + z, sz - z);
    }
    else if(field->code == 4)
    {
      z += field_unmarshal_string_list(object, field, src + z, sz - z);
    }
  }

  return z;
}
