/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "locks.h"
#include "threads.h"
#include "locks.h"

//extern struct spinlock trace_lock;

#include <stdio.h>
#include <unistd.h>
#define tracef(fmt, ...) do {               \
    /*spinlock_acquire(&trace_lock); */          \
    printf("[%5d/%-5ld]%50s:%-5d "fmt"\n"   \
      , getpid()                            \
      , syscall(SYS_gettid)                 \
      , __FUNCTION__                        \
      , __LINE__                            \
      , ##__VA_ARGS__);                     \
    /*spinlock_release(&trace_lock); */          \
  } while(0)
