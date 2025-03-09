/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <stdint.h>

#include "descriptor.h"

extern descriptor_type descriptor_bam_build_string;
extern descriptor_type descriptor_bam_build_slot_results;
extern descriptor_type descriptor_bam_build_slot_info;

/* BAMIPC_EVENT_FORMULA_EXEC_FORKED */
typedef struct bam_build_slot_info {
  int32_t pid;            // process id
  uint16_t stage;         // slots in the same stage can execute concurrently

  /* bpe being executed */
  char *bpe_id;
  uint16_t bpe_id_len;
  char *bpe_path;
  uint16_t bpe_path_len;

  /* rma which created the bpe */
  char *rma_id;
  uint16_t rma_id_len;
  char *rma_path;
  uint16_t rma_path_len;

  /* rule attached to the rma */
  char *rule_id;
  uint16_t rule_id_len;
  char *rule_path;
  uint16_t rule_path_len;

  /* module attached to the rma */
  char *module_id;
  uint16_t module_id_len;
  char *module_path;
  uint16_t module_path_len;

  /* formula attached to the rma */
  char *formula_id;
  uint16_t formula_id_len;
  char *formula_path;
  uint16_t formula_path_len;

  /* file being executed, either the formula, or the resolve file parameter */
  char *exec_path;            // absolute path
  uint16_t exec_path_len;

  /* normally this is the module dir */
  char *cwd;                  // absolute path
  uint16_t cwd_len;

  /* struct bam_build_string */
  uint16_t arg_list_count;    // total number of items in arg_list
  void *arg_list;
  uint32_t arg_list_size;     // size of the arg_list blob in this message
  uint16_t arg_list_len;      // items in arg_list in this message

  /* struct bam_build_string */
  uint16_t env_list_count;
  void *env_list;
  uint32_t env_list_size;
  uint16_t env_list_len;

  /* struct bam_build_string */
  uint16_t source_list_count;
  void *source_list;
  uint32_t source_list_size;
  uint16_t source_list_len;

  /* struct bam_build_string */
  uint16_t target_list_count;
  void *target_list;
  uint32_t target_list_size;
  uint16_t target_list_len;
} bam_build_slot_info;

/* bam_build_slot_info strings */
/* BAMIPC_EVENT_FORMULA_EXEC_ARG */
/* BAMIPC_EVENT_FORMULA_EXEC_ENV */
typedef struct bam_build_string {
  uint16_t text_len;
  const char *text;
} bam_build_string;

/* BAMIPC_EVENT_FORMULA_EXEC_WAITED */
typedef struct bam_build_slot_results {
  int32_t status;         // process exit status
  uint64_t runtime;       // time between fork and wait, nanoseconds
  uint32_t stdout_total;
  uint32_t stderr_total;
  uint32_t auxout_total;
} bam_build_slot_results;
