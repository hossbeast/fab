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

#ifndef _XAPI_INTERNAL_H
#define _XAPI_INTERNAL_H

//
// api
//
#include "xapi.h"
#include "unwind.h"
#include "trace.h"
#include "callstack.h"
#include "error/error.h"
#include "error/SYS.errtab.h"
#include "xapi/XAPI.errtab.h"

//
// internal
//
#include "trace/trace.internal.h"
#include "callstack/callstack.internal.h"
#include "error/error.internal.h"

#undef perrtab
#define perrtab perrtab_SYS

#define API __attribute__((visibility("protected")))
#define APIDATA

#endif
