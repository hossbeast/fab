/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <getopt.h>

#include "types.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "args.h"
#include "common/parseint.h"
#include "common/hash.h"
#include "filesystem.h"
#include "common/attrs.h"
#include "git-state.h"
#include "times.h"
#include "fstree.h"

struct g_args_t g_args;
char *g_argv_base_dir_space;

#if CONFIG_DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/config.bam+devel"
#define USER_CONFIG_PATH      "$HOME/.bam/config.bam+devel"
#else
#define SYSTEM_CONFIG_PATH    "/etc/config.bam"
#define USER_CONFIG_PATH      "$HOME/.bam/config.bam"
#endif

static void args_version()
{
  printf("bamd - bam daemon %s\n\n%s\n", git_describe, git_metadata);

  exit(0);
}

static void args_usage()
{
  printf(
"bamd - bam daemon %s\n"
"\n"
"usage: bamd [option] <hash>\n"
"\n"
"bam daemon for directory indicated by <hash>\n"
"hash is computed from cwd if not specified\n"
"bamd is normally started on demand by the bam client\n"
"\n"
"options\n"
" --help | -h       print this message\n"
" --version | -V    print version information\n"
"\n"
"build optimally      https://git.sr.ht/~hossbeast/bam\n"
    , git_describe
  );

  exit(0);
}

static void args_parse(int argc, char ** restrict argv)
{
  char *pwd = 0;
  int x;
  int longindex;
  int version = 0;
  int help = 0;
  int len;

  const struct option longopts[] = {
      { "help"     , no_argument , &help, 1 }
    , { "version"  , no_argument , &version, 1 }
#if CONFIG_DEVEL
    , { "system-config-path"              , required_argument, 0, 0 }
    , { "user-config-path"                , required_argument, 0, 0 }
    , { "default-filesystem-invalidation" , required_argument, 0, 0 }
    , { "sweeper-period-nsec"             , required_argument, 0, 0 }
#endif
    , { }
  };

  const char *switches = "hV";

  // disable getopt error messages
  opterr = 0;
  optind = 0;
  while((x = getopt_long(argc, argv, switches, longopts, &longindex)) != -1)
  {
#if CONFIG_DEVEL
    if(x == 0 && strcmp(longopts[longindex].name, "system-config-path") == 0) {
      ifree(&g_args.system_config_path);
      ixstrdup(&g_args.system_config_path, optarg);
    } else if(x == 0 && strcmp(longopts[longindex].name, "user-config-path") == 0) {
      ifree(&g_args.user_config_path);
      ixstrdup(&g_args.user_config_path, optarg);
    } else if(x == 0 && strcmp(longopts[longindex].name, "default-filesystem-invalidation") == 0) {
      if((g_args.default_filesystem_invalidation = attrs16_value_byname(invalidation_attrs, optarg)) == 0) {
        fprintf(stderr, "invalid filesystem-invalidation %s\n", optarg);
        exit(1);
      }
    } else if(x == 0 && strcmp(longopts[longindex].name, "sweeper-period-nsec") == 0) {
      if(parseuint(optarg, SCNu64, 1, UINT64_MAX, 1, 20, &g_args.sweeper_period_nsec, 0) != 0) {
        fprintf(stderr, "invalid sweeper-period-nsec %s\n", optarg);
        exit(1);
      }
    }
#endif
    if(x == 'h') {
      help = 1;
    } else if(x == 'V') {
      version = 1;
    }
  }

  if(help)
  {
    args_usage();
  }
  if(version)
  {
    args_version();
  }

  if(optind == argc) {
    pwd = xrealpaths(0, getenv("PWD"));
    g_args.hash = hash64(0, pwd, strlen(pwd));
  }
  /* optional first argument - hash */
  if(optind < argc) {
    if(parseuint(argv[optind], SCNx64, 1, UINT64_MAX, 1, UINT8_MAX, &g_args.hash, 0) != 0) {
      fprintf(stderr, "invalid hash %s\n", argv[optind]);
      exit(1);
    }
    optind++;
  }
  /* optional second argument - space for displaying basedir */
  if(optind < argc) {
    len = strlen(argv[optind]);
    for(x = 0; x < len; x++) {
      if(argv[optind][x] != 'x') {
        break;
      }
    }
    if(x == 100) {
      g_argv_base_dir_space = argv[optind];
      optind++;
    }
  }
  if(optind < argc) {
    fprintf(stderr, "unexpected argument %s\n", argv[optind]);
    exit(1);
  }

  free(pwd);
}

void args_init()
{
  g_args.default_filesystem_invalidation = FILESYSTEM_INVALIDATION_DEFAULT;
  g_args.default_filesystem_index = FILESYSTEM_INDEX_DEFAULT;
  ixstrdup(&g_args.system_config_path, SYSTEM_CONFIG_PATH);
  ixstrdup(&g_args.user_config_path, USER_CONFIG_PATH);
  g_args.sweeper_period_nsec = MSEC_AS_NSEC(250);
}

void args_setup(int argc, char ** argv)
{
  char space[64];

  args_init();
  args_parse(argc, argv);

  snprintf(space, sizeof(space), "%016"PRIx64, g_args.hash);
  ixstrdup(&g_args.hash_str, space);
}

void args_teardown()
{
  free(g_args.system_config_path);
  free(g_args.user_config_path);
  free(g_args.hash_str);
}
