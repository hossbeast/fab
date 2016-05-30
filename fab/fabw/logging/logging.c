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

#include "logger/stream.h"

#include "logging.h"

logger_category * categories = (logger_category []) {
	  { name : "ERROR"	, description : "fatal errors" }
	, { name : "IPC"	  , description : "signal-exchange" }
  , { }
};

logger_stream * streams = (logger_stream []) {
    { name : "console"  , type : LOGGER_STREAM_FD , fd : 1  , expr : "+ERROR" }
  , { }
};

xapi logging_setup()
{
  enter;

  fatal(logger_category_register, categories);
  fatal(logger_stream_register, streams);

  finally : coda;
}
