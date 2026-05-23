# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

package bam;

use Digest::SHA;
use bam::bech32;

# algorithm for the bamd id, which is slightly different and relies only on sha1
sub identity_base
{
  my $hash = Digest::SHA->new('sha1');
  for(my $x = 0; $x <= $#_; $x += 2) {
    my $type = $_[$x + 0];
    my $name = $_[$x + 1];
    if($x) {
      $hash->add("\n");
    }
    $hash->add($type);
    if($name) {
      $hash->add(" ");
      $hash->add($name);
    }
  }
  my $digest = $hash->digest();
  $digest = substr($digest, 0, 16);

  my $out = join("", map { sprintf("%02x", $_) } unpack("C*", $digest));

  return $out;
}

#
# algorithm for creating node ids, see bamd/identity/identity.c
#  both the underlying hash (sha1) and the number of bits used
#  may be parameterized in future
#
sub identity
{
  my $hash = Digest::SHA->new('sha1');
  for(my $x = 0; $x <= $#_; $x += 2) {
    my $type = $_[$x + 0];
    my $name = $_[$x + 1];
    if($x) {
      $hash->add("\n");
    }
    $hash->add($type);
    if($name) {
      $hash->add(" ");
      $hash->add($name);
    }
  }
  my $digest = $hash->digest();
  $digest = substr($digest, 0, 12);

  my $out = bam::bech32::bech32_encode("bm", $digest);

  return $out;
}

1
