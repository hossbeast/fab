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

#include "xapi.h"

#include "logger.h"
#include "logger/category.h"

#include "internal.h"
#include "logging.internal.h"

logger_category * categories = (logger_category []) {
    { name : "FAB"      , description : "libfab logging" }
#if DEBUG || DEVEL || XUNIT
  , { name : "IPC"      , description : "signal-exchange" }
#endif
  , { }
};

xapi logging_setup()
{
  enter;

  fatal(logger_category_register, categories);

  finally : coda;
}
