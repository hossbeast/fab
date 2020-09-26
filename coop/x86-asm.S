.section .text

//.global jump_task_init
//.global readrip

/*
 * Save state of the current task, switch into the next task
 *
 * %rdi pointer to prev task sp
 * %rsi pointer to next task sp
 *
 * %rip was pushed by the call
 */
.global x86_task_switch
x86_task_switch:
  /* save general purpose registers */
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  /* switch stacks */
  mov %rsp, (%rdi)
  mov (%rsi), %rsp

  /* restore general purpose registers */
  popf
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp

  /* pop, jump */
  ret

/*
 * Restore the primeval thread
 */
.global x86_task_primeval_restore
x86_task_primeval_restore:
  /* recover the stack pointer */
  mov (%rsp), %rsp

  /* recover state */
  popf
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp

  /* pop, jump */
  ret

/*
 * Suspend the primeval thread, jump into the base task
 *
 * %rdi pointer to next task sp
 *
 * %rip was pushed by the call
 */
.global x86_task_jump
x86_task_jump:
  /* save the active frame onto the active stack */
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  /* save primeval stack pointer onto next stack */
  mov %rsp, 72(%rdi)

  /* switch to next stack */
  mov %rdi, %rsp

  /* zero out registers from the blank (top) frame */
  popf
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp

  /* pop, jump */
  ret

/*
 * Suspend the active task onto its stack
 *
 * %rdi pointer to active task sp
 *
 * %rip was pushed by the call
 */
.global x86_task_suspend
x86_task_suspend:
  /* save general purpose registers */
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  /* return the stack pointer */
  mov %rsp, (%rdi)

  jmp *56(%rsp)

  //push *56(%rsp)
  //ret

  ///* return without pop */
  //mov $1, %rax
  //jmp *56(%rsp)

/*
 * Resume the active task - returns from x86_task_suspend
 *
 * %rdi pointer to active task sp
 */
.global x86_task_resume
x86_task_resume:
  /* save the return address for this call */
  pop %rax

  /* restore the stack pointer */
  mov (%rdi), %rsp

  /* restore general purpose registers */
  popf
  pop %r15
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp

  /* pop, jump */
  jmp *%rax

/*
 * %rdi pointer to the new task
 */
.global x86_task_clone
x86_task_clone:
  #mov (%rsp), %rax
  #mov %rax, (%rdi)     /* return rip back into calling frame */
  #mov %rbp, 8(%rdi)    /* return rbp for calling frame */
  #mov %rsp, 16(%rdi)   /* return stack pointer */

  /* the order corresponds directly to struct x86_registers layout */
  pushf
  pop %rax
  mov %rax, (%rdi)
  mov %r15, 8(%rdi)
  mov %r14, 16(%rdi)
  mov %r13, 24(%rdi)
  mov %r12, 32(%rdi)
  mov %rbx, 40(%rdi)
  mov %rbp, 48(%rdi)

  /* return address from task_clone */
  mov (%rsp), %rax
  mov %rax, 56(%rdi)

  /* value of the stack pointer */
  mov %rsp, 64(%rdi)

  jmp x86_task_clone_finalize

//readrip:
//#  push %rbp       /* save stack base */
//#  mov %rsp, %rbp  /* allocate new frame */
//
//  /* read return address off the stack into rax */
//  movq (%rsp), %rax
//
//#  pop %rbp        /* restore caller base frame */
//  ret
//
///*
// * %rdi task
// */
//jump_task_init:
//  /* read return address into rsi */
//  mov (%rsp), %rsi
//
//  jmp task_init
//
///*
// * %rdi task
// */
//jump:
//  /* read return address into task task */
//  mov (%rsp), %rsi
//
