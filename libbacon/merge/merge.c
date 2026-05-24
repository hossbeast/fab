/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "merge.h"
#include "clone.h"
#include "make.h"
#include "parser.h"
#include "set.h"



void bacon_merge(bacon_parser * restrict parser, bacon_value ** restrict dstp, const bacon_value * restrict src)
{
  bacon_value *v;
  bacon_value *dst;
  bacon_value *c;
  bacon_value *src_v;
  rbtree_search_context ctx;
  rbnode *rbn;

  dst = *dstp;

  if(!src) {
  } else if(!dst) {
    bacon_clone(parser, dstp, src);
  }

  /* scalar -> set promotion */
  else if(dst->type == BACON_TYPE_SET && ((src->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR))
  {
    bacon_clone(parser, &c, src);
    bacon_set_mkv(parser, 0, *dstp, 0, c);
  }
  /* set -> scalar */
  else if((dst->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR && src->type == BACON_TYPE_SET)
  {
    bacon_clone(parser, dstp, src);
    bacon_set_mkv(parser, 0, *dstp, 0, dst);
  }
  /* scalar -> list promotion */
  else if(dst->type == BACON_TYPE_LIST && ((src->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR))
  {
    bacon_clone(parser, &c, src);
    bacon_list_mkv(parser, 0, *dstp, 0, c);
  }
  /* list -> scalar */
  else if((dst->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR && src->type == BACON_TYPE_LIST)
  {
    bacon_clone(parser, dstp, src);
    bacon_list_mkv(parser, 0, *dstp, 0, dst);
  }
  /* scalar -> scalar : list vivification */
  else if((dst->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR && ((src->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR))
  {
    bacon_clone(parser, &c, src);
    bacon_list_mkv(parser, 0, 0, dstp, c);
    bacon_list_mkv(parser, 0, *dstp, 0, dst);
  }
  /* scalar -> mapping promotion */
  else if(dst->type == BACON_TYPE_MAPPING && ((src->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR))
  {
    bacon_merge(parser, &dst->val, src);
  }
  /* mapping -> scalar */
  else if((dst->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR && src->type == BACON_TYPE_MAPPING)
  {
    bacon_clone(parser, dstp, src);
    bacon_merge(parser, &(*dstp)->val, dst);
  }
  /* mapping -> mapping */
  else if(dst->type == BACON_TYPE_MAPPING && src->type == BACON_TYPE_MAPPING)
  {
    bacon_merge(parser, &dst->val, src->val);
  }
  /* list -> mapping */
  else if(dst->type == BACON_TYPE_MAPPING && src->type == BACON_TYPE_LIST)
  {
    bacon_clone(parser, dstp, src);
    bacon_list_mkv(parser, 0, *dstp, 0, dst);
  }
  /* set -> mapping */
  else if(dst->type == BACON_TYPE_MAPPING && src->type == BACON_TYPE_SET)
  {
    bacon_clone(parser, dstp, src);
    bacon_set_mkv(parser, 0, *dstp, 0, dst);
  }
  /* list -> list */
  else if(dst->type == BACON_TYPE_LIST && src->type == BACON_TYPE_LIST)
  {
    llist_foreach(&src->list, v, lln) {
      bacon_clone(parser, &c, v);
      bacon_list_mkv(parser, 0, *dstp, 0, c);
    }
  }
  /* set -> list */
  else if(dst->type == BACON_TYPE_LIST && src->type == BACON_TYPE_SET)
  {
    rbtree_foreach(&src->set, v, rbn) {
      bacon_clone(parser, &c, v);
      bacon_list_mkv(parser, 0, *dstp, 0, c);
    }
  }
  /* mapping -> list */
  else if(dst->type == BACON_TYPE_LIST && src->type == BACON_TYPE_MAPPING)
  {
    bacon_clone(parser, &c, src);
    bacon_list_mkv(parser, 0, *dstp, 0, c);
  }
  /* set -> set */
  else if(dst->type == BACON_TYPE_SET && src->type == BACON_TYPE_SET)
  {
    rbtree_foreach(&src->set, src_v, rbn) {
      if((rbn = rbtree_search(&dst->set, &ctx, src_v, bacon_set_key_cmp))) {
        v = containerof(rbn, typeof(*v), rbn);
        if((v->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR) {
          /* two identical scalars ; no-op */
        } else {
          bacon_merge(parser, &v, src_v);
        }
      } else {
        bacon_clone(parser, &c, src_v);
        bacon_set_mkv(parser, 0, dst, 0, c);
      }
    }
  }
  /* list -> set */
  else if(dst->type == BACON_TYPE_SET && src->type == BACON_TYPE_LIST)
  {
    llist_foreach(&src->list, v, lln) {
      bacon_clone(parser, &c, v);
      bacon_set_mkv(parser, 0, *dstp, 0, c);
    }
  }
  /* mapping -> set */
  else if(dst->type == BACON_TYPE_SET && src->type == BACON_TYPE_MAPPING)
  {
    /* mapping-equality is key only */
    if((rbn = rbtree_search(&dst->set, &ctx, (void*)src, bacon_set_key_cmp))) {
      v = containerof(rbn, typeof(*v), rbn);
      bacon_merge(parser, &v, src);
    } else {
      bacon_clone(parser, &v, src);
      rbtree_insert(&dst->set, &ctx, &v->rbn);
    }
  }
  else
  {
    RTA;
  }
}
