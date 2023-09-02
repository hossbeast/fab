# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

use File::Basename;
use Data::Dumper;
use Time::HiRes qw|nanosleep|;

use strict;
use warnings;
use bigint qw|hex|;

our $repodir;
our $bam_fabpath;
our $bam_name;          # test-module-unref-use+stat
our $bam_basedir;       # /run/fab-integ/testformulamanyref+stat
our $bam_basehash;      # 35f92367872d6ecc              (base)
our $bam_baseipc;       # /var/run/fab/35f92367872d6ecc
our $bam_projdir;       # /run/fab-integ/testformulamanyref+stat/B
our $bam_projhash;      # d5c90b82ddcd99bb              (proj)
our $bam_projipc;       # /var/run/fab/d5c90b82ddcd99bb
our $bam_ipcdir;        # /var/run/fab/d5c90b82ddcd99bb (proj)
our $bam_invalidate;
our $bam_sweeper_period = 250 * 1000 * 1000;

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

  my $dir = "/run/fab-integ/$name";
  my $hash = hash64($dir);
  my $ipc = "/var/run/fab/$hash";

  return $dir, $hash, $ipc;
}

sub init
{
  my %params = @_;

  $bam_fabpath = "$repodir/fab/fab/fab.devel";
  $bam_invalidate = $ARGV[0] || "stat";
  $bam_name = basename($0) . "+$bam_invalidate";
  ($bam_basedir, $bam_basehash, $bam_baseipc) = test_parameters($bam_name);
  if($params{subdir}) {
    $bam_projdir .= "$bam_basedir/$params{subdir}";
  } else {
    $bam_projdir = $bam_basedir;
  }
  $bam_projhash = hash64($bam_projdir);
  $bam_projipc = "/var/run/fab/$bam_projhash";

  $bam_ipcdir = $bam_projipc;

  printf("%10s %s %s\n", "base", $bam_basehash, $bam_basedir);
  printf("%10s %s %s\n", "proj", $bam_projhash, $bam_projdir);
  printf("%10s %s %s\n", "ipcdir", $bam_projhash, $bam_ipcdir);
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
    , $bam_fabpath
    , "--no-launch"
    , "kill"
  );

  # snarf the best-effort exit status file
  my $exit = -1;
  if(open my $fh, "<", "$bam_ipcdir/fabd/exit")
  {
    my $line = <$fh>;
    chomp $line;
    close $fh;
    $exit = int($line);
  }

  my $err = '';
  if(open my $fh, "<", "$bam_ipcdir/fabd/stderr")
  {
    $err = do { local $/ = undef; <$fh> };
    chomp $err;
    close $fh;
  }

  # must have written zero to its exit file
  # must not have written anything to its stderr file
  return if $exit == 0 && length($err) == 0;

  print("> $bam_ipcdir/fabd/exit\n$exit\n");
  print("> $bam_ipcdir/fabd/stderr\n$err\n");
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
  run({ @_, cwd => $bam_projdir }, $bam_fabpath
    , "reconcile"
    , "-K"
    , "--system-config-path", "$bam_basedir/system-config"
    , "--user-config-path", "$bam_basedir/user-config"
    , "--default-filesystem-invalidate", $bam_invalidate
    , "--sweeper-period-nsec", $bam_sweeper_period
  );

  my $result = bam_tree([$bam_basedir, "--no-module"]);
  print("$$result{out}\n");
}

sub bam_reconcile
{
  run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
    , "--no-launch"
    , "reconcile"
  );
}

sub bam_stats
{
  my $name = shift;
  my @args = @_;

  # fetch stats
  my %stats;
  unless(grep { $_ eq "-S" } @args) {
    unshift @args, "--no-special";
  }
  unshift @args, "--no-launch", "stats";
  unshift @args, "-m", ".";
  push @args, $name if $name;

  my $result = run({ cwd => $bam_projdir }
    , $bam_fabpath
    , @args
  );
  my $text = $$result{out};
  for my $line (split(/\n/, $text))
  {
    chomp $line;
    my $k;
    my $v;
    if($line =~ /^ *([-_a-z0-9]+) : ([0-9]+)$/) {
      $k = $1;
      $v = int($2);
    } elsif ($line =~ /^ *([-_a-z0-9]+) : ([-_a-zA-Z0-9\/+.]+)$/) {
      $k = $1;
      $v = $2;
    } else {
      next;
    }
    die "duplicate key $k" if exists($stats{$k});
    $stats{$k} = $v;
  }

  \%stats;
}

sub bam_metadata
{
  # fetch metadata
  my %metadata;

  my $result = run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
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

sub bam_filesystem
{
  my $arglist = shift;

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_fabpath
    , "--no-launch"
    , "filesystem"
    , @$arglist
  );
}

sub bam_describe
{
  my $arglist = shift;

  run({ @_, cwd => $bam_projdir, @_ }
    , $bam_fabpath
    , "--no-launch"
    , "describe"
    , @$arglist
  );
}

sub bam_ls
{
  my $arglist = shift;

  run({ cwd => $bam_projdir, @_ }
    , $bam_fabpath
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
    , $bam_fabpath
    , "--no-launch"
    , "--no-color"
    , "tree"
    , @$arglist
  );
}

sub bam_modtree
{
  run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
    , "--no-launch"
    , "tree"
    , "-g"
    , "modtree"
  );
}

sub bam_build
{
  run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
    , "--no-launch"
    , "build"
  );
}

sub bam_vars
{
  my $res = run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
    , "--no-launch"
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

sub bam_env
{
  my $res = run({ @_, cwd => $bam_projdir }
    , $bam_fabpath
    , "--no-launch"
    , "env"
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
    , $bam_fabpath
    , "--no-launch"
    , "config"
  );
}

1
