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

#include "logs.h"

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R;
  fatal(logger_setup);
  fatal(logger_category_register, logs, 0);
  fatal(logger_stream_register, streams);
  fatal(logger_initialize, envp);
  
  logs(L_FOO, "hello world");

  xlogs(L_BAR, L_RED, "red");
  xlogs(L_BAR, L_GREEN, "green");
  xlogs(L_BAR, L_YELLOW, "yellow");
  xlogs(L_BAR, L_BLUE, "blue");
  xlogs(L_BAR, L_MAGENTA, "magenta");
  xlogs(L_BAR, L_CYAN, "cyan");
  xlogs(L_BAR, L_WHITE, "white");

  xlogs(L_BAR, 0, "regular");

  xlogs(L_BAR, L_BOLD_RED, "red");
  xlogs(L_BAR, L_BOLD_GREEN, "green");
  xlogs(L_BAR, L_BOLD_YELLOW, "yellow");
  xlogs(L_BAR, L_BOLD_BLUE, "blue");
  xlogs(L_BAR, L_BOLD_MAGENTA, "magenta");
  xlogs(L_BAR, L_BOLD_CYAN, "cyan");
  xlogs(L_BAR, L_BOLD_WHITE, "white");

finally:
  logger_teardown();
conclude(&R);

  xapi_teardown();
  return !!R;
}
