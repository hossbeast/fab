#include <stdio.h>

#include "xapi.h"

#include "logger.h"
#include "category.h"

#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

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
      { name : "ERROR", .attr = L_GREEN }
    , { name : "FOO" }
    , { name : "BAR" }
    , { name : "WARN" }
    , { name : "INFO" }
    , { name : "BAZ" }
    , {}
  };

  logger_category * logs_c = (logger_category[]) {
      { name : "QUX" }
    , { name : "ERROR" }
    , {}
  };

  fatal(logger_category_setup);

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_b, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_c, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_resolve);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}
