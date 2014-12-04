/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _FAB_GLOBAL_H
#define _FAB_GLOBAL_H

// fab error table
#include "FAB.errtab.h"
#undef perrtab
#define perrtab perrtab_FAB

// fab common modules
#include "args.h"
#include "params.h"
#include "logs.h"
#include "error.h"
#include "identity.h"
#include "dirutil.h"

#endif
