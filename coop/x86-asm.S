.section .text

/*
 * Save state of the current task, switch to the next task
 *
 * %rdi pointer to prev task sp
 * %rsi pointer to next task sp
 *
 * %rip was pushed by the call
 */
.global x86_switch
x86_switch:
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
 * Switch to the next task (called from x86_task_finalize)
 *
 * %rdi pointer to next task sp
 */
.global x86_resume
x86_resume:
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
 * foo
 *
 * %rdi pointer to domain
 * %rsi pointer to task
 */
.global x86_task_assimilate
x86_task_assimilate:
  /* current stack pointer */
  mov %rsp, %rdx

  jmp x86_task_assimilate_finish

/*
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  return address from x86_task_assimilate
  mov (%rsp), %rax
  push %rax

  current stack pointer
  push %rsp
  mov %rsp, %rdx

  jmp x86_task_assimilate_finish
*/

/*
 * Write register state to the dst fiber, jump to clone_finalize
 *
 * %rdi pointer to domain
 * %rsi pointer to the new fiber
 *
 * %rsp was pushed by the call
 */
.global x86_clone_fiber
x86_clone_fiber:
  /* fiber stack */
  mov 32(%rsi), %rcx

  /* this order corresponds directly to x86_registers layout */
  pushf
  pop %rax
  mov %rax, (%rcx)
  mov %r15, 8(%rcx)
  mov %r14, 16(%rcx)
  mov %r13, 24(%rcx)
  mov %r12, 32(%rcx)
  mov %rbx, 40(%rcx)
  mov %rbp, 48(%rcx)

  /* return address from x86_clone_fiber */
  mov (%rsp), %rax
  mov %rax, 56(%rcx)

  /* current stack pointer */
  mov %rsp, %rdx

  jmp x86_clone_fiber_finish


/*
 * Write register state to the dst fiber, jump to clone_finalize
 *
 * %rdi pointer to domain
 * %rsi pointer to the task
 *
 * %rsp was pushed by the call
 */
.global x86_clone_task
x86_clone_task:
  /* this order corresponds directly to x86_registers layout */
  pushf
  pop %rax
  mov %rax, 36+0(%rsi)
  mov %r15, 36+8(%rsi)
  mov %r14, 36+16(%rsi)
  mov %r13, 36+24(%rsi)
  mov %r12, 36+32(%rsi)
  mov %rbx, 36+40(%rsi)
  mov %rbp, 36+48(%rsi)

  /* return address from x86_clone_task */
  mov (%rsp), %rax
  mov %rax, 36+56(%rsi)

  /* current stack pointer */
  mov %rsp, %rdx

  jmp x86_clone_task_finish


/*
 * Restore the original thread which called x86_task_base_run
 */
.global x86_thread_restore
x86_thread_restore:
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
 * Suspend the thread and switch to the base fiber
 *
 * %rdi pointer to domain
 * %rsi pointer to next fiber sp
 *
 * %rip was pushed by the call
 */
.global x86_base_fiber_switch
x86_base_fiber_switch:
  /* save the active frame onto the active stack */
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
  pushf

  /* save threads stack pointer onto base fiber stack */
  mov %rsp, 80(%rsi)

  /* switch to base fiber stack */
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

/* base fiber */
.global x86_base_fiber_finalize_jump
x86_base_fiber_finalize_jump:
  pop %rdi
  pop %rsi
  jmp x86_base_fiber_finalize

/* sub fibers */
.global x86_sub_fiber_finalize_jump
x86_sub_fiber_finalize_jump:
  pop %rdi
  jmp x86_sub_fiber_finalize

/* tasks */
.global x86_task_finalize_jump
x86_task_finalize_jump:
  pop %rdi          # task
  pop %rsi          # fiber
  jmp x86_task_finalize

/* utils */

.global x86_clone_return
x86_clone_return:
  mov $0, %rax
  ret
