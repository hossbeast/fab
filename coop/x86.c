#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#include "x86.h"
#include "task.h"
#include "macros.h"
#include "locks.h"

static inline uint64_t pad_to(uint64_t x, uint64_t align)
{
  uint64_t r = x & (align - 1);
  if(!r)
    return 0;

  return align - r;
}

/*
 * topmost frame of base fiber jumps here
 *
 * thread_sp - stack pointer of original thread which called cmt_run
 */
void x86_base_fiber_finalize(x86_fiber *fiber, void *thread_sp)
{
  RUNTIME_ASSERT(fiber->domain->executing == fiber);

  fiber->state = FINALIZED;
  x86_thread_restore(thread_sp);
}

/* topmost frame of sub-fibers jumps here */
void x86_sub_fiber_finalize(x86_fiber *fiber)
{
  x86_domain *domain;
  x86_fiber *next, *parent;

  /* finalize the executing fiber */
  domain = fiber->domain;
  RUNTIME_ASSERT(domain->executing == fiber);
  fiber->state = FINALIZED;

  /* re-enable the parent fiber if any - it can only be in the same domain */
  parent = fiber->parent;
  if(__atomic_sub_fetch(&parent->children, 1, __ATOMIC_RELAXED) == 0) {
    parent->state = RUNNING;
    llist_push(&parent->domain->runqueue, parent, lln);
  }

  /* switch to the next fiber */
  RUNTIME_ASSERT(!llist_empty(&domain->runqueue));

  next = llist_shift(&domain->runqueue, typeof(*next), lln);
  next->state = EXECUTING;
  domain->executing = next;

  x86_resume(next->sp);
}

/*
 * topmost frame of tasks jumps here
 *
 * task  - task which has returned
 * fiber - fiber which executed the task
 */
void x86_task_finalize(x86_task * restrict task, x86_fiber * restrict fiber)
{
  x86_fiber *parent;
  x86_domain *domain;

  /* re-enable the parent fiber if any - it may be in another domain */
  parent = task->parent;
  if(__atomic_sub_fetch(&parent->children, 1, __ATOMIC_RELAXED) == 0) {
    parent->state = RUNNING;

    domain = parent->domain;
    futex_acquire(&domain->joinqueue_lock);
    llist_push(&domain->joinqueue, parent, lln);
    futex_release(&domain->joinqueue_lock);

#if 0
    if(domain->waiting) {
//printf("JOIN WAKE\n");
      syscall(SYS_futex, &domain->futex, FUTEX_WAKE, 1, 0, 0, 0);
    }
#endif
  }

  x86_resume(fiber->sp);
}

void x86_task_enqueue(x86_domain * restrict domain, x86_task * restrict task)
{
  futex_acquire(&domain->taskqueue_lock);
  llist_push(&domain->taskqueue, task, lln);
  futex_release(&domain->taskqueue_lock);

#if 0
  if(domain->waiting) {
printf("ENQUEUE WAKE\n");
    syscall(SYS_futex, &domain->futex, FUTEX_WAKE, 1, 0, 0, 0);
  }
#endif
}

/*
 * x86 task api
 */

x86_fiber *x86_clone_fiber_finish(x86_domain * restrict domain, x86_fiber * restrict fiber, void *rsp)
{
  x86_registers *regs;
  size_t size;
  size_t pad;
  void *sp;
  void *rip;

  RUNTIME_ASSERT(fiber->stack);
  RUNTIME_ASSERT(((uintptr_t)fiber->stack % 16) == 0);
  RUNTIME_ASSERT((fiber->stack_size % 2) == 0);
  RUNTIME_ASSERT(fiber->stack_size >= 8096);

  /* x86_clone_fiber stashes register state here */
  regs = fiber->stack;

  /* top of the calling frame, excluding the return address */
  rsp += 8;

  /* size of the calling frame */
  size = regs->rbp - (uint64_t)rsp;
  pad = pad_to(size, 16);

  /* build the new stack */
  sp = fiber->stack + fiber->stack_size;

  /* alignment padding */
  sp -= pad;

  /* allocate the top frame */
  sp -= 24;

  /* allocate the start frame */
  sp -= size;

  /* allocate the resume frame */
  sp -= (sizeof(*regs)) + 8;
  fiber->sp = sp;

  /* build the resume frame */
  // frame pointer - base of the start frame (highest address)
  regs->rbp = (uint64_t)sp + sizeof(*regs) + 8 + size;

  // bounce through to arrange x86_fiber_clone return value
  rip = (void*)regs->rip;
  regs->rip = (uintptr_t)x86_clone_return;

  memcpy(sp, regs, sizeof(*regs));
  sp += sizeof(*regs);

  /* popped in x86_clone_return */
  *(void**)sp = rip;
  sp += 8;

  /* copy the start frame from the executing fiber */
  memcpy(sp, rsp, size);
  sp += size;

  /* build the top frame */
  *(void**)sp = (void*)0xfacefacefaceface; // dummy frame pointer
  sp += 8;

  *(void**)sp = x86_sub_fiber_finalize_jump;
  sp += 8;

  /* arg0 to x86_sub_fiber_finalize */
  *(void**)sp = fiber;

  /* add to the runqueue */
  fiber->state = RUNNING;
  fiber->domain = domain;
  fiber->parent = domain->executing;
  __atomic_add_fetch(&fiber->parent->children, 1, __ATOMIC_RELAXED);
  llist_push(&domain->runqueue, fiber, lln);

  return fiber;
}

x86_task * x86_clone_task_finish(x86_domain * restrict domain, x86_task * restrict task, void *rsp)
{
  x86_registers *regs;
  size_t task_size;
  size_t size;
  void *sp;
  void *rip;

  /* x86_clone_task writes current state to the task stack */
  regs = (void*)task->stack;

  /* top of the calling frame excluding the return address */
  rsp += 8;

  /* this is the size of the calling frame */
  task_size = regs->rbp - (uint64_t)rsp;

//printf("regs @ %p\n", regs);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[0]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[1]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[2]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[3]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[4]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[5]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[6]);
//printf(" 0x%016lx\n", ((uint64_t*)regs)[7]);
//printf("task %p stack-size %hu\n", task, task->stack_size);

  size = 0;
  size += 32;                 // top frame
  size += task_size;          // task frame
  size += sizeof(*regs) + 8;  // resume frame

//printf("start-size %zu size %zu\n", task_size, size);

  RUNTIME_ASSERT(task->stack_size >= (size + sizeof(*regs)));

  /* build the stack frames */
  sp = task->stack + task->stack_size;
  sp -= size;
  task->sp = sp;

  /* build the resume frame */
  // frame pointer - base of the task frame (highest address)
  regs->rbp = (uint64_t)sp + sizeof(*regs) + 8 + task_size;

  // bounce through clone_return to arrange return value
  rip = (void*)regs->rip;
  regs->rip = (uintptr_t)x86_clone_return;

  memcpy(sp, regs, sizeof(*regs));
  sp += sizeof(*regs);

  /* popped in x86_clone_return */
  *(void**)sp = rip;
  sp += 8;

  /* ingest the task frame */
  memcpy(sp, rsp, task_size);
  sp += task_size;

  /* build the top frame */
  *(void**)sp = (void*)0xdeadbeefdeadbeef; // dummy frame pointer
  sp += 8;
  *(void**)sp = x86_task_finalize_jump;   // return address
  sp += 8;
  *(void**)sp = task;                     // arg0
  sp += 8;
  /* arg1 - fiber which executed the task */
  sp += 8;

  task->parent = domain->executing;
  __atomic_add_fetch(&task->parent->children, 1, __ATOMIC_RELAXED);

  return task;
}

void x86_task_assimilate_finish(x86_domain * restrict domain, x86_task *restrict task, void *sp)
{
  x86_fiber *fiber;
  uint16_t size;

  fiber = domain->executing;

  /* stash this fiber onto the task stack */
  *((void**)(((void*)task->stack) + task->stack_size - 8)) = fiber;

//  // return address from task_assimilate call
//  sp += 8;

//printf("sp 0x%016lx\n", (uint64_t)sp);

/* without this extra space, memcpy overwrites its own return address back to here */
sp -= 256;

//  /* executing fiber state */
//  sp -= sizeof(x86_registers);

  /* splice the task stack onto the executing fiber stack */
  size = (task->stack + task->stack_size) - (char*)task->sp;
  sp -= size;
//printf("sp 0x%016lx size 0x%hx\n", (uint64_t)sp, size);
  memcpy(sp, task->sp, size);

  x86_switch(&fiber->sp, &sp);
}

void x86_base_fiber_run(x86_fiber *fiber, void (*fn)(x86_domain *))
{
  x86_registers *regs;
  void *sp;
  size_t pad;

//printf("x86_task run, task %p fn %p\n", task, fn);
//printf(" stack 0x%08lx - 0x%08lx\n", (uint64_t)(void*)task->stack, (uint64_t)(void*)(task->stack + task->stack_size));

  pad = 0;

  /* build the base fiber stack */
  sp = fiber->stack + fiber->stack_size;

  /* alignment padding */
  sp -= pad;

  /* allocate the start frame */
  sp -= sizeof(*regs);

  /* allocate the top frame */
  sp -= 24;

  fiber->sp = sp;
  regs = sp;

  /* build the start frame */
  memset(regs, 0, sizeof(*regs));
  regs->rip = (uintptr_t)fn;
  sp += sizeof(*regs);

  /* build the top frame */
  *(void**)sp = x86_base_fiber_finalize_jump;
  sp += 8;

  /* arg0 to x86_base_task_finalize */
  *(void**)sp = fiber;
  sp += 8;

  /* x86_base_task_switch sets these 8 bytes to the original thread stack pointer */
  sp += 8;

  fiber->state = EXECUTING;
  fiber->domain->executing = fiber;
  x86_base_fiber_switch(fiber->domain, fiber->sp);

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

void cmt_run(x86_domain * restrict domain, x86_fiber * restrict fiber, void (*fn)(x86_domain *))
{
  RUNTIME_ASSERT(fiber->stack);
  RUNTIME_ASSERT(((uintptr_t)fiber->stack % 16) == 0);
  RUNTIME_ASSERT((fiber->stack_size % 2) == 0);
  RUNTIME_ASSERT(fiber->stack_size >= 8096);

  memset(domain, 0, sizeof(*domain));
  llist_init_node(&domain->runqueue);
  llist_init_node(&domain->joinqueue);
  llist_init_node(&domain->taskqueue);

  fiber->domain = domain;
  x86_base_fiber_run(fiber, fn);
}

void cmt_yield(x86_domain *domain)
{
  x86_fiber *prev, *next;

  /* no one to yield to */
#if 1
  RUNTIME_ASSERT(!llist_empty(&domain->runqueue));
#else
  if(llist_empty(&domain->runqueue)) {
    return;
  }
#endif

  prev = domain->executing;
  prev->state = RUNNING;
  llist_push(&domain->runqueue, prev, lln);

  next = llist_shift(&domain->runqueue, typeof(*next), lln);
  next->state = EXECUTING;
  domain->executing = next;

  x86_switch(&prev->sp, &next->sp);
}

void cmt_join(x86_domain *domain)
{
  x86_fiber *prev, *next;

  prev = domain->executing;

  /* no one to join */
  if(prev->children == 0) {
    return;
  }

  prev->state = JOINING;

  next = llist_shift(&domain->runqueue, typeof(*next), lln);
  next->state = EXECUTING;
  domain->executing = next;

  x86_switch(&prev->sp, &next->sp);
}
