# SPDX-License-Identifier: GPL-3.0
# Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com>

package bam::bech32;

#
# taken and adapted from https://github.com/sipa/bech32/blob/master/ref/c/segwit_addr.c
#

my @alphabet = (
    'q', 'p', 'z', 'r', 'y', '9', 'x', '8'
  , 'g', 'f', '2', 't', 'v', 'd', 'w', '0'
  , 's', '3', 'j', 'n', '5', '4', 'k', 'h'
  , 'c', 'e', '6', 'm', 'u', 'a', '7', 'l'
);

sub bech32_polymod_step
{
  my $pre = shift;
  my $b;
  my $x;

  $b = $pre >> 25;
  $x = (($pre & 0x1ffffff) << 5);
  $x ^= (-(($b >> 0) & 1) & 0x3b6a57b2);
  $x ^= (-(($b >> 1) & 1) & 0x26508e6d);
  $x ^= (-(($b >> 2) & 1) & 0x1ea119fa);
  $x ^= (-(($b >> 3) & 1) & 0x3d4233dd);
  $x ^= (-(($b >> 4) & 1) & 0x2a1462b3);

  return $x;
}

sub bech32_encode
{
  my ($hrps, $datas) = @_;

  my $chk;
  my $x;
  my $out;
  my $val;
  my $bits;

  my @hrp = unpack("C*", $hrps);
  my @data = unpack("C*", $datas);

  # hrp first pass
  $chk = 1;
  for($x = 0; $x <= $#hrp; $x++) {
    $ch = $hrp[$x];
    $chk = bech32_polymod_step($chk) ^ ($ch >> 5);
  }
  $chk = bech32_polymod_step($chk);

  # hrp 2nd pass
  $out = '';
  for($x = 0; $x <= $#hrp; $x++) {
    $ch = $hrp[$x];
    $chk = bech32_polymod_step($chk) ^ ($ch & 0x1f);

    $out .= chr($ch);
  }

  # separator
  $out .= "1";

  # 8 -> 5 conversion
  $val = 0;
  $bits = 0;
  for($x = 0; $x <= $#data; $x++) {
    $val = ($val << 8) | $data[$x];
    $bits += 8;
    while($bits >= 5) {
      $bits -= 5;
      $ch = ($val >> $bits) & 0x1f;
      $chk = bech32_polymod_step($chk) ^ $ch;
      $out .= $alphabet[$ch];
    }
  }

  # remainder
  if($bits) {
    $ch = ($val << (5 - $bits)) & 0x1f;
    $chk = bech32_polymod_step($chk) ^ $ch;
    $out .= $alphabet[$ch];
  }

  # finalize checksum
  for($x = 0; $x < 6; $x++) {
    $chk = bech32_polymod_step($chk);
  }
  $chk ^= 0x2bc830a3;

  # append checksum
  for($x = 0; $x < 6; $x++) {
    $ch = ($chk >> ((5 - $x) * 5)) & 0x1f;
    $out .= $alphabet[$ch];
  }

  return $out;
}

1
