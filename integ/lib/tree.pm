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

    $line =~ /-- ([-_a-zA-Z0-9.+]+)( +<<)?( +[0-8]+)?$/ or die("wtf : $line");
    my $name = $1;
    my $contents = '';
    my $perms = 0644;
    if($3)
    {
      $perms = oct($3);
    }
    if($2)
    {
      while(@lines)
      {
        $subline = shift @lines;
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

sub tree_make
{
  my ($spec, $path) = @_;

#print("tree-make path $path\n");

  my $file = 1;
  for my $k (keys %$spec)
  {
    next if $k eq "__content";
    next if $k eq "__perms";
    $file = 0;
    last;
  }

  if($file)
  {
    open FH, ">$path" or die "open($path) : $!";
    print FH $$spec{__content};
    close FH;
    chmod($$spec{__perms}, $path) or die $!;
  }
  else
  {
    mkdirp($path);
    while(my($k, $v) = each %$spec)
    {
      next if $k eq "__content";
      next if $k eq "__perms";

      tree_make($v, "$path/$k");
    }
  }
}

sub tree_build
{
  my ($basedir, $spec) = @_;

  my $root = tree_parse($spec);

  system("rm -rf $basedir/*") if -e $basedir;
  mkdirp("/run/fab-integ");
  tree_make($root, $basedir);
}

sub tree_setup
{
  my ($testdir, $spec) = @_;
  my $root = tree_parse($spec);
  tree_make($root, $testdir);
}

1
