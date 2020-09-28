#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "x86.h"

task *task_active;

void task_run(task *task, void (*fn)(void *))
{
  task_active = task;
  x86_base_task_run(task, fn);
}
