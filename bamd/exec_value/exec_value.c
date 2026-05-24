/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xlinux/xstdlib.h"
#include "bacon/registry.h"
#include "bacon/location.h"
#include "bacon/builder.h"

#include "exec_value.h"
#include "selector.h"
#include "build_slot.h"
#include "module.h"
#include "selection.h"
#include "pattern.h"
#include "zcat.h"
#include "resolve.h"
#include "namespace.h"

#include "common/attrs.h"

attrs32 exec_value_attrs = (attrs32){
#undef DEF
#define DEF(x, s, r, y) + 1
		num : 0
			EXEC_VALUE_TABLE
	, members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : UINT32_C(y), range : UINT32_C(r) },
EXEC_VALUE_TABLE
	}
};

static void __attribute__((constructor)) init()
{
	attrs32_init(&exec_value_attrs);
}

static exec_value *allocate(exec_value_type type)
{
	exec_value *v;

	xmalloc(&v, sizeof(*v));
	bacon_value_init(&v->bacon);

	v->bacon.type = type;

	return v;
}

static void __attribute__((nonnull)) exec_value_destroy(struct bacon_parser * restrict parser, struct bacon_value * restrict v)
{
  bacon_value *sv;
  exec_value *ev;

  ev = containerof(v, typeof(*ev), bacon);
	switch((int)v->type) {
		case EXEC_VALUE_SELECT:
			selector_free(ev->select);
			break;
		case EXEC_VALUE_PROPERTY:
			break;
		case EXEC_VALUE_PREPEND:
      bacon_value_free(parser, ev->prepend);
			break;
		case EXEC_VALUE_SEQUENCE:
			while((sv = llist_pop(&ev->sequence, typeof(*sv), lln))) {
				bacon_value_free(parser, sv);
			}
			break;
		case EXEC_VALUE_LOOKUP:
      pattern_free(&ev->lookup.pattern->pattern);
			break;
    RTA;
	}
}

static void __attribute__((nonnull)) exec_value_reclaim(struct bacon_value * restrict v)
{
  free(v);
}

static int exec_value_cmp(const struct bacon_value * restrict A, const struct bacon_value * restrict B)
{
	switch((int)A->type) {
		case EXEC_VALUE_SELECT:
			break;
		case EXEC_VALUE_PROPERTY:
			break;
		case EXEC_VALUE_PREPEND:
			break;
		case EXEC_VALUE_SEQUENCE:
			break;
		case EXEC_VALUE_LOOKUP:
			break;
	}

	RTA;
}

static void __attribute__((nonnull)) exec_value_write(
    struct bacon_writer * restrict builder_writer
  , const struct bacon_value * restrict v
)
{
  bacon_value *sv;
  exec_value *ev;

  ev = containerof(v, typeof(*ev), bacon);
	switch((int)ev->bacon.type) {
		case EXEC_VALUE_SELECT:
			{ MAPPING
				STRING("select");
				{ LIST
					selector_describe(ev->select, builder_writer);
				}
			}
			break;
		case EXEC_VALUE_PROPERTY:
      { MAPPING STRING("property")
				STRING(attrs32_name_byvalue(fsent_property_attrs, FSENT_PROPERTY_OPT & ev->property));
      }
			break;
		case EXEC_VALUE_PREPEND:
			{ MAPPING STRING("prepend");
				bacon_writer_value(builder_writer, ev->prepend);
			}
			break;
		case EXEC_VALUE_SEQUENCE:
      { SET_IF_MAPPING
        { MAPPING STRING("sequence")
          { LIST
            llist_foreach(&ev->sequence, sv, lln) {
              bacon_writer_value(builder_writer, sv);
            }
          }
        }
      }
			break;
		case EXEC_VALUE_LOOKUP:
      { SET_IF_MAPPING
        { MAPPING STRING("lookup")
          { SET
            { MAPPING STRING("pattern")
              bacon_writer_push_bytes(builder_writer);
              pattern_say(&ev->lookup.pattern->pattern, builder_writer->N);
              bacon_writer_pop_bytes(builder_writer);
            }
            if(ev->lookup.namespace_len) {
              MAPPING STRING("namespace")
              BYTES(ev->lookup.namespace, ev->lookup.namespace_len)
            }
          }
        }
      }
			break;
    RTA;
	}
}

//
//
//

bacon_value *exec_value_property_mk(
		bacon_parser * restrict parser
	, const yyu_location * restrict loc
	, bacon_value ** rv
	, fsent_property property
)
{
	exec_value *v;

	v = allocate(EXEC_VALUE_PROPERTY);
	bacon_location_clone(&v->bacon.loc, loc, parser);
	v->property = property;

  *rv = &v->bacon;
	return &v->bacon;
}

bacon_value *exec_value_select_mk(
		bacon_parser * restrict parser
	, const yyu_location * restrict loc
	, bacon_value ** rv
	, selector * restrict select
)
{
	exec_value *v;

	v = allocate(EXEC_VALUE_SELECT);
	bacon_location_clone(&v->bacon.loc, loc, parser);
	v->select = select;

  *rv = &v->bacon;
	return &v->bacon;
}

bacon_value * exec_value_prepend_mk(
		bacon_parser * restrict parser
	, const yyu_location * restrict loc
	, bacon_value ** rv
	, bacon_value * restrict val
)
{
	exec_value *v;

	v = allocate(EXEC_VALUE_PREPEND);
	bacon_location_clone(&v->bacon.loc, loc, parser);
	v->prepend = val;

  *rv = &v->bacon;
	return &v->bacon;
}

bacon_value *exec_value_sequence_mkv(
		bacon_parser * restrict parser
	, const yyu_location * restrict loc
	, bacon_value * e
	, bacon_value ** rv
	, bacon_value * el
)
{
  exec_value *v;

  if(e) {
    v = containerof(e, typeof(*v), bacon);
  } else {
		v = allocate(EXEC_VALUE_SEQUENCE);
    e = &v->bacon;
		llist_init(&v->sequence);
	}
	bacon_location_clone(&v->bacon.loc, loc, parser);
	if(el) {
		llist_push(&v->sequence, &el->lln);
	}

  *rv = &v->bacon;
  return &v->bacon;
}

bacon_value *exec_value_lookup_mk(
		bacon_parser * restrict parser
	, const yyu_location * restrict loc
	, bacon_value ** rv
	, struct lookup_pattern * restrict pattern
	, const char * restrict namespace
	, uint16_t namespace_len
)
{
	exec_value *v;

	v = allocate(EXEC_VALUE_LOOKUP);
	bacon_location_clone(&v->bacon.loc, loc, parser);
	v->lookup.pattern = pattern;
	v->lookup.namespace_len = 0;
	if(namespace_len) {
		v->lookup.namespace_len = zcatw(v->lookup.namespace, sizeof(v->lookup.namespace), namespace, namespace_len);
	}

  *rv = &v->bacon;
	return &v->bacon;
}

static bacon_object bacon_object_select = {
		.type = EXEC_VALUE_SELECT
	, .destroy = exec_value_destroy
	, .reclaim = exec_value_reclaim
	, .cmp = exec_value_cmp
	, .write = exec_value_write
};

static bacon_object bacon_object_property = {
		.type = EXEC_VALUE_PROPERTY
	, .destroy = exec_value_destroy
	, .reclaim = exec_value_reclaim
	, .cmp = exec_value_cmp
	, .write = exec_value_write
};

static bacon_object bacon_object_prepend = {
		.type = EXEC_VALUE_PREPEND
	, .destroy = exec_value_destroy
	, .reclaim = exec_value_reclaim
	, .cmp = exec_value_cmp
	, .write = exec_value_write
};

static bacon_object bacon_object_sequence = {
		.type = EXEC_VALUE_SEQUENCE
	, .destroy = exec_value_destroy
	, .reclaim = exec_value_reclaim
	, .cmp = exec_value_cmp
	, .write = exec_value_write
};

static bacon_object bacon_object_lookup = {
		.type = EXEC_VALUE_LOOKUP
	, .destroy = exec_value_destroy
	, .reclaim = exec_value_reclaim
	, .cmp = exec_value_cmp
	, .write = exec_value_write
};

void exec_value_setup(void)
{
	bacon_registry_register(&bacon_object_select);
	bacon_registry_register(&bacon_object_property);
	bacon_registry_register(&bacon_object_prepend);
	bacon_registry_register(&bacon_object_sequence);
	bacon_registry_register(&bacon_object_lookup);
}

void exec_value_cleanup(void)
{
}
