/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>
#include <errno.h>

#include "value.h"
#include "make.h"
#include "coerce.h"
#include "parser.h"

static int coerce_boolean(bacon_parser * restrict parser, const bacon_value * src, bacon_value ** restrict valp)
{
  if(!src) {
    bacon_boolean_mk(parser, 0, valp, false);
    return 0;
  }

  switch(src->type)
  {
    case BACON_TYPE_BOOLEAN:
      *valp = (void*)src;
      return 0;
    case BACON_TYPE_INTEGER:
      bacon_boolean_mk(parser, 0, valp, src->i != 0);
      return 0;
    case BACON_TYPE_STRING:
      bacon_boolean_mk(parser, 0, valp, src->len != 0);
      return 0;
    case BACON_TYPE_SET:
      bacon_boolean_mk(parser, 0, valp, !rbtree_empty(&src->set));
      return 0;
    case BACON_TYPE_LIST:
      bacon_boolean_mk(parser, 0, valp, llist_attached(&src->list));
      return 0;
    default:
      break;
  }

  return -EINVAL;
}

static int coerce_string(bacon_parser * restrict parser, const bacon_value * src, bacon_value ** restrict valp)
{
  if(!src) {
    bacon_string_mk(parser, 0, 0, valp);
    return 0;
  }

  switch(src->type)
  {
    case BACON_TYPE_STRING:
      *valp = (void*)src;
      return 0;
    case BACON_TYPE_INTEGER:
      if(src->i > INT64_MAX) {
        bacon_string_mks(parser, 0, 0, valp, "-int128-");
      } else {
        bacon_string_mkf(parser, 0, 0, valp, "%"PRId64, (int64_t)src->i);
      }
      return 0;
    default:
      break;
  }

  return -EINVAL;
}

static int coerce_float(bacon_parser * restrict parser, const bacon_value * src, bacon_value ** restrict valp)
{
  if(!src) {
    bacon_float_mk(parser, 0, valp, 0);
    return 0;
  }

  switch(src->type)
  {
    case BACON_TYPE_FLOAT:
      *valp = (void*)src;
      return 0;
    case BACON_TYPE_INTEGER:
      bacon_float_mk(parser, 0, valp, src->i);
      return 0;
    default:
      break;
  }

  return -EINVAL;
}

int bacon_value_coerce(bacon_parser * restrict parser, const bacon_value * src, bacon_type type, bacon_value ** restrict valp)
{
  switch(type)
  {
    case BACON_TYPE_BOOLEAN:
      return coerce_boolean(parser, src, valp);
    case BACON_TYPE_STRING:
      return coerce_string(parser, src, valp);
    case BACON_TYPE_FLOAT:
      return coerce_float(parser, src, valp);
    default:
      break;
  }

  return -EINVAL;
}
