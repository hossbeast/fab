/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

const char *__asan_default_options(void);
const char *__asan_default_options() {
  return
      ":halt_on_error=1"
      ":abort_on_error=1"
      ":print_stacktrace=1"
      ":disable_coredump=0"       // yes, produce coredumps
      ":report_objects=1"         // print leaked addresses
      ":fast_unwind_on_malloc=0"  // prevent crash-while-reporting-leaks in certain cases
      ":strict_string_checks=1"
      ":detect_stack_use_after_return=1"
      ":check_initialization_order=1"
      ":strict_init_order=1"
    ;
};

const char *__ubsan_default_options(void);
const char *__ubsan_default_options() {
  return __asan_default_options();
};

/* this should go in a different file but this is conventient for now */
#include <backtrace.h>
struct backtrace_state *btstate;

static int bt_frame(void *data, uintptr_t pc, const char *file, int lineno, const char *function)
{
  if(!function) {
    return 0;
  }

  printf(" pc : %lx %s %s:%d\n", pc, function, file, lineno);
  return 0;
}

static void bt_error(void *data, const char *msg, int errnum)
{
  printf("ERR %s %d\n", msg, errnum);
}

void backtracef_setup(void)
{
  btstate = backtrace_create_state(__FILE__, 1, 0, 0);
}

void backtracef(void)
{
  printf("[%5d/%-5ld] BACKTRACE\n", getpid(), syscall(SYS_gettid));
  backtrace_full(btstate, 1, bt_frame, bt_error, 0);
}
