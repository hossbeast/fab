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

#ifndef _FABCORE_LOGGING_H
#define _FABCORE_LOGGING_H

#include "logger.h"
#include "logger/category.h"

extern struct logger_category * fabcore_categories;

#define L_FABCORE fabcore_categories[0x00].id
#define L_ERROR   fabcore_categories[0x01].id
#define L_INFO    fabcore_categories[0x02].id
#define L_PARAMS  fabcore_categories[0x03].id
#define L_TIMING  fabcore_categories[0x04].id
#define L_USAGE   fabcore_categories[0x05].id
#define L_CONFIG  fabcore_categories[0x06].id

#if DEBUG || DEVEL
#define L_IPC     fabcore_categories[0x07].id
#define L_TOKENS  fabcore_categories[0x08].id
#define L_STATES  fabcore_categories[0x09].id
#endif

#endif
