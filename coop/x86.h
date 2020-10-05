#ifndef _TASK_x86_H
#define _TASK_x86_H

#include <stdint.h>
#include <stdbool.h>

#include "llist.h"

/*
 * api
 */

typedef struct x86_domain domain;
typedef struct x86_fiber fiber;
typedef struct x86_task task;

/*
 * internal
 */

typedef struct x86_registers {
  uint64_t rflags;
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t rbx;
  uint64_t rbp;
  uint64_t rip;         // 8 fields
//  uint64_t rip2;      // x86_task_primeval_restore address
//  uint64_t rsp2;      // primeval stack pointer
} x86_registers;

typedef enum x86_fiber_state {
  UNINITIALIZED = 0,
  EXECUTING,      /* currently executing in a domain */
  RUNNING,        /* ready to execute - in a queue */
  JOINING,        /* waiting for clones - not in any queue */
  FINALIZED,      /* exited */
} x86_fiber_state;

struct x86_fiber;
typedef struct x86_domain {
  struct x86_fiber *executing;  // fiber which is presently executing

  /* fibers in the RUNNING state */
  llist runqueue;

  /* fibers which have recently joined */
  llist joinqueue;
  int32_t joinqueue_lock;

  /* incoming tasks */
  llist taskqueue;
  int32_t taskqueue_lock;

  /* when all queues are empty */
  int32_t waiting;
  int32_t __attribute__((aligned(4))) futex;
} x86_domain;

typedef struct x86_task {
  llist lln;
  struct x86_fiber *parent;     // fiber from which this task was cloned
  void *sp;                     // stack pointer
  uint32_t stack_size;          // allocated stack size
  char stack[0];
} x86_task;

STATIC_ASSERT(offsetof(x86_task, stack) == 36);

typedef struct x86_fiber {
  llist lln;                // runqueue / joinqueue
  void *sp;                 // stack pointer
  x86_domain *domain;
  void *stack;              // stack base - 16-byte aligned
  uint32_t stack_size;        // allocated stack size
  struct x86_fiber *parent; // (local) fiber from which this fiber was cloned
  uint16_t children;        // number of extant clones (fibers + tasks)
  x86_fiber_state state;
} x86_fiber;

STATIC_ASSERT(offsetof(x86_fiber, stack) == 32);

/*
 * Suspend the running fiber, switch to the next fiber
 */
void x86_switch(void * restrict prev_sp, void *restrict next_sp)
  __attribute__((nonnull));

/*
 * Switch into the next fiber
 */
void x86_resume(void *restrict next_sp)
  __attribute__((nonnull));

/*
 *
 */
void x86_sub_fiber_finalize_jump(void);
void x86_sub_fiber_finalize(x86_fiber *fiber)
  __attribute__((nonnull));

void x86_task_finalize_jump(void);
void x86_task_finalize(x86_task *restrict, x86_fiber *restrict)
  __attribute__((nonnull));

void x86_clone_return(void);


/*
 *
 */
void x86_base_fiber_finalize_jump(void);
void x86_base_fiber_finalize(x86_fiber * restrict fiber, void * restrict thread_sp)
  __attribute__((nonnull));

/*
 * Create a copy of the executing fiber
 *
 * domain - domain to which the executing fiber belongs
 * fiber - new fiber
 *
 * returns fiber in the parent and 0 in the child
 */
x86_fiber * x86_clone_fiber(x86_domain * restrict domain, x86_fiber * restrict fiber)
  __attribute__((nonnull));

/*
 * Jumped to from x86_clone_fiber
 */
x86_fiber * x86_clone_fiber_finish(x86_domain * restrict domain, x86_fiber * restrict fiber, void *sp)
  __attribute__((nonnull));

/*
 * Create a copy of the executing fiber
 *
 * returns task in the parent and 0 in the child
 */
x86_task * x86_clone_task(x86_domain * restrict domain, x86_task * restrict task)
  __attribute__((nonnull));

x86_task * x86_clone_task_finish(x86_domain * restrict domain, x86_task * restrict task, void *sp)
  __attribute__((nonnull));

void x86_task_enqueue(x86_domain * restrict domain, x86_task * restrict task)
  __attribute__((nonnull));

/*
 * Ingest a task into the executing fiber
 */
void x86_task_assimilate(x86_domain * restrict domain, x86_task *restrict task)
  __attribute__((nonnull));

void x86_task_assimilate_finish(x86_domain * restrict domain, x86_task *restrict task, void *sp)
  __attribute__((nonnull));

/*
 * Restore the original thread which called x86_task_base_run
 */
void x86_thread_restore(void * restrict sp)
  __attribute__((nonnull));

/*
 * Suspend the calling thread, switch to the domains base fiber
 */
void x86_base_fiber_run(x86_fiber *restrict fiber, void (*fn)(x86_domain *))
  __attribute__((nonnull));

/*
 * Suspend the calling thread, switch to the domains base fiber
 *
 * sp - base fiber stack pointer
 */
void x86_base_fiber_switch(x86_domain * restrict thread, void * restrict sp)
  __attribute__((nonnull));

#endif
