/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "narrator.h"

// structure definitions
#include "fd.internal.h"
#include "fixed.internal.h"
#include "growing.internal.h"
#include "multi.internal.h"
#include "nullity.internal.h"
#include "record.internal.h"
#include "rolling.internal.h"

#include "macros.h"

/// narrator
//
// SUMMARY
//  primary data type
//
typedef struct narrator
{
  narrator_type type;       // one of NARRATOR_*

  union {
    narrator_fixed fixed;
    narrator_growing growing;
    narrator_fd fd;
    narrator_multi multi;
    narrator_nullity nullity;
    narrator_record record;
    narrator_rolling rolling;
  };
} narrator;
