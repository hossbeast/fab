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

#ifndef _LISTWISE_LOGGING_H
#define _LISTWISE_LOGGING_H

#include "logger.h"
#include "logger/category.h"

extern logger_category * listwise_logs;

#define L_LISTWISE  listwise_logs[0].id
#define L_PARSE     listwise_logs[1].id
#define L_EXEC      listwise_logs[2].id
#define L_OPINFO    listwise_logs[3].id
#if SANITY
#define L_SANITY    listwise_logs[6].id
#endif

#endif
