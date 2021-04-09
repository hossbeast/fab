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

#define L_CLIENT    categories[0].id
#define L_PROTOCOL  categories[1].id
#define L_CONFIG    categories[2].id
#if DEBUG || DEVEL || XAPI
# define L_IPC      categories[3].id
#endif

/// logging_setup
//
// SUMMARY
//  register logging categories
//
xapi logging_setup(char ** envp);

#endif
