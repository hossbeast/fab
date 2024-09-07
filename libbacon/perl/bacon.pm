# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

sub bacon__stackpush
{
  my ($stack, $value) = @_;

  if(ref(@{$stack}[$#$stack]) eq 'SCALAR') {
    if(${@{$stack}[$#$stack]} eq 'bacon__mapping-token') {
      pop @$stack;
      my $key = pop @$stack;
      my $mapping = { $key => $value };
      push @$stack, $mapping;
      return;
    }
  }

  push @$stack, $value;
}

sub bacon__set_end
{
  my $stack = shift;

  my %set;
  while(1)
  {
    my $value = pop @$stack;
    if(ref($value) eq 'SCALAR') {
      if(${$value} eq 'bacon__set-token') {
        last;
      } else {
        die;
      }
    } elsif(ref($value) eq 'HASH') {
      while(my($k, $v) = each %$value) {
        $set{$k} = $v;
      }
    } else {
      $set{$value} = undef;
    }
  }

  bacon__stackpush($stack, \%set);
}

sub bacon__list_end
{
  my $stack = shift;

  my @list;
  while(1)
  {
    my $value = pop @$stack;
    if(ref($value) eq 'SCALAR') {
      if(${$value} eq 'bacon__list-token') {
        last;
      } else {
        die;
      }
    }

    unshift @list, $value;
  }

  bacon__stackpush($stack, \@list);
}

sub bacon_parse
{
  my $text = shift;

  my @stack;
  while(1)
  {
    if(!$text) {
      last;
    # discard leading whitespace
    } elsif($text =~ /^\s+/) {
    # value - primitives
    } elsif($text =~ /^0x[0-9a-f]+/) {
      bacon__stackpush(\@stack, hex($&));
    } elsif($text =~ /^[0-9]+/) {
      bacon__stackpush(\@stack, int($&));
    } elsif($text =~ /^true\b/) {
      bacon__stackpush(\@stack, 1);
    } elsif($text =~ /^false\b/) {
      bacon__stackpush(\@stack, 0);
    # unquoted string
    } elsif($text =~ /^[-_a-zA-Z0-9.\/+=]+/) {
      bacon__stackpush(\@stack, "$&");
    # quoted string
    } elsif($text =~ /^"([^"]+)"/) {
      bacon__stackpush(\@stack, $1);
    # aggregates - mapping
    } elsif($text =~ /^:\s+/) {
      push @stack, \'bacon__mapping-token';
    # aggregates - set
    } elsif($text =~ /^{/) {
      push @stack, \'bacon__set-token';
    } elsif($text =~ /^}/) {
      bacon__set_end(\@stack);
    # aggregates - list
    } elsif($text =~ /^\[/) {
      push @stack, \'bacon__list-token';
    } elsif($text =~ /^]/) {
      bacon__list_end(\@stack);
    } else {
      die("WTF");
    }

    substr($text, 0, length($&)) = '';
  }

  return $stack[0];
}

1
