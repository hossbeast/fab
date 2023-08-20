# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

use File::Basename;
use Data::Dumper;
use Time::HiRes qw|nanosleep|;

use strict;
use warnings;

our $bam_fabpath;
our $bam_testdir;
our $bam_invalidate;
our $bam_sweeper_period = 250 * 1000 * 1000;
our $repodir;

sub setup
{
  my $spec = shift;

  $bam_invalidate = $ARGV[0] || die "missing required argument : invalidate";
  $bam_fabpath = "$repodir/fab/fab/fab.devel.pic";
  $bam_testdir = tree_build(basename($0), $spec);

  bam_bootstrap();

  return $bam_testdir;
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
  print Dumper bam_tree();
}

sub build
{
  bam_build(@_);
  if($bam_invalidate eq "notify")
  {
    printf("sleep " . (($bam_sweeper_period * 4)/1000/1000/1000) . "\n");
    nanosleep($bam_sweeper_period * 4);
  }
}

sub bam_bootstrap
{
print("x");
  run({ cwd => $bam_testdir, tolerate_error => 1 }
    , $bam_fabpath
    , "reconcile"
    , "-K"
    , "--system-config-path", "system-config"
    , "--user-config-path", "user-config"
    , "--default-filesystem-invalidate", $bam_invalidate
    , "--sweeper-period-nsec", $bam_sweeper_period
  );

print("y");
  print Dumper bam_metadata();
print("z");
  print Dumper bam_tree();
}

sub bam_reconcile
{
  run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "reconcile"
  );
}

sub bam_stats
{
  my $name = shift;

  # fetch stats
  my %stats;
  my @args = ("--no-launch", "stats");
  push @args, $name if $name;

  my $text = run({ cwd => $bam_testdir }
    , $bam_fabpath
    , @args
  );
  for my $line (split(/\n/, $text))
  {
    chomp $line;
    if($line =~ /^ *([-_a-z0-9]+) : ([0-9]+)$/) {
      $stats{$1} = int($2);
    } elsif ($line =~ /^ *([-_a-z0-9]+) : ([-a-zA-Z0-9\/]+)$/) {
      $stats{$1} = $2;
    }
  }

#for my $k (sort keys %stats)
#{
#  printf("%25s %s\n", $k, $stats{$k});
#}

  \%stats;
}

sub bam_metadata
{
  # fetch metadata
  my %metadata;

  my $text = run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "metadata"
  );
  for my $line (split(/\n/, $text))
  {
    next unless $line =~ /^ *([-_a-z0-9]+) : (.*)/;
    $metadata{$1} = $2;
  }

  \%metadata;
}

sub bam_describe
{
  my $text = run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "describe"
    , "."
  );
  print("$text\n");
}

sub bam_tree
{
  run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "tree"
  );
}

sub bam_modtree
{
  run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "tree"
    , "-g"
    , "modtree"
  );
}

sub bam_build
{
  run({ @_, cwd => $bam_testdir }
    , $bam_fabpath
    , "--no-launch"
    , "build"
  );
}

1
