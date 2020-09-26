#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "x86.h"

static task main_task;
static task sub_task;

static void comain(void * arg)
{
  int x;

printf("start of comain\n");
printf("subtask %p\n", &sub_task);

  x86_task_clone(&sub_task);
  printf("task clone complete\n");

  for(x = 0; x < 3; x++)
  {
    printf("[%p][%p] x %d\n", task_active, __builtin_frame_address(0), x);

    if(task_active == &main_task) {
      task_switch(&sub_task);
    } else {
      task_switch(&main_task);
    }
  }

  printf("[%p][%p] end of comain\n", task_active, __builtin_frame_address(0));
}

static void goats(void)
{
  printf("goats\n");
}

int main(int argc, char ** argv)
{
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

goats();

printf("before task_run, comain %p\n", comain);
printf(" [%p] main\n", &main_task);
printf(" [%p] sub\n", &sub_task);

  task_run(&main_task, comain);

printf("after task_run\n");

  return 0;
}
