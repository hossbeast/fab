/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

 This file is part of fab.

 fab is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 fab is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "types.h"
#include "xapi.h"

#include "value.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "valyria/set.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/map.h"
#include "value/parser.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"

#include "exec_builder.internal.h"
#include "node.h"
#include "path.h"
#include "logging.h"
#include "variant.h"
#include "formula.internal.h"
#include "formula_value.internal.h"
#include "build_thread.h"
#include "build_slot.h"
#include "module.h"
#include "selector.h"
#include "params.h"
#include "sysvar.h"

#include "common/snarf.h"
#include "common/assure.h"
#include "common/grow.h"
#include "common/attrs.h"

//
// static
//

struct def_name_compare_context {
  const char * textbase;
  const uint32_t * base;
  const char * name;  // key to find

  int lx; // last compare position
  int lc; // last compare result
};

/*
* compare two environment key/value strings by key name
*
* name - var name
* text - env key/value string
*/
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

static int def_name_compare(const void * _ctx, const void * _item)
{
  struct def_name_compare_context * ctx = (typeof(ctx))_ctx;

  const uint32_t * item = _item;

  ctx->lx = item - ctx->base;
  return ctx->lc = env_def_cmp(ctx->name, ctx->textbase + *item);
}

static xapi __attribute__((nonnull))
dispatch_render_function(
    enum builder_render_function func
  , const union builder_render_function_args * restrict args
  , narrator * restrict N
)
{
  enter;

  if(func == RENDER_PROPERTY)
  {
    fatal(node_property_say, args->n, args->prop, &args->pctx, N);
  }
  else if(func == RENDER_STRING)
  {
    fatal(narrator_xsays, N, args->s);
  }
  else if(func == RENDER_FORMULA_VALUE)
  {
    fatal(formula_value_render, args->f, N);
  }
  else if(func == RENDER_VALUE)
  {
    fatal(value_render, args->v, N);
  }
  else if(func == RENDER_FMT)
  {
    fatal(narrator_xsayvf, N, args->fmt, *args->va);
  }

  finally : coda;
}

xapi builder_add(exec_builder * restrict builder, const builder_add_args *args)
{
  enter;

  uint32_t def;
  uint32_t *defp;
  char * textbase;
  struct def_name_compare_context ctx;
  off_t pos;
  uint32_t start;
  uint32_t idx;
  size_t len;
  const char *eq;

  // current narrator position
  fatal(narrator_xseek, builder->Nexec, 0, NARRATOR_SEEK_CUR, &pos);
  start = pos;

  def = 0;
  if(args->item == PATH)
  {
    // locate an existing definition if any
    def = builder->path;

    // new definition goes here
    builder->path = start;
  }
  else if(args->item == ARGS)
  {
    // check for an existing definition
    if(args->position > -1)
    {
      def = builder->args[args->position];
      idx = args->position;
    }
    else
    {
      idx = builder->args_len++;
      fatal(assure, &builder->args, sizeof(*builder->args), idx + 1, &builder->args_alloc);
    }

    builder->args[idx] = start;
  }
  else if(args->item == ENVS)
  {
    // search for an existing definition
    memset(&ctx, 0, sizeof(ctx));
    ctx.lc = -1;
    if(args->position > -1)
    {
      def = builder->envs[args->position];
      ctx.lc = 0;
      ctx.lx = args->position;
    }
    else if(builder->envs && args->name)
    {
      textbase = narrator_growing_buffer(builder->Nexec);
      ctx.textbase = textbase;
      ctx.base = builder->envs;
      ctx.name = args->name;
      defp = bsearch(&ctx, builder->envs, builder->envs_len, sizeof(*builder->envs), def_name_compare);
      if(defp)
        def = *defp;
    }

    // displacement if no existing definition
    idx = ctx.lx;
    if(ctx.lc)
    {
      if(ctx.lc > 0)
        idx++;

      fatal(assure, &builder->envs, sizeof(*builder->envs), builder->envs_len + 1, &builder->envs_alloc);
      memmove(
          builder->envs + (idx + 1)
        , builder->envs + idx
        , (builder->envs_len - idx) * sizeof(*builder->envs)
      );

      builder->envs_len++;
    }

    builder->envs[idx] = start;
  }

  if(args->mode == PREPEND)
  {
    if(args->item == ENVS)
    {
      fatal(narrator_xsayw, builder->Nexec, args->name, args->name_len);
      fatal(narrator_xsays, builder->Nexec, "=");
    }

    // append the value from this definition
    fatal(dispatch_render_function, args->render_val, &args->val, builder->Nexec);

    if(def)
    {
      if(args->render_sep)
        fatal(dispatch_render_function, args->render_sep, &args->sep, builder->Nexec);

      // carry forward the existing definition
      textbase = narrator_growing_buffer(builder->Nexec);
      if(args->item == ENVS)
      {
        eq = strchr(textbase + def, '=');
        len = strlen(eq + 1);
        fatal(narrator_growing_allocate, builder->Nexec, len);
        textbase = narrator_growing_buffer(builder->Nexec);
        eq = strchr(textbase + def, '=');
        fatal(narrator_xsayw, builder->Nexec, eq + 1, len);
      }
      else
      {
        len = strlen(textbase + def);
        fatal(narrator_growing_allocate, builder->Nexec, len);
        textbase = narrator_growing_buffer(builder->Nexec);
        fatal(narrator_xsayw, builder->Nexec, textbase + def, len);
      }
    }

    fatal(narrator_xsayc, builder->Nexec, 0);
  }
  else if(args->mode == APPEND)
  {
    if(def)
    {
      // carry forward the existing definition
      textbase = narrator_growing_buffer(builder->Nexec);
      len = strlen(textbase + def);
      fatal(narrator_growing_allocate, builder->Nexec, len);
      textbase = narrator_growing_buffer(builder->Nexec);
      fatal(narrator_xsayw, builder->Nexec, textbase + def, len);
    }
    else if(args->item == ENVS)
    {
      fatal(narrator_xsayw, builder->Nexec, args->name, args->name_len);
      fatal(narrator_xsays, builder->Nexec, "=");
    }

    if(def && args->render_sep)
      fatal(dispatch_render_function, args->render_sep, &args->sep, builder->Nexec);

    // append the value from this definition
    fatal(dispatch_render_function, args->render_val, &args->val, builder->Nexec);
    fatal(narrator_xsayc, builder->Nexec, 0);
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}

//
// public
//

void exec_builder_take(exec_builder * restrict builder, exec ** restrict envp)
{
  narrator_growing_claim_buffer(builder->Nexec, &builder->exec, &builder->exec_size);

  *envp = builder->exec;
  builder->exec = 0;

  builder->path_stor = 0;
  builder->args_stor = 0;
  builder->args_stor_alloc = 0;
  builder->envs_stor = 0;
  builder->envs_stor_alloc = 0;
}

void exec_free(exec * e)
{
  if(e)
  {
    wfree(e->path);
    wfree(e->args);
    wfree(e->envs);
  }
  wfree(e);
}

void exec_ifree(exec ** restrict ep)
{
  if(*ep)
    exec_free(*ep);

  *ep = 0;
}

xapi exec_builder_xinit(exec_builder * restrict builder)
{
  enter;

  memset(builder, 0, sizeof(*builder));

  builder->Nexec = narrator_growing_init(builder->Nexec_stor);

  // position immediately after the env struct
  fatal(narrator_xseek, builder->Nexec, sizeof(exec), NARRATOR_SEEK_SET, 0);

  finally : coda;
}

xapi exec_builder_xreset(exec_builder * restrict builder)
{
  enter;

  builder->path = 0;
  builder->envs_len = 0;
  builder->args_len = 0;

  // position immediately after the env struct
  fatal(narrator_xseek, builder->Nexec, sizeof(exec), NARRATOR_SEEK_SET, 0);

  finally : coda;
}

xapi exec_builder_xdestroy(exec_builder * restrict builder)
{
  enter;

  fatal(narrator_xdestroy, builder->Nexec);
  wfree(builder->args);
  wfree(builder->args_stor);
  wfree(builder->envs);
  wfree(builder->envs_stor);

  finally : coda;
}

xapi exec_builder_build(exec_builder * restrict builder, exec ** restrict envp)
{
  enter;

  int x;
  char * textbase;

  textbase = narrator_growing_buffer(builder->Nexec);
  if(textbase == 0)
  {
    /* nothing has been added to the builder */
    fatal(narrator_growing_allocate, builder->Nexec, sizeof(exec));
    textbase = narrator_growing_buffer(builder->Nexec);
  }

  builder->exec = (typeof(builder->exec))textbase;
  RUNTIME_ASSERT(builder->exec);

  // path - pointer from offset
  if(builder->path == 0)
  {
    builder->exec->path = 0;
    builder->exec->filename = 0;
  }
  else
  {
    builder->path_stor = textbase + builder->path;
    builder->exec->path = builder->path_stor;
    if((builder->exec->filename = strrchr(builder->exec->path, '/')))
      builder->exec->filename++;
  }

  // args - space
  if(builder->args_stor_alloc < (builder->args_len + 1))
  {
    fatal(xrealloc, &builder->args_stor, sizeof(*builder->args_stor), builder->args_len + 1, builder->args_stor_alloc);
    builder->args_stor_alloc = builder->args_len + 1;
  }

  // args- pointers from offsets
  for(x = 0; x < builder->args_len; x++)
    builder->args_stor[x] = textbase + builder->args[x];
  builder->args_stor[x] = 0;

  builder->exec->args = builder->args_stor;
  builder->exec->args_size = builder->args_len;

  // envs - space
  if(builder->envs_stor_alloc < (builder->envs_len + 1))
  {
    fatal(xrealloc, &builder->envs_stor, sizeof(*builder->envs_stor), builder->envs_len + 1, builder->envs_stor_alloc);
    builder->envs_stor_alloc = builder->envs_len + 1;
  }

  // envs - pointers from offsets
  for(x = 0; x < builder->envs_len; x++)
    builder->envs_stor[x] = textbase + builder->envs[x];
  builder->envs_stor[x] = 0;

  builder->exec->envs = builder->envs_stor;
  builder->exec->envs_size = builder->envs_len;

  // return
  if(envp)
  {
    *envp = builder->exec;
  }

  finally : coda;
}

static xapi render_say(const char * restrict name, enum builder_render_function renderer, const union builder_render_function_args *args)
{
  enter;

  if(renderer == RENDER_PROPERTY)
  {
    xsayf("render-%s PROPERTY\n", name);
    xsayf(" node %p %s\n", args->n, args->n->name->name);
    xsayf(" prop %s\n", attrs32_name_byvalue(node_property_attrs, NODE_PROPERTY_OPT & args->prop));
  }
  else if(renderer == RENDER_STRING)
  {
    xsayf("render-%s STRING\n", name);
    xsayf(" string '%s'\n", args->s);
  }
  else if(renderer == RENDER_FORMULA_VALUE)
  {
    xsayf("render-%s FORMULA-VALUE\n", name);
    xsays(" formula-value ");
    fatal(formula_value_say, args->f, N);
    xsays("\n");
  }
  else if(renderer == RENDER_VALUE)
  {
    xsayf("render-%s VALUE\n", name);
    xsays(" value ");
    fatal(value_say, args->v, N);
    xsays("\n");
  }
  else if(renderer == RENDER_FMT)
  {
    xsayf("render-%s FMT\n", name);
    xsayf(" fmt %s\n", args->fmt);
    xsayf(" va %p\n", args->va);
  }

  finally : coda;
}

xapi builder_add_args_say(const exec_builder * restrict builder, const builder_add_args * restrict args, narrator * restrict N)
{
  enter;

  xsayf("builder %p\n", builder);
  xsayf("item %s\n", args->item == PATH ? "PATH" : args->item == ARGS ? "ARGS" : args->item == ENVS ? "ENVS" : "-wtf-");
  xsayf("position %d\n", args->position);
  xsayf("name %.*s\n", (int)args->name_len, args->name);
  xsayf("name-len %hu\n", args->name_len);
  xsayf("mode %d %s\n", args->mode, args->mode == PREPEND ? "PREPEND" : args->mode == APPEND ? "APPEND" : "-wtf-");
  fatal(render_say, "value", args->render_val, &args->val);
  fatal(render_say, "sep", args->render_sep, &args->sep);

  finally : coda;
}
