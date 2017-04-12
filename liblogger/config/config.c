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

#include "internal.h"
#include "config.internal.h"

#include "zbuffer.h"

APIDATA int g_logger_default_stderr;

char logger_process_name[32];
__thread char logger_thread_name[32];

uint64_t logger_process_categories;
__thread uint64_t logger_thread_categories;

uint32_t logger_process_attrs;
__thread uint32_t logger_thread_attrs;

#define restrict __restrict

API void logger_set_process_name(const char * const restrict name)
{
  znloads(logger_process_name, sizeof(logger_process_name), name);
}

API void logger_set_thread_name(const char * const restrict name)
{
  znloads(logger_thread_name, sizeof(logger_thread_name), name);
}

API void logger_set_process_categories(uint64_t ids)
{
  logger_process_categories = ids;
}

API void logger_set_thread_categories(uint64_t ids)
{
  logger_thread_categories = ids;
}

API void logger_set_process_attrs(uint32_t attrs)
{
  logger_process_attrs = attrs;
}

API void logger_set_thread_attrs(uint32_t attrs)
{
  logger_thread_attrs = attrs;
}
