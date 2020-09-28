#ifndef _TASK_x86_H
#define _TASK_x86_H

#include <stdint.h>

/*
 * api
 */

typedef struct x86_task task;

/* executing task */
extern task *task_active;
extern task main_task;

void task_switch(task *)
  __attribute__((nonnull));

void task_clone(task * restrict t)
  __attribute__((nonnull));

void task_run(task * restrict, void (*)(void *))
  __attribute__((nonnull));

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

typedef enum x86_task_state {
  INVALID = 0,
  RUNNING,
  SUSPENDED,
  FINALIZED,
} x86_task_state;

typedef struct x86_task {
  char pad[64];
  x86_task_state state;

  void *sp;

  //union {
  //  x86_registers regs;
  //  char u8[0];
  //  uint64_t u64[0];
  //} *sp;

  char __attribute__((aligned(16))) stack[1024 * 1024 * 2]; // 2 MB stacks
} __attribute__((aligned(16))) x86_task;

/*
 * Suspend the running task, switch into the next task
 */
void x86_task_switch(void * restrict prev_sp, void *restrict next_sp)
  __attribute__((nonnull));

/*
 * Switch into the next task
 *
 * sp - next task stack pointer
 */
void x86_task_resume(void *sp)
  __attribute__((nonnull));

/*
 *
 */
void x86_sub_task_finalize_jump(void);
void x86_sub_task_finalize(x86_task *task)
  __attribute__((nonnull));

/*
 *
 */
void x86_base_task_finalize_jump(void);
void x86_base_task_finalize(x86_task * restrict task, void * restrict thread_sp)
  __attribute__((nonnull));

/*
 * Create a copy of the active task
 *
 * task - new task
 */
void x86_task_clone(task * restrict task)
  __attribute__((nonnull));

/*
 * Jumped to from x86_task_clone
 */
void x86_task_clone_finish(task * restrict task)
  __attribute__((nonnull));


/*
 * Restore the original thread which called x86_task_base_run
 */
void x86_task_thread_restore(void * restrict sp)
  __attribute__((nonnull));

/*
 * Suspend the calling thread, switch into the base task
 */
void x86_base_task_run(task *restrict task, void (*fn)(void *))
  __attribute__((nonnull));

/*
 * Suspend the original thread, switch into the base task
 *
 * sp - base task stack pointer
 */
void x86_base_task_switch(void *sp)
  __attribute__((nonnull));

#endif
