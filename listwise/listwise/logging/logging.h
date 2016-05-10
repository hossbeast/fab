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

#include "logger.h"
#include "logger/category.h"

struct logger_stream;

extern logger_category * categories;
extern struct logger_stream * streams;

#define L_ERROR		categories[0].id
#define L_ARGS		categories[1].id
#define L_PARAMS	categories[2].id
#define L_LOGGER	categories[3].id

xapi logs_setup();

#endif
