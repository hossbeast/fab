/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "descriptor.h"
#include "stats.h"
#include "types.h"

extern descriptor_type descriptor_stats_fsent_dir;

/* BAM_FSENT_TYPE_DIR - directory node */
typedef union stats_fsent_dir {
  struct {
    /* directory nodes only */
    uint32_t rules;         /* number of rma references to this directory */

    /* event counters */
    uint32_t mkdir;         /* number of times mkdir has been called on this dir */

    /* instantaneous state */
    uint8_t indexed_lookup; /* whether this node is indexed for lookup by name */
  };

  stats_record record;
} stats_fsent_dir;
