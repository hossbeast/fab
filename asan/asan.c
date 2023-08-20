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
