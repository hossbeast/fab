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

#ifndef _VALYRIA_FAULTS_H
#define _VALYRIA_FAULTS_H

/*

SUMMARY
 runtime fault injection

*/

#include "types.h"

#define VALYRIA_FAULT_TABLE          \
  VALYRIA_FAULT(ALL_INDEX_BOUNDARY)  \
  VALYRIA_FAULT(KEY_INDEX_BOUNDARY)  \
  VALYRIA_FAULT(KEY_HASH)            \
  VALYRIA_FAULT(HASH_INDEX)          \

typedef enum {
  VALYRIA_FAULT_RANGE_BEFORE = 0,

#define VALYRIA_FAULT(x) VALYRIA_ ## x,
VALYRIA_FAULT_TABLE
#undef VALYRIA_FAULT

  VALYRIA_FAULT_RANGE_AFTER
} valyria_fault;

void valyria_fault_activate(valyria_fault fault, ...);
void valyria_faults_reset(void);

#endif
