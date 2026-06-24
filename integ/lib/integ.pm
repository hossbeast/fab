# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

use File::Basename;
use Data::Dumper;
use Time::HiRes qw|nanosleep|;
use bam::bam;

use strict;
use warnings;

our $repodir;
our $bam_rundir = $ENV{XDG_RUNTIME_DIR} . "/bam";              # /run/user/1000/bam
our $bam_integ_rundir = $ENV{XDG_RUNTIME_DIR} . "/bam-integ";  # /run/user/1000/bam-integ
our $bam_bampath;       # /home/todd/bam-build/bam/devel/bam
our $bam_name;          # test-module-unref-use+stat
our $bam_basedir;       # /run/user/1000/bam-integ/testformulamanyref+stat
our $bam_base_bamd_id;  # 35f92367872d6ecc                      (base)
our $bam_base_ipcdir;   # /run/user/1000/bam/35f92367872d6ecc   (base)
our $bam_module_id;     #
our $bam_dir;           # /run/user/1000/bam-integ/testformulamanyref+stat/B
our $bam_bamd_id;       # d5c90b82ddcd99bb
our $bam_ipcdir;        # /run/user/1000/bam/d5c90b82ddcd99bb
our $bam_invalidate;
our $bam_sweeper_period = 250 * 1000 * 1000;  # 250 ms

sub test_parameters
{
  my $name = shift;

  $name = basename($name);
  $name =~ s/[^a-zA-Z0-9%]//g;

  my $dir = "$bam_integ_rundir/$name";
  my $hash = bam::identity_base($dir);
  my $ipc = "$bam_rundir/$hash";

  return $dir, $hash, $ipc;
}

sub permissions_check
{
  return if(-e "$bam_integ_rundir/logs");

  print STDERR ("mkdir -p $bam_integ_rundir/logs\n");
  print STDERR ("forgot?\n");
  exit(1);
}

sub env_check
{
  return if exists $ENV{bm_bam_path};

  print STDERR ("export bm_bam_path=/home/todd/bam-build/bam/devel/bam\n");
  print STDERR ("forgot?\n");
  exit(1);
}

sub init
{
  my %params = @_;

  my $fd;

  permissions_check();
  env_check();

  $bam_bampath = $ENV{bm_bam_path};
  $bam_invalidate = $ARGV[0] || "stat";
  $bam_name = basename($0) . '%' . $bam_invalidate;
  ($bam_basedir, $bam_base_bamd_id, $bam_base_ipcdir) = test_parameters($bam_name);
  if($params{subdir}) {
    $bam_dir .= "$bam_basedir/$params{subdir}";
  } else {
    $bam_dir = $bam_basedir;
  }
  $bam_bamd_id = bam::identity_base($bam_dir);
  $bam_module_id = bam::identity("module", $bam_dir);
  $bam_ipcdir = "$bam_rundir/$bam_bamd_id";

  # arrange for bamd to also write to the log
  system("mkdir -p $bam_base_ipcdir/bamd");
  uxunlink("$bam_base_ipcdir/log");
  uxunlink("$bam_base_ipcdir/bamd/stdout");

  $fd = POSIX::open("$bam_base_ipcdir/log", O_WRONLY | O_CREAT | O_TRUNC) or die $!;
  POSIX::close($fd);
  $fd = POSIX::open("$bam_base_ipcdir/bamd/stdout", O_WRONLY | O_CREAT | O_TRUNC) or die $!;
  POSIX::close($fd);

  # tail from the log to old-stdout
  if(fork() == 0) {
    # kill this tail automatically when the test ends
    pr_set_pdeathsig(15);

    my @cmd = ("/usr/bin/tail", "-f", "$bam_base_ipcdir/log");
    exec { $cmd[0] } @cmd;
    die "WTF";
  }

  # tail from bamd/stdout to the log
  if(fork() == 0) {
    # kill this tail automatically when the test ends
    pr_set_pdeathsig(15);

    POSIX::close(1);
    POSIX::open("$bam_base_ipcdir/log", O_WRONLY | O_APPEND) or die $!;

    my @cmd = ("/usr/bin/tail", "-F", "$bam_base_ipcdir/bamd/stdout");
    exec { $cmd[0] } @cmd;
    die "WTF";
  }

  # tail from bamd/stderr to the log
  if(fork() == 0) {
    # kill this tail automatically when the test ends
    pr_set_pdeathsig(15);

    POSIX::close(1);
    POSIX::open("$bam_base_ipcdir/log", O_WRONLY | O_APPEND) or die $!;

    my @cmd = ("/usr/bin/tail", "-F", "$bam_base_ipcdir/bamd/stderr");
    exec { $cmd[0] } @cmd;
    die "WTF";
  }

  # redirect test stdout to the log
  POSIX::close(1);
  POSIX::open("$bam_base_ipcdir/log", O_WRONLY | O_APPEND) or die $!;

  # redirect test stderr to the log
  POSIX::dup2(1, 2) or die $!;

  printf("%10s %s %s\n", "base", $bam_base_bamd_id, $bam_basedir);
  printf("%10s %s %s\n", "proj", $bam_bamd_id, $bam_dir);
  printf("%10s %s %s\n", "ipcdir", $bam_bamd_id, $bam_ipcdir);
  printf("%10s %s %s/bamd/stdout\n", "out", $bam_bamd_id, $bam_ipcdir);
  printf("%10s %s %s/bamd/stderr\n", "err", $bam_bamd_id, $bam_ipcdir);
  printf("%10s %s %s/bamd/objects\n", "objects", $bam_bamd_id, $bam_ipcdir);
}

sub setup
{
  my $args;
  my $spec;
  if($#_ == 1) {
    $args = shift;
    $spec = shift;
  } else {
    $args = { };
    $spec = shift;
  }

  # setup the base ipc dir
  mkdirp($bam_base_ipcdir);

  # proj ipc links to base ipc
  if($bam_dir ne $bam_basedir) {
    symlinkf($bam_base_ipcdir, $bam_ipcdir);
  }

  # build out the basedir
  tree_build($bam_basedir, $spec);
  bam_bootstrap(%$args);

  return $bam_basedir;
}

sub teardown
{
  run({ cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "kill"
  );

  my $objects = '';

  # snarf the best-effort objects counts
  my $leaks = '';
  if(open my $fh, "<", "$bam_ipcdir/bamd/objects")
  {
    while((my $line = <$fh>))
    {
      chomp $line;
      die "WTF" unless($line =~ /^([_a-zA-Z][-_a-zA-Z0-09]*) +([0-9]+)$/);
      my $count = int($2);
      $objects .= "$line\n";
      $leaks .= "$line\n" if $count > 0;
    }
    close $fh;
  }

  # snarf the best-effort exit status file
  my $exit = -1;
  if(open my $fh, "<", "$bam_ipcdir/bamd/exit")
  {
    my $line = <$fh>;
    chomp $line;
    close $fh;
    $exit = int($line);
  }

  my $err = '';
  if(open my $fh, "<", "$bam_ipcdir/bamd/stderr")
  {
    $err = do { local $/ = undef; <$fh> };
    chomp $err;
    close $fh;
  }

  # must have written zero to its exit file
  # must not have written anything to its stderr file
  # must have cleaned up all resources
  return if $exit == 0 && length($err) == 0 && length($leaks) == 0;

  print("> $bam_ipcdir/bamd/exit\n$exit\n");
  print("> $bam_ipcdir/bamd/stderr\n$err\n");
  print("> $bam_ipcdir/bamd/objects\n$leaks\n");
  die;
}

sub reconcile
{
  my %args = @_;
  if($bam_invalidate eq "notify")
  {
    my $duration = $bam_sweeper_period * 3;
    printf("sleep $duration nsec\n" );
    nanosleep($duration);
  }
  bam_reconcile(%args);
}

sub build
{
  my $result = run({ @_, cwd => $bam_dir, show => 1 }
    , $bam_bampath
    , "--no-launch"
    , "build"
  );

  if($bam_invalidate eq "notify")
  {
    my $duration = $bam_sweeper_period * 3;
    printf("sleep $duration nsec\n" );
    nanosleep($duration * 4);
  }
  return $result;
}

sub bam_bootstrap
{
  my $result;

  run({ @_, cwd => $bam_dir }, $bam_bampath
    , "reconcile"
    , "-K"
    , "--system-config-path", "$bam_basedir/system-config"
    , "--user-config-path", "$bam_basedir/user-config"
    , "--default-filesystem-invalidation", $bam_invalidate
    , "--sweeper-period-nsec", $bam_sweeper_period
  );

  $result = bam_tree([$bam_basedir, "--no-module"]);
  print("$$result{out}\n");

  $result = bam_tree(["/", "--no-module"]);
  print("$$result{out}\n");
}

sub bam_reconcile
{
  run({ @_, cwd => $bam_dir} # , show => 1 }
    , $bam_bampath
    , "--no-launch"
    , "reconcile"
  );
}

sub bam_touch
{
  my $name = shift;
  my @args = @_;

  unshift @args, "--no-launch", "touch";
  unshift @args, "-m", ".";
  push @args, $name if $name;

  run({ cwd => $bam_dir }
    , $bam_bampath
    , @args
  );
}

sub bam_debug
{
  my $name = shift;
  my @args = @_;

  unshift @args, "--no-launch", "adhoc", "--", "plugin-ready";
  unshift @args, "-m", ".";
  push @args, $name if $name;

  my $result = run({ cwd => $bam_dir }
    , $bam_bampath
    , @args
  );
}

sub bam_adhoc
{
  my @args = @_;

  unshift @args, "--no-launch", "adhoc", "--";
  unshift @args, "-m", ".";

  my $result = run({ cwd => $bam_dir }
    , $bam_bampath
    , @args
  );
}

sub bam_goals
{
  my $res;
  my $msg;
  my $goals;

  $res = bam_describe(["//bm/sys-goals", "--follow"]);
  $msg = bam::bacon::parse($$res{"out"});
  $goals = $$msg{"//bm/sys-goals"};

  return $goals;
}

sub bam_stats
{
  my @args = @_;
  my $entity = undef;
  if($#args >= 0) {
    $entity = $args[0];
  }

  # special case for no target ; fetch global stats
  my $result;
  my $msg;
  my $stats = {};
  if (!$entity) {
    unshift @args, "//bm/sys-system", "--no-module", "--follow";
  }
  $result = bam_describe(\@args);
  # there can be multiple values - parse in set context
  $msg = bam::bacon::parse("{" . $$result{out} . "}");

  for my $rec (values %$msg) {
    while(my($k, $v) = each %{$$rec{stats}}) {
      $$stats{$k} = $v unless defined $$stats{$k};
    }
    $$stats{type} = $$rec{type} unless defined $$stats{type};
    $$stats{state} = $$rec{state} unless defined $$stats{state};
  }

  return $stats;
}

sub bam_metadata
{
  # fetch metadata
  my %metadata;

  my $result = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "metadata"
  );
  my $text = $$result{out};

  for my $line (split(/\n/, $text))
  {
    next unless $line =~ /^ *([-_a-z0-9]+) : (.*)/;
    $metadata{$1} = $2;
  }

  \%metadata;
}

sub bam_fstrees
{
  my $arglist = shift;

  run({ @_, cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "fstrees"
    , @$arglist
  );
}

sub bam_info
{
  my $arglist = shift;

  run({ @_, cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "info"
    , @$arglist
  );
}

sub bam_describe
{
  my $arglist = shift;

  run({ @_, cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "describe"
    , @$arglist
  );
}

sub bam
{
  my $arglist = shift;

  run({ cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "--no-color"
    , @$arglist
  );
}

sub bam_plugins
{
  my $res = run({ @_, cwd => $bam_dir, show => 0 }
    , $bam_bampath
    , "--no-launch"
    , "plugins"
  );

  return $$res{out};
}

sub bam_plugin
{
  my $res = run({ cwd => $bam_dir, show => 0 }
    , $bam_bampath
    , "--no-launch"
    , "plugin"
    , @_
  );

  return $$res{out};
}

sub bam_ls
{
  my $arglist = shift;

  run({ cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "--no-color"
    , "ls"
    , @$arglist
  );
}

sub bam_tree
{
  my $arglist = shift;

  run({ @_, cwd => $bam_dir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "--no-color"
    , "tree"
    , @$arglist
  );
}

sub bam_modtree
{
  run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "tree"
    , "-g"
    , "modtree"
  );
}

sub bam_build
{
  run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "build"
  );
}

sub bam_varsets
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "varsets"
  );

  return $$res{out};
}

sub bam_formulas
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "formulas"
  );

  return $$res{out};
}

sub bam_configsets
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "configsets"
  );

  return $$res{out};
}

sub bam_rules
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "rules"
  );

  return $$res{out};
}

sub bam_rule
{
  my $res = run({ cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "rule"
    , @_
  );

  return $$res{out};
}

sub bam_rmas
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "rmas"
  );

  return $$res{out};
}

sub bam_rma
{
  my $res = run({ cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "rma"
    , @_
  );

  return $$res{out};
}

sub bam_rulesets
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "rulesets"
  );

  return $$res{out};
}

sub bam_module_rulesets
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "module"
    , "rulesets"
  );

  return $$res{out};
}

sub bam_modules
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "modules"
  );

  return $$res{out};
}

sub bam_module_vars
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "module"
    , "vars"
  );

  my $text = $$res{out};
  my @lines;
  for my $line (split(/\n/, $text))
  {
    next if $line =~ /^#/;
    push @lines, $line;
  }

  join("\n", @lines);
}

sub bam_module_envs
{
  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "module"
    , "envs"
  );

  my $text = $$res{out};
  my @lines;
  for my $line (split(/\n/, $text))
  {
    next if $line =~ /^#/;
    push @lines, $line;
  }

  join("\n", @lines);
}

sub bam_config
{
  run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "config"
  );
}

sub bam_namespace_fsnames
{
  my $ns_id = shift;

  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "namespace"
    , $ns_id
    , "fsnames"
  );

  my $text = $$res{out};
  my %names;
  for my $line (split(/\n/, $text))
  {
    next unless $line =~ /fsname id : (?:bm1[qpzry9x8gf2tvdw0s3jn54khce6mua7l]{7,}) namespace : (?:bm1[qpzry9x8gf2tvdw0s3jn54khce6mua7l]{7,}) label : ([a-zA-Z0-9]+)/;
    $names{$1} = 1;
  }

  return \%names;
}

sub bam_list
{
  my $target = shift;

  my $res = run({ @_, cwd => $bam_dir }
    , $bam_bampath
    , "--no-launch"
    , "ls"
    , $target
  );

  my $text = $$res{out};
  my @lines = split(/\n/, $text);
  return \@lines;
}

1
