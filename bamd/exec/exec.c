/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>

#include "xlinux/xstdlib.h"
#include "bacon/value.h"
#include "bacon/writer.h"
#include "bacon/builder.h"

#include "exec_builder.h"
#include "exec.h"
#include "exec_value.h"

#include "common/assure.h"
#include "common/attrs.h"

void exec_free(exec * e)
{
  if(e)
  {
    free(e->args);
    free(e->envs);
  }
  free(e);
}

void exec_ifree(exec ** restrict ep)
{
  if(*ep)
    exec_free(*ep);

  *ep = 0;
}

int env_def_cmp(const char * restrict name, const char * restrict text)
{
  int r;

  while(name && text)
  {
    if((r = INTCMP(*name, *text)))
      return r;

    name++;
    if(!*name || *name == '=')
      name = 0;

    text++;
    if(!*text || *text == '=')
      text = 0;
  }

  if(!name && !text)
    return 0;

  return INTCMP(name, text);
}

void exec_describe(const struct exec * restrict exec, bacon_writer * restrict builder_writer)
{
  char **s;

  { SET
    { MAPPING STRING("cwd")
      BYTES(exec->cwd, exec->cwd_len)
    }
    { MAPPING STRING("file")
      BYTES(exec->file_path, exec->file_path_len)
    }
    { MAPPING STRING("args")
      { LIST
        for(s = exec->args; *s; s++) {
          STRING(*s)
        }
      }
    }
    { MAPPING STRING("envs")
      { SET
        for(s = exec->envs; *s; s++) {
          STRING(*s)
        }
      }
    }
  }
}

void exec_show(const struct exec * restrict exec)
{
  bacon_writer writer, *builder_writer;

  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, g_narrator_stdout);

  exec_describe(exec, builder_writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  printf("\n");
}
