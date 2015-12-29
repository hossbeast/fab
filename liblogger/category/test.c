#include <stdio.h>

#include "xapi.h"

#include "logger.h"
#include "category.h"

int main()
{
  enter;

  logger_category * logs_a = (logger_category[]) {
      { name : "ERROR" }
    , { name : "WARN" }
    , { name : "INFO" }
    , {}
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "ERROR", .bits = L_GREEN }
    , { name : "INFO" }
    , { name : "WARN" }
    , { name : "WARN" }
    , { name : "FOO" }
    , { name : "BAR" }
    , { name : "BAZ" }
    , {}
  };

  fatal(logger_register, logs_a);
  fatal(logger_register_resolve);

  fatal(logger_register, logs_b);
  fatal(logger_register_resolve);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}
