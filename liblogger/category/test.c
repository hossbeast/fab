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
      { name : "FOO" }
    , { name : "QUX" }
    , { name : "INFO" }
    , { name : "ERROR" }
    , {}
  };

  logger_category * logs_b = (logger_category[]) {
      { name : "A" }
    , { name : "B" }
    , { name : "FOO", .attr = L_RED }
    , { name : "BAR" }
    , { name : "A" }
    , { name : "INFO" }
    , {}
  };

  logger_category * logs_c = (logger_category[]) {
      { name : "FOO" }
    , { name : "BAR" }
    , { name : "WARN" }
    , { name : "BAZ" }
    , { name : "INFO" }
    , { name : "ERROR", .attr = L_GREEN }
    , {}
  };

  logger_category * logs_d = (logger_category[]) {
      { name : "QUX" }
    , { name : "ERROR" }
    , {}
  };

  fatal(category_setup);

  fatal(logger_category_register, logs_a, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_b, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_c, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_register, logs_d, __FILE__ " : " XQUOTE(__LINE__));
  fatal(logger_category_resolve);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}
