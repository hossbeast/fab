# Copyright (c) 2017-2020 Todd Freed <todd.freed@gmail.com>
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
#

our $bam_invalidate;

sub tree_parse
{
  my $spec = shift;

  my $root = {};
  my @stack = ( $root );
  my $level = 0;
  my @lines = split(/\n/, $spec);
  while(my $line = shift(@lines))
  {
    $line =~ m/^[|` ]+/;
    my $indent = int(sprintf("%d", length($&) / 4)) + 1;

    $line =~ /-- ([-_a-zA-Z0-9.+]+)( +<<)?( +[0-8]+)?$/ or die;
    my $name = $1;
    my $contents = '';
    my $perms = 0644;
    if($3)
    {
      $perms = oct($3);
    }
    if($2)
    {
      while(my $subline = shift(@lines))
      {
        last if $subline eq ">>";
        $contents .= "$subline\n";
      }
    }

    while($indent <= $level)
    {
      pop @stack;
      $level--;
    }

    $level = $indent;
    my $item = {
        __content => $contents
      , __perms => $perms
    };
    $stack[$#stack]{$name} = $item;
    push @stack, $item;
  }

  return $root;
}

sub mkdirp
{
  my $path = shift;

  return if mkdir($path);
  die("mkdir($path) : $!") unless $!{EEXIST};
}

sub tree_make
{
  my ($base, $path) = @_;

  my $file = 1;
  for my $k (keys %$base)
  {
    next if $k eq "__content";
    next if $k eq "__perms";
    $file = 0;
    last;
  }

  if($file)
  {
    open FH, ">$path" or die "open($path) : $!";
    print FH $$base{__content};
    close FH;
    chmod($$base{__perms}, $path) or die $!;
  }
  else
  {
    mkdirp($path);
    while(my($k, $v) = each %$base)
    {
      next if $k eq "__content";
      next if $k eq "__perms";

      tree_make($v, "$path/$k");
    }
  }
}

sub tree_build
{
  my ($name, $spec) = @_;

  $name =~ s/[^a-zA-Z0-0]//g;
  my $testdir = "/run/fab-integ/${name}-${bam_invalidate}";

  my $root = tree_parse($spec);

  system("rm -rf $testdir/*") if -e $testdir;

  mkdirp("/run/fab-integ");
  tree_make($root, $testdir);

  return $testdir;
}

1
