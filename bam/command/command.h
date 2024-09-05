/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "args.h"

struct bam_client;
struct bamipc_message;
struct narrator;
struct bacon_writer;

#define COMMAND_OPT               0xffff
#define COMMAND_ATTRS_TABLE               \
  DEF(COMMAND_CONTEXT_MODULE,     0x1)    \

typedef enum command_opt {
#undef DEF
#define DEF(x, y) x = y,
COMMAND_ATTRS_TABLE
} command_opt;

extern struct attrs32 command_attrs;

extern uint64_t message_no_error_list[8];
extern uint16_t message_no_error_list_len;

typedef struct command {
  char *name;
  void (*usage)(struct command *cmd);
  void __attribute__((nonnull)) (*attached)(struct command *cmd, struct bam_client * restrict client);
  void __attribute__((nonnull)) (*connected)(struct command *cmd, struct bam_client * restrict client);
  void __attribute__((nonnull)) (*process)(
      struct command *cmd
    , struct bam_client * restrict client
    , struct bamipc_message * restrict msg
  );
  void __attribute__((nonnull)) (*disconnected)(struct command *cmd);

  /* above this field, overwritten by subcommand */
  bool _sub_command_state;

  char module_id[40];
  uint16_t module_id_len;

  /* arguments */
  char prefix[64];
  uint64_t id;
  const char *visit;

  bool any;
  char path[255];

  uint64_t ids[64];
  uint16_t ids_len;

  bead targets[64];
  uint16_t targets_len;

  char *target;           // path to resolve the module of
  uint16_t target_len;

  int special;
  bead special_target;

  char fstree_id[40];
  uint16_t fstree_id_len;

  uint64_t request_id;
  uint64_t list_id;
  uint64_t identify_id;
  uint64_t describe_id;
  uint64_t module_describe_id;
  bool context_module;
  uint64_t tell_id;
//  uint64_t module_identify_id;
  uint64_t identify_id_0;
  uint64_t identify_id_1;
  bool reset;
} command;

extern struct command *g_cmd;

typedef void (command_builder)(struct command * restrict cmd, struct bacon_writer * restrict builder_writer);

uint64_t command_post(struct command * restrict cmd, struct bam_client * restrict client, command_builder *builder)
  __attribute__((nonnull));

void client_post(struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

void command_context(struct command * restrict cmd, struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

#define CASE2(x, y) ((((uint32_t)x) << 16) | ((uint32_t)y))

size_t command_identify_result_parse(struct bamipc_message * restrict msg, char *dst, size_t dst_size)
  __attribute__((nonnull))
  __attribute__((warn_unused_result));

void command_set(struct command * restrict cmd)
  __attribute__((nonnull));

void command_sub_set(struct command * restrict cmd)
  __attribute__((nonnull));
