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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"
#include "tune.internal.h"

APIDATA float xapi_dangle_factor  = XAPI_DANGLE_FACTOR_DEFAULT;
APIDATA float xapi_stacks_factor  = XAPI_STACKS_FACTOR_DEFAULT;
APIDATA float xapi_infos_factor   = XAPI_INFOS_FACTOR_DEFAULT;
APIDATA float xapi_strings_factor = XAPI_STRINGS_FACTOR_DEFAULT;
