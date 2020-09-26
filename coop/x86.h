#ifndef _TASK_x86_H
#define _TASK_x86_H

#include <stdint.h>

/*
 * api
 */

typedef struct x86_task task;

/* executing task */
extern task *task_active;

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

typedef struct x86_task {
  char pad[64];

  union {
    x86_registers regs;
    char u8[0];
    uint64_t u64[0];
  } *sp;

  char __attribute__((aligned(16))) stack[1024 * 1024 * 2]; // 2 MB stacks
} __attribute__((aligned(16))) x86_task;

extern int x86_task_suspend(void *sp)
  __attribute__((nonnull));

extern void x86_task_resume(void *sp)
  __attribute__((nonnull));

extern void x86_task_switch(void * restrict prev_sp, void *restrict next_sp)
  __attribute__((nonnull));

extern void x86_task_jump(void *sp)
  __attribute__((nonnull));

extern void x86_task_run(task *restrict task, void (*fn)(void *))
  __attribute__((nonnull));

extern void x86_task_clone(task * restrict task)
  __attribute__((nonnull));

extern void x86_task_clone_finalize(task * restrict task)
  __attribute__((nonnull));

extern void x86_task_primeval_restore(task *)
  __attribute__((nonnull));

#endif
