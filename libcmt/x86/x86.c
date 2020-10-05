#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#include "x86.h"
#include "task.h"
#include "macros.h"

static inline uint64_t pad_to(uint64_t x, uint64_t align)
{
  uint64_t r = x & (align - 1);
  if(!r)
    return 0;

  return align - r;
}

/* topmost frame of base task jumps here */
void x86_base_task_finalize(x86_task *task, void *thread_sp)
{
  RUNTIME_ASSERT(task->domain->executing == task);

  task->state = FINALIZED;
  x86_task_thread_restore(thread_sp);
}

/* topmost frame of sub-tasks jump here */
void x86_sub_task_finalize(x86_task *task)
{
  x86_task_domain *domain;

  domain = task->domain;
  RUNTIME_ASSERT(domain->executing == task);
  task->state = FINALIZED;

  llist_delete_node(&domain->base->lln);
  domain->executing = domain->base;
  x86_task_resume(domain->base->sp);
}

/*
 * x86 task api
 */

x86_task *x86_task_clone_finish(x86_task_domain * restrict domain, x86_task * restrict task)
{
  x86_registers *regs;
  size_t size;
  size_t pad;
  void *sp;
  void *rsp;
  void *rip;

  RUNTIME_ASSERT(task->stack);
  RUNTIME_ASSERT(((uintptr_t)task->stack % 16) == 0);
  RUNTIME_ASSERT((task->stack_size % 2) == 0);
  RUNTIME_ASSERT(task->stack_size >= 8096);

  /* get the size of the calling frame */
  regs = (void*)task;
  rsp = *(void**)&regs[1];
  size = regs->rbp - (uint64_t)rsp;
  pad = pad_to(size, 16);

//  printf("clone -> task %p\n", task);
//  printf(" rip 0x%08lx\n", regs->rip);
//  printf(" rbp 0x%08lx\n", regs->rbp);
//  printf(" rsp 0x%08lx\n", (uint64_t)rsp);
//  printf(" frame\n");
//  printf("  size %zu + %zu = %zu\n", size, pad, size + pad);

  /* build the new task stack */
  sp = task->stack + task->stack_size;

  /* alignment padding */
  sp -= pad;

  /* allocate the top frame */
  sp -= 24;

  /* allocate the start frame */
  sp -= size;

  /* allocate the resume frame */
  sp -= (sizeof(*regs)) + 8;
  task->sp = sp;

  /* build the resume frame */
  // frame pointer - base of the start frame (highest address)
  regs->rbp = (uint64_t)sp + sizeof(*regs) + 8 + size;

  // bounce through to arrange x86_task_clone return value
  rip = (void*)regs->rip;
  regs->rip = (uintptr_t)x86_task_sub_clone_return;

  memcpy(sp, regs, sizeof(*regs));
  sp += sizeof(*regs);

  /* popped in x86_task_sub_clone_return */
  *(void**)sp = rip;
  sp += 8;

  /* copy the start frame from the active task */
  memcpy(sp, rsp, size);
  sp += size;

  /* build the top frame */
  *(void**)sp = (void*)0xdeadbeef; // dummy frame pointer
  sp += 8;

  *(void**)sp = x86_sub_task_finalize_jump;
  sp += 8;

  /* arg0 to x86_sub_task_finalize */
  *(void**)sp = task;

  /* add to the runqueue */
  task->state = RUNNING;
  task->domain = domain;
  llist_push(&domain->runqueue, task, lln);

  return task;
}

void x86_base_task_run(x86_task *task, void (*fn)(x86_task_domain *))
{
  x86_registers *regs;
  void *sp;
  size_t pad;

//printf("x86_task run, task %p fn %p\n", task, fn);
//printf(" stack 0x%08lx - 0x%08lx\n", (uint64_t)(void*)task->stack, (uint64_t)(void*)(task->stack + task->stack_size));

  pad = 0;

  /* build the base task stack */
  sp = task->stack + task->stack_size;

  /* alignment padding */
  sp -= pad;

  /* allocate the start frame */
  sp -= sizeof(*regs);

  /* allocate the top frame */
  sp -= 24;

  task->sp = sp;
  regs = sp;

  /* build the start frame */
  memset(regs, 0, sizeof(*regs));
  regs->rip = (uintptr_t)fn;
  sp += sizeof(*regs);

  /* build the top frame */
  *(void**)sp = x86_base_task_finalize_jump; // x86_task_thread_restore;
  sp += 8;

  /* arg0 to x86_base_task_finalize */
  *(void**)sp = task;
  sp += 8;

  /* x86_base_task_switch sets these 8 bytes to the original thread stack pointer */
  sp += 8;

  task->state = EXECUTING;
  task->domain->executing = task;
  x86_base_task_switch(task->domain, task->sp);

  //memset(&task->sp->regs, 0, sizeof(task->sp->regs));
  //task->sp->regs.rip = (uintptr_t)fn;
//  sp -= 8; /* for thread restore */
//  sp -= 8; /* 16b alignment */
//  sp -= 8; /* wtf */
//printf(" sp 0x%08lx : end - 0x%lx\n", (uint64_t)sp, (uint64_t)(task->stack + sizeof(task->stack)) - (uint64_t)sp);
//printf(" sp/64 %lu\n", (uint64_t)sp % 64);
//printf(" sp/32 %lu\n", (uint64_t)sp % 32);
//printf(" sp/16 %lu\n", (uint64_t)sp % 16);
//printf(" sp/8  %lu\n", (uint64_t)sp % 8);
  ///* return address beyond the base frame */
  //task->sp->u64[8] = (uintptr_t)x86_task_thread_restore;
  ////    sp->u64[9] = primeval thread stack pointer
}

/*
 * task api
 */

void task_run(domain * restrict domain, task * restrict task, void (*fn)(x86_task_domain *))
{
  RUNTIME_ASSERT(task->stack);
  RUNTIME_ASSERT(((uintptr_t)task->stack % 16) == 0);
  RUNTIME_ASSERT((task->stack_size % 2) == 0);
  RUNTIME_ASSERT(task->stack_size >= 8096);

  llist_init_node(&domain->runqueue);
  domain->base = task;
  task->domain = domain;
  x86_base_task_run(task, fn);
}

static void yield(x86_task_domain *domain, x86_task_state state)
{
  x86_task *prev, *next;

  /* theres always the base task */
  RUNTIME_ASSERT(!llist_empty(&domain->runqueue));

  prev = domain->executing;
  prev->state = RUNNING;
  llist_push(&domain->runqueue, prev, lln);

  next = llist_shift(&domain->runqueue, typeof(*next), lln);
  next->state = EXECUTING;
  domain->executing = next;

  x86_task_switch(&prev->sp, &next->sp);
}

void task_yield(x86_task_domain *domain)
{
  yield(domain, RUNNING);
}

void task_join(x86_task_domain *domain)
{
  /* somehow assert that the base task never calls join */
  yield(domain, WAITING);
}

#if 0
extern void *readrip(void);
extern void jump_task_init(struct x86_task *task);
extern void jump(struct x86_task *task);

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
