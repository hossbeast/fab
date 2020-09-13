/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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

#include "asan.h"

const char *__asan_default_options() {
  return
      ":halt_on_error=1"
      ":abort_on_error=1"
      ":print_stacktrace=1"
      ":report_objects=1"         // print leaked addresses
      ":fast_unwind_on_malloc=0"  // prevent crash-while-reporting-leaks in certain cases
      ":detect_odr_violation=0"   // remove after changing xunit to not use dlopen
    ;
};

const char *__ubsan_default_options() {
  return
      ":halt_on_error=1"
      ":abort_on_error=1"
      ":print_stacktrace=1"
    ;
};
