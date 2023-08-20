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

 only the merge attributes in the src tree are relevant, and merge attributes are not
 propagated to the destination by the merge operation

 when two sets are merged, mappings with the same key are coalesced, and their values are merged.
 Thus, unless you pass two mappings in to value_merge, mappings are never merged. When this does
 happen, their respective keys are ignored.

 if two scalars are merged, a list is created to contain them
*/

struct value;         // value.h
struct value_parser;  // value/parser.h

#define MERGE_OPT   UINT16_C(0x000F)

// get the name of a modifier or option from its value
#define VALUE_ATTR_DEF(a, b, x, m) ((x) & (m)) == (b) ? "VALUE_" #a :
#define MERGE_VALUE(x) VALUE_ATTR_TABLE(x, MERGE_OPT) "NONE"

/// value_merge
//
// SUMMARY
//  combine two value trees from the same value_store
//
// PARAMETERS
//  dst - destination tree
//  src - source tree
//  attrs - VALUE_MERGE_*
//
void value_merge(struct value_parser * restrict parser, struct value ** dst, const struct value * src, uint16_t attrs)
  __attribute__((nonnull(1)));
