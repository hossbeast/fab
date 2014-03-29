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

#ifndef _LISTWISE_INTERNAL_H
#define _LISTWISE_INTERNAL_H

#include "xapi.h"

// all of liblistwise
#include "listwise.h"
#include "xtra.h"
#include "operator.h"
#include "lwx.h"
#include "lstack.h"
#include "generator.h"
#include "genscan.h"
#include "ops.h"
#include "object.h"
#include "re.h"
#include "fs.h"
#include "logging.h"
#include "log.h"
#include "describe.h"

// common modules
#include "coll.h"
#include "map.h"

// error tables
#include "LW.errtab.h"
#include "PCRE.errtab.h"
#undef perrtab
#define perrtab perrtab_LW

// visibility declarations
#define API __attribute__((visibility("protected")))
#define APIDATA

#define restrict __restrict

// collection of registered object types with lookup index by type id
map * object_registry;

#undef restrict
#endif
