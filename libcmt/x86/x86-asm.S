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

.global x86_base_task_finalize_jump
x86_base_task_finalize_jump:
  pop %rdi
  pop %rsi
  jmp x86_base_task_finalize

.global x86_sub_task_finalize_jump
x86_sub_task_finalize_jump:
  pop %rdi
  jmp x86_sub_task_finalize

.global x86_task_sub_clone_return
x86_task_sub_clone_return:
  mov $0, %rax
  ret

/*
 * Switch into the next task (called from x86_task_finalize)
 *
 * %rdi pointer to next task sp
 */
.global x86_task_resume
x86_task_resume:
  /* switch stacks */
  mov %rdi, %rsp

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
 * Write register state to the dst task, jump to clone_finalize
 *
 * %rdi pointer to domain
 * %rsi pointer to the new task
 */
.global x86_task_clone
x86_task_clone:
  /* the order corresponds directly to struct x86_registers layout */
  pushf
  pop %rax
  mov %rax, (%rsi)
  mov %r15, 8(%rsi)
  mov %r14, 16(%rsi)
  mov %r13, 24(%rsi)
  mov %r12, 32(%rsi)
  mov %rbx, 40(%rsi)
  mov %rbp, 48(%rsi)

  /* return address from x86_task_clone */
  mov (%rsp), %rax
  mov %rax, 56(%rsi)

  /* current stack pointer */
  mov %rsp, 64(%rsi)

  jmp x86_task_clone_finish

/*
 * Restore the original thread which called x86_task_base_run
 */
.global x86_task_thread_restore
x86_task_thread_restore:
  /* restore the original thread stack pointer */
  mov %rdi, %rsp

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
 * Suspend the thread and jump into the base task
 *
 * %rdi pointer to domain
 * %rsi pointer to next task sp
 *
 * %rip was pushed by the call
 */
.global x86_base_task_switch
x86_base_task_switch:
  /* save the active frame onto the active stack */
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  /* save threads stack pointer onto base task stack */
  mov %rsp, 80(%rsi)

  /* switch to base task stack */
  mov %rsi, %rsp

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

///*
// * Suspend the active task onto its stack
// *
// * %rdi pointer to active task sp
// *
// * %rip was pushed by the call
// */
//.global x86_task_suspend
//x86_task_suspend:
//  /* save general purpose registers */
//  push %rbp
//  push %rbx
//  push %r12
//  push %r13
//  push %r14
//  push %r15
//  pushf
//
//  /* return the stack pointer */
//  mov %rsp, (%rdi)
//
//  jmp *56(%rsp)
//
//  //push *56(%rsp)
//  //ret
//
//  ///* return without pop */
//  //mov $1, %rax
//  //jmp *56(%rsp)
//
///*
// * Resume the active task - returns from x86_task_suspend
// *
// * %rdi pointer to active task sp
// */
//.global x86_task_resume
//x86_task_resume:
//  /* save the return address for this call */
//  pop %rax
//
//  /* restore the stack pointer */
//  mov (%rdi), %rsp
//
//  /* restore general purpose registers */
//  popf
//  pop %r15
//  pop %r14
//  pop %r13
//  pop %r12
//  pop %rbx
//  pop %rbp
//
//  /* pop, jump */
//  jmp *%rax
//  #mov (%rsp), %rax
//  #mov %rax, (%rdi)     /* return rip back into calling frame */
//  #mov %rbp, 8(%rdi)    /* return rbp for calling frame */
//  #mov %rsp, 16(%rdi)   /* return stack pointer */
  #pop %rax  /* discard callers frame pointer */
  #pop %rax  /* discard callers return address */
  #pop %rsp  /* recover thread stack pointer */
