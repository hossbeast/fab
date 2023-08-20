# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

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
