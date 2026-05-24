/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xpthread.h"

void xpthread_attr_init(pthread_attr_t * attr)
{
  RUNTIME_ASSERT(pthread_attr_init(attr) == 0);
}

void xpthread_attr_destroy(pthread_attr_t * attr)
{
  RUNTIME_ASSERT(pthread_attr_destroy(attr) == 0);
}

void xpthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
{
  RUNTIME_ASSERT(pthread_create(thread, attr, start_routine, arg) == 0);
}

void xpthread_sigmask(int how, const sigset_t * set, sigset_t * oldset)
{
  RUNTIME_ASSERT(pthread_sigmask(how, set, oldset) == 0);
}

void xpthread_attr_setdetachstate(pthread_attr_t * attr, int detachstate)
{
  RUNTIME_ASSERT(pthread_attr_setdetachstate(attr, detachstate) == 0);
}
