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
  char id[40];
  uint16_t id_len;
  char prefix[64];
  const char *visit;

  bool nofollow;          // on target lookup
  bool hidden;

  bool any;
  char path[255];

  const char *targets[64];
  uint16_t targets_lens[64];
  uint16_t targets_len;

  char *target;           // path to resolve the module of
  uint16_t target_len;

  int special;
  const char *special_target;
  uint16_t special_target_len;

  char fstree_id[40];
  uint16_t fstree_id_len;

  char rule_id[40];
  uint16_t rule_id_len;
  const char *rule_name;
  uint16_t rule_name_len;

  char rma_id[40];
  uint16_t rma_id_len;
  const char *rma_name;
  uint16_t rma_name_len;

  char namespace_id[40];
  uint16_t namespace_id_len;
  const char *namespace_name;
  uint16_t namespace_name_len;

  char build_record_id[40];
  uint16_t build_record_id_len;
  uint16_t build_record_order;

  char fsname_id[40];
  uint16_t fsname_id_len;
  const char *fsname_name;
  uint16_t fsname_name_len;

  const char *show_detail;
  uint16_t show_detail_len;

  /* runtime state */

  /* command ids */
  uint64_t request_id;
  uint64_t list_id;
  uint64_t tell_id;
  uint64_t identify_id;
  uint64_t run_id;
  uint64_t describe_id;
  uint64_t module_describe_id;
  uint64_t identify_id_0;
  uint64_t identify_id_1;
  uint64_t identify_id_2;
  bool reset;
  bool trace;
} command;

extern struct command *g_cmd;

typedef void (command_builder)(struct command * restrict cmd, struct bacon_writer * restrict builder_writer);

uint64_t command_post(struct command * restrict cmd, struct bam_client * restrict client, command_builder *builder)
  __attribute__((nonnull));

void command_context(struct bacon_writer * restrict builder_writer)
  __attribute__((nonnull));

size_t command_identify_result_parse(char *dst, size_t dst_size, struct bamipc_message * restrict msg)
  __attribute__((nonnull))
  __attribute__((warn_unused_result));

void command_set(struct command * restrict cmd)
  __attribute__((nonnull));

void command_sub_set(struct command * restrict cmd)
  __attribute__((nonnull));

//
//
//

struct bamipc_message *client_tx(struct bam_client * restrict client)
  __attribute__((nonnull));

void client_publish(struct bam_client * restrict client, struct bamipc_message * restrict msg)
  __attribute__((nonnull));

int client_rx(struct bam_client * restrict client, struct timespec * restrict timeout, struct bamipc_message ** restrict msg)
  __attribute__((nonnull));

void client_ping(struct bam_client * restrict client, uint64_t msgid)
  __attribute__((nonnull));

void client_pong(struct bam_client * restrict client, uint64_t msgid)
  __attribute__((nonnull));
