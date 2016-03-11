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
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "internal.h"
#include "log.internal.h"
#include "stream.internal.h"

#include "macros.h"
#include "strutil.h"
#include "color.h"

#define restrict __restrict

//
// [[ static ]]
//

static __thread uint64_t  storage_ids;	    // when a log is being constructed, effective bits
static __thread uint32_t  storage_attrs;
static __thread pstring * storage_message;  // the log message
static __thread long      storage_time_msec;

/// start
//
// SUMMARY
//  store the log prefix if streams_would
//
// PARAMETERS
//  ids   - log bits
//  attrs - 
//  w     - (returns) whether streams_would
//
static xapi start(const uint64_t ids, uint32_t attrs, int * const restrict w)
{
  enter;

	if(streams_would(ids))
	{
    (*w) = 1;

    storage_ids = ids;
    storage_attrs = attrs;
    fatal(psclear, &storage_message);

    // wall-clock milliseconds
    struct timespec times;
    clock_gettime(CLOCK_REALTIME, &times);
    storage_time_msec = (times.tv_sec * 1000) * (times.tv_nsec / 1000);
	}

  finally : coda;
}

static xapi finish()
{
  enter;

  // write to active streams
  fatal(streams_write, storage_ids, storage_attrs, storage_message, storage_time_msec);
	storage_ids = 0;

  finally : coda;
}

//
// api
//

API xapi logger_vlogf(const uint64_t ids, uint32_t attrs, const char * const fmt, va_list va)
{
  enter;

	if(storage_ids)
	{
		if(streams_would(storage_ids))
      fatal(psvcatf, &storage_message, fmt, va);
	}
  else
  {
    int w = 0;
    fatal(start, ids, attrs, &w);

    if(w)
    {
      fatal(psvcatf, &storage_message, fmt, va);
      fatal(finish);
    }
  }

  finally : coda;
}

API xapi logger_logf(const uint64_t ids, uint32_t attrs, const char * const fmt, ...)
{
  enter;

	va_list va;
	va_start(va, fmt);
	fatal(logger_vlogf, ids, attrs, fmt, va);

finally:
	va_end(va);
coda;
}

API xapi logger_logs(const uint64_t ids, uint32_t attrs, const char * const s)
{
  enter;

	fatal(logger_logw, ids, attrs, s, strlen(s));

  finally : coda;
}

API xapi logger_logw(const uint64_t ids, uint32_t attrs, const char * const src, size_t len)
{
  enter;

	if(storage_ids)
	{
		if(streams_would(storage_ids))
      fatal(pscatw, &storage_message, src, len);
	}
	else
	{
    int w = 0;
    fatal(start, ids, attrs, &w);

    if(w)
    {
      fatal(pscatw, &storage_message, src, len);
      fatal(finish);
    }
	}

  finally : coda;
}

API xapi logger_log_start(const uint64_t ids, uint32_t attrs)
{
  enter;

  int w;
  fatal(start, ids, attrs, &w);

  finally : coda;
}

API xapi logger_log_finish()
{
  enter;

	if(streams_would(storage_ids))
		fatal(finish);

	storage_ids = 0;

  finally : coda;
}

API int logger_log_would(const uint64_t ids)
{
  return streams_would(ids);
}

API int logger_log_bytes()
{
  if(storage_message)
    return storage_message->l;

  return 0;
}

API int logger_log_chars()
{
  if(storage_message)
    return storage_message->l;

  return 0;
}
