#include "xunit.h"

xunit_unit xunit_units = (xunit_unit) {
    next : &xunit_units
  , prev : &xunit_units
};

static inline void __attribute__((nonnull)) append(xunit_unit * head, xunit_unit * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;
}

void xunit_register_unit(xunit_unit * restrict unit)
{
  append(&xunit_units, unit);
}
