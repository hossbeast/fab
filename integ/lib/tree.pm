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

    $line =~ /-- ([-_a-zA-Z0-9.+%]+)(?: -> ([-_a-zA-Z0-9.+\/]+))?( +<<)?( +[0-8]+)?$/ or die("wtf : $line");
    my $name = $1;
    my $ref = $2;
    my $contents = $3;
    my $perms = $4;

    if($contents) {
      $contents = '';
      while(@lines)
      {
        $subline = shift @lines;
        last if $subline eq ">>";
        $contents .= "$subline\n";
      }
    }
    if($perms) {
      $perms = oct($perms);
    } else {
      $perms = 0644;
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
      , __linkref => $ref
    };
    $stack[$#stack]{$name} = $item;
    push @stack, $item;
  }

  return $root;
}

sub tree_make
{
  my ($spec, $path) = @_;

  my $file = 1;
  for my $k (keys %$spec)
  {
    next if $k eq "__content";
    next if $k eq "__perms";
    next if $k eq "__linkref";
    $file = 0;
    last;
  }

  if($$spec{__linkref})
  {
    symlinkf($$spec{__linkref}, $path);
  }
  elsif($file)
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
      next if $k eq "__linkref";

      tree_make($v, "$path/$k");
    }
  }
}

sub tree_build
{
  my ($basedir, $spec) = @_;

  my $root = tree_parse($spec);

  # default user-config file if the spec does not specify
  # a user config file explicitly
  unless(exists($$root{"user-config"})) {
    $$root{"user-config"} = {
        __perms => 0644
      , __linkref => "$repodir/integ/user-config.bam%devel"
    };
  }

  system("rm -rf $basedir/*") if -e $basedir;
  mkdirp($basedir);
  tree_make($root, $basedir);
}

1
