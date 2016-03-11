#include <stdio.h>

#include "xapi.h"
#include "logger.h"
#include "logger/category.h"
#include "logger/stream.h"

logger_category * logs = (logger_category []) {
    { name : "FOO", description : "logger configuration changes", .attr = L_CATEGORY }
  , { }
};

#define L_FOO logs[0].id

logger_stream * streams = (logger_stream []) {
    { name : "foo", type : LOGGER_STREAM_FD, .fd = 1 }
  , { }
};

xapi main(int argc, char ** argv, char ** envp)
{
  enter;

  fatal(logger_setup);
  fatal(logger_category_register, logs, 0);
  fatal(logger_category_activate);
  fatal(logger_stream_register, streams);
  fatal(logger_initialize, envp);

  logs(L_FOO, "foo");

finally:
  logger_teardown();

  if(XAPI_UNWINDING)
    xapi_backtrace();
coda;
}
