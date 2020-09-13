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

#ifndef FABD_BEHOLDER_THREAD_H
#define FABD_BEHOLDER_THREAD_H

/*

*/

#include "types.h"
#include "xapi.h"

extern int beholder_stdout_rd;
extern int beholder_stderr_rd;

xapi beholder_thread_setup(void);

xapi beholder_thread_cleanup(void);

xapi beholder_thread_launch(void);

#endif
