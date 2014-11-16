/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

//
// api (library users)
//
#include "listwise.h"
#include "describe.h"
#include "exec.h"
#include "fs.h"
#include "generator.h"
#include "genscan.h"
#include "iterate.h"
#include "logging.h"
#include "lstack.h"
#include "lwx.h"
#include "object.h"
#include "operator.h"
#include "operators.h"
#include "re.h"
#include "selection.h"
#include "tune.h"
#include "window.h"

//
// operator (listwise operators)
//
#include "logging.internal.h"

//
// internal
//
#include "describe.internal.h"
#include "operator.internal.h"
#include "window.internal.h"
#include "sanity.internal.h"
#include "object.internal.h"
#include "selection.internal.h"

// error tables
#include "LW.errtab.h"
#undef perrtab
#define perrtab perrtab_LW

// visibility declarations
#define API __attribute__((visibility("protected")))
#define APIDATA

#endif
