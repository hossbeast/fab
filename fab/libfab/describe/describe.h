/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _FAB_DESCRIBE_H
#define _FAB_DESCRIBE_H

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"
#include "fsent.h"

extern descriptor_type descriptor_fab_describe_item;

/* result from the describe api */
typedef union fab_describe_item {
  struct {
    uint16_t distance;
    enum fab_fsent_type type;
    enum fab_fsent_state state;

    const char *label;
    uint16_t label_len;

    const char *abspath;
    uint16_t abspath_len;

    const char *ref;
    uint16_t ref_len;

    const char *variant;
    uint16_t variant_len;

    const char *ext;
    uint16_t ext_len;

    const char *suffix;
    uint16_t suffix_len;

    const char *base;
    uint16_t base_len;

    const char *fsroot;
    uint16_t fsroot_len;

    const char *invalidation;
    uint16_t invalidation_len;
  };

  const char *s[0];
  uint16_t u16[0];
} fab_describe_item;

#endif
