/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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

void valyria_fault_activate(valyria_fault fault, ...)
{
  va_list va;
  va_start(va, fault);
  size_t x;

  faults[fault] = true;

  if(fault == VALYRIA_KEY_INDEX_BOUNDARY)
  {
    x = fault_state.key_index_boundary.len;
    fault_state.key_index_boundary.keys[x].key = va_arg(va, void*);
    fault_state.key_index_boundary.len++;
  }
  else if(fault == VALYRIA_KEY_HASH)
  {
    x = fault_state.key_hash.len;
    fault_state.key_hash.keys[x].key = va_arg(va, void*);
    fault_state.key_hash.keys[x].hash = va_arg(va, uint32_t);
    fault_state.key_hash.len++;
  }
  else if(fault == VALYRIA_HASH_INDEX)
  {
    x = fault_state.hash_index.len;
    fault_state.hash_index.hashes[x].hash = va_arg(va, uint32_t);
    fault_state.hash_index.hashes[x].index = va_arg(va, size_t);
    fault_state.hash_index.len++;
  }

  va_end(va);
}

void valyria_faults_reset()
{
  memset(faults, 0, sizeof(faults));
  memset(&fault_state, 0, sizeof(fault_state));
}

#endif
