/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

union pattern_segment;
struct pattern_segments;

int pattern_segments_cmp(const struct pattern_segments * A, const struct pattern_segments * B);
int pattern_segment_cmp(const union pattern_segment * A, const union pattern_segment * B);
