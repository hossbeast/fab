#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#include "task.h"
#include "x86.h"

extern void *readrip(void);
extern void jump_task_init(struct x86_task *task);
extern void jump(struct x86_task *task);

static void jumpframe(void)
{
  printf("jumpframe\n");
}

#define containerof(ptr, type, member) ({        \
  void *__mptr = (void*)(ptr);                   \
  (type*)(__mptr - offsetof(type, member));      \
})

static inline uint64_t pad_to(uint64_t x, uint64_t align)
{
  uint64_t r = x & (align - 1);
  if(!r)
    return 0;

  return align - r;
}

void x86_task_clone_finalize(x86_task *task)
{
  x86_registers *regs;
  size_t size;
  size_t pad;
  void *sp;
  void *rsp;

  /* get the size of the calling frame */
  regs = (void*)task;
  rsp = *(void**)&regs[1];
  size = regs->rbp - (uint64_t)rsp;
  pad = pad_to(size, 16);

  printf("clone -> task %p\n", task);
  printf(" rip 0x%08lx\n", regs->rip);
  printf(" rbp 0x%08lx\n", regs->rbp);
  printf(" rsp 0x%08lx\n", (uint64_t)rsp);
  printf(" frame\n");
  printf("  size %zu + %zu = %zu\n", size, pad, size + pad);

  /* build the new task stack */
  sp = task->stack + sizeof(task->stack);

  /* allocate the jump frame */
  sp -= 16;

  /* allocate the start frame */
  sp -= (size + pad);

  /* allocate the resume frame */
  sp -= sizeof(*regs);
  task->sp = sp;

  /* build the resume frame */
  regs->rbp = (uint64_t)sp + sizeof(*regs) + size; // base of the start frame (highest address)
  memcpy(sp, regs, sizeof(*regs));
  sp += sizeof(*regs);

  /* copy the start frame from the active task */
  memcpy(sp, rsp, size);
  sp += size;

  /* the jump frame */
  *(void**)sp = jumpframe;
}

void x86_task_run(x86_task *task, void (*fn)(void *))
{
  void *sp;

printf("x86_task run, task %p fn %p\n", task, fn);
printf(" stack 0x%08lx - 0x%08lx\n", (uint64_t)(void*)task->stack, (uint64_t)(void*)(task->stack + sizeof(task->stack)));

  sp = task->stack + sizeof(task->stack);
  sp -= sizeof(task->sp->regs);

//printf(" sp 0x%08lx : end - 0x%lx\n", (uint64_t)sp, (uint64_t)(task->stack + sizeof(task->stack)) - (uint64_t)sp);
//printf(" sp/64 %lu\n", (uint64_t)sp % 64);
//printf(" sp/32 %lu\n", (uint64_t)sp % 32);
//printf(" sp/16 %lu\n", (uint64_t)sp % 16);
//printf(" sp/8  %lu\n", (uint64_t)sp % 8);

  /* allocate the jump frame */
  sp -= 16;

  sp -= 8; /* for primeval restore */
//  sp -= 8; /* 16b alignment */
//  sp -= 8; /* wtf */

printf(" sp 0x%08lx : end - 0x%lx\n", (uint64_t)sp, (uint64_t)(task->stack + sizeof(task->stack)) - (uint64_t)sp);
printf(" sp/64 %lu\n", (uint64_t)sp % 64);
printf(" sp/32 %lu\n", (uint64_t)sp % 32);
printf(" sp/16 %lu\n", (uint64_t)sp % 16);
printf(" sp/8  %lu\n", (uint64_t)sp % 8);

  task->sp = sp;
  memset(&task->sp->regs, 0, sizeof(task->sp->regs));
  task->sp->regs.rip = (uintptr_t)fn;

  /* return address beyond the base frame */
  task->sp->u64[8] = (uintptr_t)x86_task_primeval_restore;
  //    sp->u64[9] = primeval stack pointer

printf("task jump\n");
  x86_task_jump(task->sp);
}

void task_switch(x86_task *next)
{
  x86_task *prev;

  prev = task_active;
  task_active = next;

  x86_task_switch(&prev->sp, &next->sp);
}

#if 0
void x86_task_clone_finalize(x86_task *task)
{
  x86_task *active;
  void *rsp;
  void *rbp;
  size_t size;

  active = containerof(task_active, typeof(*active), t);

  printf("suspend start\n");
  x86_task_suspend(&active->sp);
  printf("suspend done\n");

  /* get the size of the current stack frame */
  rbp = (void*)(uintptr_t)active->sp->regs.rbp;
  rsp = active->sp->u8;
  size = rbp - rsp;
  /* excluding the state we just pushed onto the stack */
  size -= 64;

  /* allocate the jump frame */
  rsp = task->stack + sizeof(task->stack);
  rsp -= 8;

  /* allocate start frame */
  rsp = task->stack + sizeof(task->stack);
  rsp -= size;

printf("start frame : %zu\n", size);

  /* copy the start frame */
  memcpy(rsp, (void*)(uintptr_t)active->sp->regs.rbp, size);

  /* setup the jump frame */
  void *jt = jumpframe;
  memcpy(rsp + size, &jt, sizeof(jt));

  /* now undo damage to the active stack */
  //active->sp += 64;

  printf("calling resume\n");
  x86_task_resume(&active->sp);
}
#endif
