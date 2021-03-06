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

#include <pthread.h>

#include "xpthread.h"
#include "KERNEL.errtab.h"

xapi API xpthread_attr_init(pthread_attr_t * attr)
{
  enter;

  int rv;
  if((rv = pthread_attr_init(attr)) != 0)
    tfail(perrtab_KERNEL, rv);

  finally : coda;
}

xapi API xpthread_attr_destroy(pthread_attr_t * attr)
{
  enter;

  int rv;
  if((rv = pthread_attr_destroy(attr)) != 0)
    tfail(perrtab_KERNEL, rv);

  finally : coda;
}

xapi API xpthread_create(pthread_t * thread, const pthread_attr_t * attr, void *(*start_routine)(void *), void * arg)
{
  enter;

  int rv;
  if((rv = pthread_create(thread, attr, start_routine, arg)) != 0)
    tfail(perrtab_KERNEL, rv);

  finally : coda;
}

xapi API xpthread_sigmask(int how, const sigset_t * set, sigset_t * oldset)
{
  enter;

  int rv;
  if((rv = pthread_sigmask(how, set, oldset)) != 0)
    tfail(perrtab_KERNEL, rv);

  finally : coda;
}

xapi API xpthread_attr_setdetachstate(pthread_attr_t * attr, int detachstate)
{
  enter;

  int rv;
  if((rv = pthread_attr_setdetachstate(attr, detachstate)) != 0)
    tfail(perrtab_KERNEL, rv);

  finally : coda;
}
