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

#ifndef _VALYRIA_FAULTS_INTERNAL_H
#define _VALYRIA_FAULTS_INTERNAL_H

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

#endif
