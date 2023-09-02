# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

use xlinux;
use Data::Dumper;

sub mkdirp
{
  my $path = shift;

  return if mkdir($path);
  die("mkdir($path) : $!") unless $!{EEXIST};
}

sub symlinkf
{
  my ($target, $ref) = @_ or die $!;

  if(!unlink($ref)) {
    die("unlink($ref) : $!") unless $!{ENOENT};
  }

  return if symlink($target, $ref);
  die("symlink($target, $ref) : $!");
}

sub assert(&)
{
  my $block = shift;

  my $rv = eval { &$block };
  $rv = !!$rv;

  if($@ || !$rv)
  {
    my (undef, $file, $line) = caller();
    open my $fh, ("<", $file) or die("open($file) : $!");
    while(<$fh>)
    {
      chomp;
      if($. == $line)
      {
        die "DIED : $_ @ $file:$line" if $@;
        die "FAILED : $_ @ $file:$line\n" if !$rv;
      }
    }
    die "ASSERTED : $rv : $@";
  }
}

sub equal
{
  my ($a, $b) = @_;
  if(defined($a) ^ defined($b)) {
    return 0;
  }
  if(!defined($a)) {
    return !defined($b);
  }
  return $a eq $b;
}

sub stats
{
  my $entity = shift;
  my $items = pop;
  my @args = @_;

  my $stats = bam_stats($entity, @args);

  my $mismatch = 0;
  for my $k (keys %$items)
  {
    my $v = $$items{$k};
    if(!equal($$stats{$k}, $v))
    {
      $mismatch = 1;
      last;
    }
  }

  return 1 unless $mismatch;

  my %pkeys;
  map { $pkeys{$_} = 1 } keys %$items;
  map { $pkeys{$_} = 1 } keys %$stats;
  my @keys = sort keys %pkeys;

  print("stats " . ($entity || '-system-') . " >> \n");
  for my $k (@keys)
  {
    my $v = $$stats{$k};
    printf("%35s : %-12s", $k, $v);
    if(exists($$items{$k}))
    {
      my $eq = equal($$items{$k}, $$stats{$k});# ($$items{$k} || '') eq ($$stats{$k} || '');
      printf("%s %s", ($eq ? "==" : "<>"), $$items{$k});
    }
    print("\n");
  }

  return 0;
}

sub eexists
{
  my $name = shift;

  my $result = bam_ls([$name, "--no-follow"]);
  if(!length($$result{out})) {
    goto err;
  }
  if(length($$result{err})) {
    goto err;
  }
  return 1;

err:
  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  return 0;
}

sub enoent
{
  my $name = shift;

  my $result = bam_ls([$name, "--no-follow"], tolerant => 1);
  if(length($$result{out})) {
    goto err;
  }
  if($$result{err} !~ /not found/) {
    goto err;
  }
  return 1;

err:
  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  return 0;
}

sub uncreated
{
  my $name = shift;

  my $result = bam_ls([$name, "--no-follow", "--graph", "fstree-hidden"]);
  if($$result{out} !~ /uncreated/) {
    goto err;
  }
  if(length($$result{err})) {
    goto err;
  }
  return 1;

err:
  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  return 0;
}

sub unlinked
{
  my $name = shift;

  my $result = bam_ls([$name, "--no-follow", "--graph", "fstree-hidden"]);
  if($$result{out} !~ /unlinked/) {
    goto err;
  }
  if(length($$result{err})) {
    goto err;
  }
  return 1;

err:
  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  return 0;
}

sub ls
{
  my $path = shift;
  my $expected = shift;
  my @args = @_;

  unshift(@args, $path);
  unshift(@args, "--no-follow");
#  unshift(@args, "--no-module");

  my $result = bam_tree(\@args);
  return 1 if $$result{out} eq $expected;

  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  print STDERR ("$$result{out} != $expected\n");

  return 0;
}

sub get_node_identifiers
{
  my $path = shift;
  my @args = @_;

  unshift(@args, $path);

  my $result = bam_describe(\@args);
  goto err if $$result{err};
  goto err if $$result{exit};

  if($$result{out} !~ m|^\s+id : (?:")?([^"\n]+)(?:")?$|m) {
    goto err;
  }
  my $id = "$1";

  if($$result{out} !~ m|^\s+token : (?:")?([^"\n]+)(?:")?$|m) {
    goto err;
  }
  my $token = "$1";

  return $id, $token;

err:
  print STDERR ("exit : $$result{exit}\n");
  print STDERR ("out  : $$result{out}\n");
  print STDERR ("err  : $$result{err}\n");
  die;
}

sub snarf
{
  my ($path) = @_;

  open $fh, "<", "$path" or die("open($path) : $!");
  my $text = do { local $/ ; <$fh> };
  chomp $text;
  close $fh;
  return $text;
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

sub assert_system_state
{
  my $expected = shift;

  my $md = bam_metadata();
  return if $$md{"system-state"} eq $expected;

  print Dumper $md;
  assert { $$md{"system-state"} eq $expected };
}

my %status_by_pid;
$SIG{CHLD} = sub {
  local ($!, $?);
  while((my $pid = waitpid(-1, WNOHANG)) > 0) {
    my $exit = $? >> 8;
    my $sig = $? & 127;
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
    POSIX::dup2($out_wr_fd, 1) or die $!;
    POSIX::dup2($err_wr_fd, 2) or die $!;

    for(my $x = 0; $x < 1024; $x++)
    {
      next if $x == 1 || $x == 2;
      POSIX::close($x);
    }

    POSIX::open("/dev/null", O_WRONLY);

    if($$params{cwd})
    {
      chdir($$params{cwd}) or die $!;
    }

    pr_set_pdeathsig(9);  # no zombies

    exec { $cmd[0] } @cmd;
    exit(1);
  }

  # close write-end of the pipes
  POSIX::close($out_wr_fd) or die $!;
  POSIX::close($err_wr_fd) or die $!;

  my $rin = '';
  vec($rin, $out_rd_fd, 1) = 1;
  vec($rin, $err_rd_fd, 1) = 1;

  my $buf = '';
  my $out = '';
  my $out_len = 0;
  my $err = '';
  my $err_len = 0;
  while(1)
  {
    my $n = select(my $rout = $rin, undef, undef, 0.125);
    next unless ($n > 0);

    my $or = 0;
    if(vec($rout, $out_rd_fd, 1) == 1)
    {
      $or = xread($out_rd_fd, $buf, 0xffff);
      $out_len += $or;
      if($or)
      {
        $out .= substr($buf, 0, $or);
      }
    }

    my $er = 0;
    if(vec($rout, $err_rd_fd, 1) == 1)
    {
      $er = xread($err_rd_fd, $buf, 0xffff);
      $err_len += $er;
      if($er)
      {
        $err .= substr($buf, 0, $er);
      }
    }

    # once the child process has exited (which closes its fds), and pipes are drained
    #
    # https://man7.org/linux/man-pages/man7/pipe.7.html
    #  If all file descriptors referring to the write end of a pipe have
    #  been closed, then an attempt to read(2) from the pipe will see
    #  end-of-file (read(2) will return 0).
    last if($or == 0 && $er == 0 && exists $status_by_pid{$pid});
  }

  if($$params{tolerant}) {
    goto pass;
  }
  if(!$$params{expect_error} && $status_by_pid{$pid} == 0 && $err_len == 0) {
    goto pass;
  }
  if($$params{expect_error} && $status_by_pid{$pid} != 0) {
    goto pass;
  }

  print STDERR ("pid : $pid\n");
  if($$params{cwd}) {
    print STDERR ("cwd : $$params{cwd}\n");
  }
  print STDERR ("cmd : " . join(" ", @cmd) . "\n");
  print STDERR ("status : $status_by_pid{$pid}\n");
  print STDERR ("stdout : $out_len\n$out\n");
  print STDERR ("stderr : $err_len\n$err\n");
  die;

pass:
  print STDERR ("[$status_by_pid{$pid} out $out_len err $err_len] " . join(" ", @cmd) . "\n");

  chomp($out);
  chomp($err);

  return { pid => $pid, exit => $status_by_pid{$pid}, out => $out, err => $err };
}

1
