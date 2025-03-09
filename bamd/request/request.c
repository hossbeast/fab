/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "bacon/writer.h"
#include "bacon/builder.h"

#include "request.h"
#include "command.h"

void request_cleanup()
{
}

void request_init(request * restrict req)
{
  memset(req, 0, sizeof(*req));
  llist_init(&req->commands);
}

void request_destroy(request * restrict req)
{
  command_free_list(&req->commands);
}

void request_describe(request * const restrict req, bacon_writer * const restrict builder_writer)
{
  command *cmd;

  { LIST
    llist_foreach(&req->commands, cmd, lln) {
      command_describe(cmd, builder_writer);
    }
  }
}

void request_say(request * restrict req, narrator * restrict N)
{
  bacon_writer * writer = 0;
  bacon_writer_create(&writer);
  bacon_writer_open(writer, N);

  request_describe(req, writer);

  bacon_writer_close(writer);
  bacon_writer_free(writer);
}

void request_show(request * restrict req)
{
  request_say(req, g_narrator_stdout);
  printf("\n");
}
