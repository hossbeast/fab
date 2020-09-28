#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "x86.h"

task sub_task;

static void boats()
{
  int x;

  for(x = 0; x < 5; x++) {
    printf("sub  [%p][%p] x %d\n", task_active, __builtin_frame_address(0), x);
    task_switch(&main_task);
  }
}

static void comain(void * arg)
{
  int x;

  printf("[%p][%p] comain start\n", task_active, __builtin_frame_address(0));

  x86_task_clone(&sub_task);

  if(task_active == &sub_task) {
    boats();
    return;
  }

  for(x = 0; x < 2; x++)
  {
    printf("base [%p][%p] x %d\n", task_active, __builtin_frame_address(0), x);
    task_switch(&sub_task);
  }

  printf("base [%p][%p] comain end\n", task_active, __builtin_frame_address(0));
}

int main(int argc, char ** argv)
{
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

printf("before task_run, comain %p\n", comain);
printf(" [%p] main\n", &main_task);
printf(" [%p] sub\n", &sub_task);

  task_run(&main_task, comain);

printf("after task_run\n");

  return 0;
}
