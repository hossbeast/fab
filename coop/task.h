#ifndef _TASK_H
#define _TASK_H

/*
 * enqueue the active task onto the runqueue, switch to the top task on the runqueue
 */
void cmt_yield(domain *restrict)
  __attribute__((nonnull));

/*
 *
 */
void cmt_join(domain *restrict)
  __attribute__((nonnull));

/*
 * Run tasks cooperatively
 *
 * domain -
 * task   -
 * fn     -
 */
void cmt_run(domain * restrict, fiber * restrict, void (* fn)(domain *))
  __attribute__((nonnull));

#endif
