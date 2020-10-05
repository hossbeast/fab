#ifndef _TASK_H
#define _TASK_H

/*
 * enqueue the active task onto the runqueue, switch to the top task on the runqueue
 */
void task_yield(domain *restrict)
  __attribute__((nonnull));

/*
 *
 */
void task_join(domain *restrict)
  __attribute__((nonnull));

/*
 * Run tasks in a domain
 *
 * domain -
 * task   -
 * fn     -
 */
void task_run(domain * restrict, task * restrict, void (* fn)(domain *))
  __attribute__((nonnull));

#endif
