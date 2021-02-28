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

#package util;
#
#use strict;
#use warnings;
#
#require Exporter;
#our @ISA = qw|Exporter|;
#our @EXPORT = qw|uxmkdir xfhopen uxfhopen uxopen xunlink uxunlink pr_set_pdeathsig awrite xread|;
#
use xlinux;
use Data::Dumper;

sub assert(&)
{
  my $block = shift;

  my $rv = eval { &$block };
  $rv = !!$rv;
#  print("asserted -> $@, $rv\n");
#  die $@ if $@;
#  die $block if !$rv;

  if($@ || !$rv)
  {
    my (undef, $file, $line) = caller();
    open my $fh, ("<", $file) or die("open($file) : $!");
    while(<$fh>)
    {
      if($. == $line)
      {
        die "DIED : $_" if $@; 
        die "FAILED : $_" if !$rv;
      }
    }
    die "ASSERTED : $rv : $@";
  }
}

sub assert_stats
{
  my ($entity, $items) = @_;

  my $stats = bam_stats($entity);

  my $bad = 0;
  for my $k (keys %$items)
  {
    my $v = $$items{$k};
    if(($$stats{$k} || '') ne ($v || ''))
    {
      $bad = 1;
      last;
    }
  }

  return unless $bad;

  my %pkeys;
  map { $pkeys{$_} = 1 } keys %$items;
  map { $pkeys{$_} = 1 } keys %$stats;
  my @keys = sort keys %pkeys;

  print("stats for " . ($entity || '-system-') . " >> \n");
  for my $k (@keys)
  {
    my $v = $$stats{$k};
    printf("%25s : %-10s", $k, $v);
    if(exists($$items{$k}))
    {
      my $eq = ($$items{$k} || '') eq ($$stats{$k} || '');
      printf("%s %s", ($eq ? "==" : "<>"), $$items{$k});
    }
    print("\n");
  }

  my (undef, $file, $line) = caller();
  open my $fh, ("<", $file) or die("open($file) : $!");
  while(<$fh>)
  {
    if($. == $line)
    {
      die "DIED $_ at $file:$line\n";
    }
  }
  die
}

sub assert_file
{
  my ($path, $expected) = @_;

  open $fh, "<", "$path" or die("open($path) : $!");
  my $text = do { local $/ ; <$fh> };
  chomp $text;
  close $fh;
  assert { $text eq $expected };
}

my %status_by_pid;
$SIG{CHLD} = sub {
  local ($!, $?);
  while((my $pid = waitpid(-1, WNOHANG)) > 0) {
    my $exit = $? >> 8;
    my $sig = $? & 127;
  
#    printf("CHLD %s pid=%s, status=%s, exit=%s, signal=%s\n"
#      , "(none)"
#      , $pid
#      , $?
#      , $exit
#      , $sig
#    );
  
    $status_by_pid{$pid} = $?;
  
    # affirmative murder
    kill 9, $pid;
  }
};

sub run
{
  my $params = shift;
  my @cmd = @_;

  my ($out_rd_fd, $out_wr_fd) = POSIX::pipe() or die $!;
  my ($err_rd_fd, $err_wr_fd) = POSIX::pipe() or die $!;
  my $pid = fork();
  if(!$pid)
  {
    POSIX::close(0);
    POSIX::close($out_rd_fd);
    POSIX::dup2($out_wr_fd, 1) or die $!;
    POSIX::close($err_rd_fd);
    POSIX::dup2($err_wr_fd, 2) or die $!;
    if($$params{cwd})
    {
      chdir($$params{cwd}) or die $!;
    }

    exec { $cmd[0] } @cmd;
  }

  POSIX::close($out_wr_fd) or die $!;
  POSIX::close($err_wr_fd) or die $!;

  my $in = '';
  vec($in, $out_rd_fd, 1) = 1;
  vec($in, $err_rd_fd, 1) = 1;

  my $out = '';
  my $err = '';
  while(1)
  {
    my $n = select(my $rout = $in, undef, my $eout = $in, 0.125);
#    print("n $n ");
#    if($n == -1) { print("$! "); }
#    print("r ");
#    print("$out_rd_fd ") if vec($rout, $out_rd_fd, 1);
#    print("$err_rd_fd ") if vec($rout, $err_rd_fd, 1);
#    print("e ");
#    print("$out_rd_fd ") if vec($eout, $out_rd_fd, 1);
#    print("$err_rd_fd ") if vec($eout, $err_rd_fd, 1);
#    print("\n");
#    next if ($n == -1 && $!{EINTR});
    last if ($n == 0 && exists $status_by_pid{$pid});
    next unless ($n > 0);

#if($Z++ == 500000) { print("WTF\n"); exit; }

    my $data;
    my $or = 0;
    my $er = 0;
    if(vec($rout, $out_rd_fd, 1))
    {
#printf("+rd($out_rd_fd)\n");
      $or = xread($out_rd_fd, $data, 0xffff);
#print("rd($out_rd_fd) : $or\n");
      $out .= $data if $or;
    }

    if(vec($rout, $err_rd_fd, 1))
    {
#printf("+rd($err_rd_fd)\n");
      $er = xread($err_rd_fd, $data, 0xffff);
#print("rd($err_rd_fd) : $er\n");
      $err .= $data if $er;
    }

    last if($or == 0 && $er == 0 && exists $status_by_pid{$pid});
  }

  if($status_by_pid{$pid} || $err)
  {
    print STDERR ("cmd : " . join(" ", @cmd) . "\n");
    print STDERR ("pid : $pid\n");
    print STDERR ("status : $status_by_pid{$pid}\n");
    print STDERR ("stderr : $err\n");
    die;
  }
  else
  {
    print STDERR ("[$status_by_pid{$pid}] " . join(" ", @cmd) . "\n");
  }

  return $out;
}

sub spawn
{
  my $params = shift;
  my @cmd = @_;

  my ($out_rd_fd, $out_wr_fd) = POSIX::pipe() or die $!;
  my ($err_rd_fd, $err_wr_fd) = POSIX::pipe() or die $!;
  my $pid = fork();
  if(!$pid)
  {
    POSIX::close(0);
    POSIX::close($out_rd_fd);
    POSIX::dup2($out_wr_fd, 1) or die $!;
    POSIX::close($err_rd_fd);
    POSIX::dup2($err_wr_fd, 2) or die $!;
    if($$params{cwd})
    {
      chdir($$params{cwd}) or die $!;
    }

    exec { $cmd[0] } @cmd;
  }

  POSIX::close($out_wr_fd) or die $!;
  POSIX::close($err_wr_fd) or die $!;

  return [ $pid, $out_rd_fd, $err_rd_fd ];
}

sub spawn_poll
{
  my ($pid, $out_rd_fd, $err_rd_fd) = @{$_[0]};

  my $in = '';
  vec($in, $out_rd_fd, 1) = 1;
  vec($in, $err_rd_fd, 1) = 1;

  my $out = '';
  my $err = '';

  my $n = select(my $rout = $in, undef, my $eout = $in, 0.125);
  die if ($n == 0 && exists $status_by_pid{$pid});

print("in  : " . vec($rout, $out_rd_fd, 1) . "\n");
print("out : " . vec($rout, $err_rd_fd, 1) . "\n");

  my $data;
  my $or = 0;
  my $er = 0;
  if(vec($rout, $out_rd_fd, 1))
  {
    $or = xread($out_rd_fd, $data, 0xffff);
    $out .= $data if $or;
  }

  if(vec($rout, $err_rd_fd, 1))
  {
    $er = xread($err_rd_fd, $data, 0xffff);
    $err .= $data if $er;
  }

  die if($or == 0 && $er == 0 && exists $status_by_pid{$pid});

  if($status_by_pid{$pid} || $err)
  {
    print STDERR ("cmd : " . join(" ", @cmd) . "\n");
    print STDERR ("pid : $pid\n");
    print STDERR ("status : $status_by_pid{$pid}\n");
    print STDERR ("stderr : $err\n");
    die;
  }
#  else
#  {
#    print STDERR ("cmd : " . join(" ", @cmd) . "\n");
#    print STDERR ("status : $status_by_pid{$pid}\n");
#  }

  return $out;
}

1
