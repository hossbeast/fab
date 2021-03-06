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

#ifndef MORIA_LOGGING_INTERNAL_H
#define MORIA_LOGGING_INTERNAL_H

#include "logger.h"
#include "logger/category.h"
#include "logging.h"

extern logger_category * categories;

#define L_MORIA       categories[0].id
#define L_GRAPH       categories[1].id
#define L_OPERATIONS  categories[2].id

/// logging_setup
//
// SUMMARY
//  configure logging
//
xapi logging_setup(void);

#endif
