#!/usr/bin/env perl
#
# Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>
# 
# This file is part of fab.
# 
# fab is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# fab is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with fab.  If not, see <http://www.gnu.org/licenses/>. */

# target - path to exports file
# deps - colon delimited list of paths to source files in the module

my @cmd = (
    $ENV{cc}
  , $ENV{cflags}
  , "-shared"
  , "-Wl,--version-script=" . $ENV{target} . "../exports"
  , map { "-I$_" } split(/:/, $ENV{includes})
  , "-o" $ENV{target}
  # object files in this module
  , split(/:/, $ENV{thisdeps})
  # immediate library dependencies
  , split(/:/, $ENV{otherdeps})
);

exec { $cmd[0] } @cmd

__END__

env [
  deps {
    type traversal
    direction up|down
    mode pre|post
    travel fs|weak|strong
    visit fs|weak|strong
  }

  otherdeps {
    type traversal
    direction up|down
    mode pre|post
    travel fs|weak|strong
    visit fs|weak|strong
  }
]
