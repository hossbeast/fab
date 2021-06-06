# Copyright (c) 2017-2018 Todd Freed <todd.freed@gmail.com>
#
# This file is part of autominer.
#
# autominer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# autominer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License

#package bam;

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
  $bam_fabpath = "$repodir/fab/fab/fab.devel.xapi";
  $bam_testdir = tree_build(basename($0), $spec);

  bam_bootstrap();

  return $bam_testdir;
}

sub reconcile
{
  if($bam_invalidate eq "notify")
  {
    printf("sleep " . (($bam_sweeper_period * 4)/1000/1000/1000) . "\n");
    nanosleep($bam_sweeper_period * 4);
  }
  bam_reconcile();
  print Dumper bam_tree();
}

sub bam
{
  my @args = @_;
  unshift @args, $bam_fabpath;

  run({ cwd => $bam_testdir, discard_stderr => 1 }, @args);
}

sub bam_bootstrap
{
  bam(
      "reconcile"
    , "-K"
    , "--system-config-path", "system-config"
    , "--user-config-path", "user-config"
    , "--default-filesystem-invalidate", $bam_invalidate
    , "--sweeper-period-nsec", $bam_sweeper_period
  );

  print Dumper bam_metadata();
  print Dumper bam_tree();
}

sub bam_reconcile
{
  bam("--no-launch", "reconcile");
}

sub bam_stats
{
  my $name = shift;

  # fetch stats
  my %stats;
  my @args = ("--no-launch", "stats");
  push @args, $name if $name;
  my $text = bam(@args);
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
  my $text = bam("--no-launch", "metadata");
  for my $line (split(/\n/, $text))
  {
    next unless $line =~ /^ *([-_a-z0-9]+) : (.*)/;
    $metadata{$1} = $2;
  }

  \%metadata;
}

sub bam_describe
{
  my $text = bam("--no-launch", "describe", ".");
  print("$text\n");
}

sub bam_tree
{
  my %metadata;
  my $text = bam("--no-launch", "tree");

  $text;
}

sub bam_modtree
{
  my %metadata;
  my $text = bam("--no-launch", "tree", "-g", "modtree");

  $text;
}

sub bam_build
{
  bam("--no-launch", "build");
}

1
