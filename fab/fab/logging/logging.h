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

#ifndef _LOGGING_H
#define _LOGGING_H

#include "xapi.h"

#include "logger.h"
#include "logger/category.h"

extern struct logger_category * categories;

#define L_ERROR   categories[0x00].id
#define L_INFO    categories[0x01].id
#define L_ARGS    categories[0x02].id
#define L_PARAMS  categories[0x03].id
#if DEBUG || DEVEL
#define L_IPC     categories[0x04].id
#endif

/// logging_setup
//
// SUMMARY
//  register logging categories
//
xapi logging_setup(void);

#endif
