/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "faults.h"

#define FAULTS_MAX_ENTRIES 8

extern bool faults[VALYRIA_FAULT_RANGE_AFTER];
extern char *fault_names[VALYRIA_FAULT_RANGE_AFTER];
extern struct {
  struct {
    struct {
      char * key;
      size_t len;
    } keys[FAULTS_MAX_ENTRIES];
    size_t len;
  } key_index_boundary;

  struct {
    struct {
      char * key;
      size_t len;
      uint32_t hash;
    } keys[FAULTS_MAX_ENTRIES];
    size_t len;
  } key_hash;

  struct {
    struct {
      uint32_t hash;
      size_t index;
    } hashes[FAULTS_MAX_ENTRIES];
    size_t len;
  } hash_index;
} fault_state;

#if FAULT_INJECTION
# define fault(x) faults[VALYRIA_ ## x]
#else
# define fault(x) 0
#endif
