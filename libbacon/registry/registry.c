/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "registry.internal.h"


static rbtree registry;

static int registry_key_cmp(void * restrict key, const rbnode * restrict rbn)
{
  const uint32_t *type = key;
  const bacon_object *obj = containerof(rbn, typeof(*obj), rbn);

  return INTCMP(*type, obj->type);
}

static int registry_rbn_cmp(const rbnode * restrict A, const rbnode * restrict B)
{
  const bacon_object *a = containerof(A, typeof(*a), rbn);
  const bacon_object *b = containerof(B, typeof(*b), rbn);

  return INTCMP(a->type, b->type);
}

void registry_setup(void)
{
  rbtree_init(&registry);
}

void registry_cleanup(void)
{

}

void bacon_registry_register(struct bacon_object * restrict object)
{
  rbtree_put(&registry, &object->rbn, registry_rbn_cmp);
}

const struct bacon_object *registry_lookup(uint32_t type)
{
  rbnode *rbn;

  if((rbn = rbtree_lookup(&registry, &type, registry_key_cmp)) == 0) {
    return NULL;
  }

  return containerof(rbn, bacon_object, rbn);
}

const struct bacon_object *registry_get(uint32_t type)
{
  const bacon_object * obj;
  if((obj = registry_lookup(type))) {
    return obj;
  }

  return NULL;
}
