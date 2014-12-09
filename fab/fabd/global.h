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

#ifndef _FABD_GLOBAL_H
#define _FABD_GLOBAL_H

#include "FAB.errtab.h"
#undef perrtab
#define perrtab perrtab_FAB

#include "error.h"
#include "logs.h"
#include "args.h"
#include "params.h"
#include "identity.h"

#include "ff.h"
#include "ff.parse.h"
#include "bp.h"
#include "traverse.h"
#include "dirutil.h"
#include "enclose.h"
#include "gn.h"
#include "gnlw.h"
#include "lwutil.h"
#include "tmp.h"
#include "var.h"
#include "list.h"
#include "ffproc.h"
#include "selector.h"
#include "dsc.h"
#include "bake.h"

#include "logger.h"

#endif
