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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "types.h"

#include "faults.internal.h"

#if FAULT_INJECTION
bool faults[VALYRIA_FAULT_RANGE_AFTER];
char *fault_names[VALYRIA_FAULT_RANGE_AFTER] = {
#define VALYRIA_FAULT(x) [VALYRIA_ ## x] = "VALYRIA_" # x,
VALYRIA_FAULT_TABLE
};
typeof(fault_state) fault_state;

//
// api
//

void API valyria_fault_activate(valyria_fault fault, ...)
{
  va_list va;
  va_start(va, fault);

  faults[fault] = true;

  if(fault == VALYRIA_KEY_INDEX_BOUNDARY)
  {
    size_t x = fault_state.key_index_boundary.len;
    fault_state.key_index_boundary.keys[x].key = va_arg(va, void*);
    fault_state.key_index_boundary.keys[x].len = va_arg(va, size_t);
    fault_state.key_index_boundary.len++;
  }
  else if(fault == VALYRIA_KEY_HASH)
  {
    size_t x = fault_state.key_hash.len;
    fault_state.key_hash.keys[x].key = va_arg(va, void*);
    fault_state.key_hash.keys[x].len = va_arg(va, size_t);
    fault_state.key_hash.keys[x].hash = va_arg(va, uint32_t);
    fault_state.key_hash.len++;
  }
  else if(fault == VALYRIA_HASH_INDEX)
  {
    size_t x = fault_state.hash_index.len;
    fault_state.hash_index.hashes[x].hash = va_arg(va, uint32_t);
    fault_state.hash_index.hashes[x].index = va_arg(va, size_t);
    fault_state.hash_index.len++;
  }

  va_end(va);
}

void API valyria_faults_reset()
{
  memset(faults, 0, sizeof(faults));
  memset(&fault_state, 0, sizeof(fault_state));
}

#endif
