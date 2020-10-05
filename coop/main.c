#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "x86.h"
#include "task.h"
#include "locks.h"
#include "llist.h"

#define STACKSIZE (1024 * 1024 * 2)
#define WORKERS 16
#define TARGET 1000

__thread int32_t g_tid;

static domain main_domain;
static fiber main_base_fiber;
static char __attribute__((aligned(16))) main_base_fiber_stack[STACKSIZE];
static fiber main_fiber;
static char __attribute__((aligned(16))) main_fiber_stack[STACKSIZE];

static domain worker_domains[WORKERS];
static fiber worker_fibers[WORKERS];
static char __attribute__((aligned(16))) worker_fiber_stacks[WORKERS][STACKSIZE];

static struct {
  struct {
    int works;
    x86_task t;
    char stack[256];
  };
} tasks[WORKERS];

static int done;
static int worker_domains_done;
static uint64_t sum;

static uint64_t sum_first(uint64_t n)
{
    return (n * (n + 1)) >> 1;
}

static void compute(x86_domain *domain)
{
  int x;
  int y;

  y = 0;
  for(x = 1; x <= TARGET; x++)
  {
    if(y == WORKERS) {
//printf("[%8d] x %d y %d compute join\n", g_tid, x, y);
      cmt_join(domain);
      y = 0;
    }
//printf("[%8d] x %d y %d clone\n", g_tid, x, y);
    if(!x86_clone_task(domain, &tasks[y].t)) {
printf("[%8d] x %d y %d TASK\n", g_tid, x, y);
      __atomic_fetch_add(&sum, x, __ATOMIC_RELAXED);
      tasks[y].works++;
      return;
    }
//printf("[%8d] x %d y %d enqueue task %p -> %d\n", g_tid, x, y, &tasks[y].t, y);
    x86_task_enqueue(&worker_domains[y], &tasks[y].t);

    y++;
  }

printf("[%8d] COMPUTE DONE\n", g_tid);
  done = 1;
}

static void comain(x86_domain *domain)
{
  g_tid = syscall(SYS_gettid);
printf("[%8d] main\n", g_tid);

  if(x86_clone_fiber(domain, &main_fiber) == 0) {
    return compute(domain);
  }

  while(!done)
  {
    if(!llist_empty(&domain->runqueue)) {
//printf("[%8d] main yield\n", g_tid);
      cmt_yield(domain);
      continue;
    }

    if(!llist_empty(&domain->joinqueue)) {
//printf("[%8d] main joinqueue\n", g_tid);
      futex_acquire(&domain->joinqueue_lock);
      llist_splice_tail(&domain->runqueue, &domain->joinqueue);
      futex_release(&domain->joinqueue_lock);
      continue;
    }

//printf("[%8d] main wait start\n", g_tid);
//    domain->waiting = 1;
//    syscall(SYS_futex, &domain->futex, FUTEX_WAIT, 0, 0, 0, 0);
//    domain->waiting = 0;
//printf("[%8d] main wait done\n", g_tid);
  }

printf("[%8d] main domain done\n", g_tid);
}

static void worker_main(x86_domain *domain)
{
  x86_task *task;
  llist *lln;

  g_tid = syscall(SYS_gettid);

printf("[%8d] worker main\n", g_tid);

  while(!done)
  {
//printf("[%8d] frame address 0x%016lx is 0x%016lx + 0x%lx\n"
//  , g_tid
//  , (uint64_t)__builtin_frame_address(0)
//  , (uint64_t)worker_fibers[0].stack
//  , (uint64_t)__builtin_frame_address(0) - (uint64_t)worker_fibers[0].stack
//);
    if(!llist_empty(&domain->runqueue)) {
//printf("[%8d] worker yield\n", g_tid);
      cmt_yield(domain);
      continue;
    }

    if(!llist_empty(&domain->joinqueue)) {
//printf("[%8d] worker joinqueue\n", g_tid);
      futex_acquire(&domain->joinqueue_lock);
      llist_splice_tail(&domain->runqueue, &domain->joinqueue);
      futex_release(&domain->joinqueue_lock);
      continue;
    }

    if(!llist_empty(&domain->taskqueue)) {
      futex_acquire(&domain->taskqueue_lock);
      lln = domain->taskqueue.next;
      llist_delete_node(lln);
      futex_release(&domain->taskqueue_lock);
      task = containerof(lln, typeof(*task), lln);
//printf("[%8d] assimilate task %p\n", g_tid, task);
      x86_task_assimilate(domain, task);
//printf("[%8d] assimilate return to worker-main\n", g_tid);
      continue;
    }

//printf("[%8d] worker wait start\n", g_tid);
//    domain->waiting = 1;
//    syscall(SYS_futex, &domain->futex, FUTEX_WAIT, 0, 0, 0, 0);
//    domain->waiting = 0;
//printf("[%8d] worker wait done\n", g_tid);
  }

printf("[%8d] worker domain done\n", g_tid);
  __atomic_fetch_add(&worker_domains_done, 1, __ATOMIC_RELAXED);
}

static void * worker_thread(void * arg)
{
  int x;

  x = (int)(intptr_t)arg;
  cmt_run(&worker_domains[x], &worker_fibers[x], worker_main);
  return 0;
}

int main(int argc, char ** argv)
{
  int x;

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

//printf("before cmt_run, comain %p\n", comain);
//printf(" [%p] task0\n", &tasks[0]);
//printf(" [%p] task1\n", &tasks[1]);
//printf(" [%p] task2\n", &tasks[2]);
//printf(" [%p] task3\n", &tasks[3]);
//printf(" [%p] task4\n", &tasks[4]);

  for(x = 0; x < sizeof(tasks) / sizeof(*tasks); x++)
  {
    tasks[x].t.stack_size = sizeof(tasks[0].stack);
  }

  main_fiber.stack = main_fiber_stack;
  main_fiber.stack_size = STACKSIZE;

  main_base_fiber.stack = main_base_fiber_stack;
  main_base_fiber.stack_size = STACKSIZE;

  for(x = 0; x < sizeof(worker_fibers) / sizeof(*worker_fibers); x++)
  {
    worker_fibers[x].stack = worker_fiber_stacks[x];
    worker_fibers[x].stack_size = STACKSIZE;
  }

#if 0
  for(x = 0; x < sizeof(worker_fibers) / sizeof(*worker_fibers); x++)
  {
printf("fiber[%8d] %016lx stack [ 0x%016lx , 0x%016lx ) size 0x%x\n"
  , x
  , (uint64_t)&worker_fibers[x]
  , (uint64_t)worker_fibers[x].stack
  , (uint64_t)worker_fibers[x].stack + worker_fibers[x].stack_size
  , worker_fibers[x].stack_size
);
  }

  for(x = 0; x < sizeof(tasks) / sizeof(*tasks); x++)
  {
printf("tasks[%8d] %016lx stack [ 0x%016lx , 0x%016lx ) size 0x%x\n"
  , x
  , (uint64_t)&tasks[x].t
  , (uint64_t)tasks[x].t.stack
  , (uint64_t)tasks[x].t.stack + tasks[x].t.stack_size
  , tasks[x].t.stack_size
);
  }
#endif

  pthread_t pthread_id;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  for(x = 0; x < WORKERS; x++) {
    pthread_create(&pthread_id, &attr, worker_thread, (void*)(intptr_t)x);
  }

  cmt_run(&main_domain, &main_base_fiber, comain);

while(worker_domains_done < WORKERS);

  printf("expected %lu actual %lu\n", sum_first(TARGET), sum);

  for(x = 0; x < WORKERS; x++) {
    printf("%2d works %d\n", x, tasks[x].works);
  }

  return 0;
}
