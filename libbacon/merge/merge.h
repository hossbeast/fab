/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 value/merge

SUMMARY
 combine two value trees.

REMARKS
 there are no error cases - any two values can be merged

 when two sets are merged, mappings with the same key are coalesced, and their values are merged.
 Thus, unless you pass two mappings in to bacon_merge, mappings are never merged. When this does
 happen, their respective keys are ignored.

 if two scalars are merged, a list is created to contain them
*/

struct bacon_value;         // value.h
struct bacon_parser;  // value/parser.h

/// bacon_merge
//
// SUMMARY
//  combine two value trees from the same value_store
//
// PARAMETERS
//  dst - destination tree
//  src - source tree
//
void bacon_merge(struct bacon_parser * restrict parser, struct bacon_value ** dst, const struct bacon_value * src)
  __attribute__((nonnull(1)));
