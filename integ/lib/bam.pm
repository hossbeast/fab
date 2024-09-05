# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

use File::Basename;
use Data::Dumper;
use Time::HiRes qw|nanosleep|;

use strict;
use warnings;
use bigint qw|hex|;

our $repodir;
our $builddir = '/home/todd/bam-build';
our $bam_rundir = $ENV{XDG_RUNTIME_DIR} . "/bam";              # /run/user/1000/bam
our $bam_integ_rundir = $ENV{XDG_RUNTIME_DIR} . "/bam-integ";  # /run/user/1000/bam-integ
our $bam_bampath;
our $bam_name;          # test-module-unref-use+stat
our $bam_basedir;       # /run/user/1000/bam-integ/testformulamanyref+stat
our $bam_basehash;      # 35f92367872d6ecc                      (base)
our $bam_baseipc;       # /run/user/1000/bam/35f92367872d6ecc
our $bam_projdir;       # /run/user/1000/bam-integ/testformulamanyref+stat/B
our $bam_projhash;      # d5c90b82ddcd99bb                      (proj)
our $bam_projipc;       # /run/user/1000/bam/d5c90b82ddcd99bb
our $bam_ipcdir;        # /run/user/1000/bam/d5c90b82ddcd99bb   (proj)
our $bam_invalidate;
our $bam_sweeper_period = 250 * 1000 * 1000;  # 250 ms

my $ff64 = hex("0xffffffffffffffff");

# compatible with libcommon/hash/hash.c
# requires bigint
sub hash64
{
  my $s = shift;
  my $h = 0;
  for my $c (unpack("C*", $s)) {
    $h = ($h + $c) & $ff64;
    $h = ($h + ($h << 13)) & $ff64;
    $h = ($h + ($h >> 3)) & $ff64;
  }
  $h = ($h + ($h << 3)) & $ff64;
  $h = ($h ^ ($h >> 23)) & $ff64;
  $h = ($h + ($h << 34)) & $ff64;

  sprintf("%016x", $h);
}

sub test_parameters
{
  my $name = shift;

  $name = basename($name);
  $name =~ s/[^a-zA-Z0-9+]//g;

  my $dir = "$bam_integ_rundir/$name";
  my $hash = hash64($dir);
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

sub init
{
  my %params = @_;

  permissions_check();

  $bam_bampath = "$builddir/bam/devel/bam";
  $bam_invalidate = $ARGV[0] || "stat";
  $bam_name = basename($0) . "+$bam_invalidate";
  ($bam_basedir, $bam_basehash, $bam_baseipc) = test_parameters($bam_name);
  if($params{subdir}) {
    $bam_projdir .= "$bam_basedir/$params{subdir}";
  } else {
    $bam_projdir = $bam_basedir;
  }
  $bam_projhash = hash64($bam_projdir);
  $bam_projipc = "$bam_rundir/$bam_projhash";

  $bam_ipcdir = $bam_projipc;

  printf("%10s %s %s\n", "base", $bam_basehash, $bam_basedir);
  printf("%10s %s %s\n", "proj", $bam_projhash, $bam_projdir);
  printf("%10s %s %s\n", "ipcdir", $bam_projhash, $bam_ipcdir);
  printf("%10s %s %s/bamd/stdout\n", "out", $bam_projhash, $bam_ipcdir);
  printf("%10s %s %s/bamd/stderr\n", "err", $bam_projhash, $bam_ipcdir);
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
  mkdirp($bam_baseipc);

  # proj ipc links to base ipc
  if($bam_baseipc ne $bam_projipc) {
    symlinkf($bam_baseipc, $bam_projipc);
  }

  # build out the basedir
  tree_build($bam_basedir, $spec);
  bam_bootstrap(%$args);

  return $bam_basedir;
}

sub teardown
{
  run({ cwd => $bam_projdir }
    , $bam_bampath
    , "--no-launch"
    , "kill"
  );

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
  return if $exit == 0 && length($err) == 0;

  print("> $bam_ipcdir/bamd/exit\n$exit\n");
  print("> $bam_ipcdir/bamd/stderr\n$err\n");
  die;
}

sub reconcile
{
  my %args = @_;
  if($bam_invalidate eq "notify")
  {
    printf("sleep " . (($bam_sweeper_period * 4)/1000/1000/1000) . "\n");
    nanosleep($bam_sweeper_period * 4);
  }
  bam_reconcile(%args);
}

sub build
{
  my $result = bam_build(@_);
  if($bam_invalidate eq "notify")
  {
    printf("sleep " . (($bam_sweeper_period * 4)/1000/1000/1000) . "\n");
    nanosleep($bam_sweeper_period * 4);
  }
  return $result;
}

sub bam_bootstrap
{
  run({ @_, cwd => $bam_projdir }, $bam_bampath
    , "reconcile"
    , "-K"
    , "--system-config-path", "$bam_basedir/system-config"
    , "--user-config-path", "$bam_basedir/user-config"
    , "--default-filesystem-invalidation", $bam_invalidate
    , "--sweeper-period-nsec", $bam_sweeper_period
  );

  my $result = bam_tree([$bam_basedir, "--no-module"]);
  print("$$result{out}\n");
}

sub bam_reconcile
{
  run({ @_, cwd => $bam_projdir }
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

  my $result = run({ cwd => $bam_projdir }
    , $bam_bampath
    , @args
  );
}

sub bam_invalidate_primary
{
  my $name = shift;
  my @args = @_;

  unshift @args, "--no-launch", "adhoc", "invalidate-primary";
  unshift @args, "-m", ".";
  push @args, $name if $name;

  my $result = run({ cwd => $bam_projdir }
    , $bam_bampath
    , @args
  );
}

sub bam_stats
{
  my $items = pop;
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
    unshift @args, "//system", "--no-module";
  }
  $result = bam_describe(\@args);
  # there can be multiple values - parse in set context
  $msg = bacon_parse("{" . $$result{out} . "}");

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

  my $result = run({ @_, cwd => $bam_projdir }
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

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "fstrees"
    , @$arglist
  );
}

sub bam_info
{
  my $arglist = shift;

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "info"
    , @$arglist
  );
}

sub bam_describe
{
  my $arglist = shift;

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "describe"
    , @$arglist
  );
}

sub bam_ls
{
  my $arglist = shift;

  run({ cwd => $bam_projdir, @_ }
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

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_bampath
    , "--no-launch"
    , "--no-color"
    , "tree"
    , @$arglist
  );
}

sub bam_modtree
{
  run({ @_, cwd => $bam_projdir }
    , $bam_bampath
    , "--no-launch"
    , "tree"
    , "-g"
    , "modtree"
  );
}

sub bam_build
{
  run({ @_, cwd => $bam_projdir }
    , $bam_bampath
    , "--no-launch"
    , "build"
  );
}

sub bam_module_vars
{
  my $res = run({ @_, cwd => $bam_projdir }
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
  my $res = run({ @_, cwd => $bam_projdir }
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
  run({ @_, cwd => $bam_projdir }
    , $bam_bampath
    , "--no-launch"
    , "config"
  );
}

1
