#!/bin/bash

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
# along with fab.  If not, see <http://www.gnu.org/licenses/>.

# final buildscripts
./fab/fab/fab.devel --bslic-fab -k make64.final.sh @final
./fab/fab/fab.devel --bslic-fab -k install64.local.final.sh +n @install.local.final \
	-K destdir	\
	-K incdir \
	-K bindir \
	-K mandir \
	-K libdir \
	-K dlibdir \
	-K lwopdir \
	-K fablwopdir \
	-K fabtmpdir \
	-K fabipcdir \
	-K fabinvokedir

./fab/fab/fab.devel --bslic-fab -k install64.dist.final.sh +n @install.dist.final \
	-K destdir	\
	-K incdir \
	-K bindir \
	-K mandir \
	-K libdir \
	-K dlibdir \
	-K lwopdir \
	-K fablwopdir \
	-K fabtmpdir \
	-K fabipcdir \
	-K fabinvokedir

# debug buildscripts
./fab/fab/fab.devel --bslic-fab -k make64.debug.sh @debug
./fab/fab/fab.devel --bslic-fab -k install64.local.debug.sh +n @install.local.debug \
	-K destdir	\
	-K incdir \
	-K bindir \
	-K mandir \
	-K libdir \
	-K dlibdir \
	-K lwopdir \
	-K fablwopdir \
	-K fabtmpdir \
	-K fabipcdir \
	-K fabinvokedir

./fab/fab/fab.devel --bslic-fab -k install64.dist.debug.sh +n @install.dist.debug \
	-K destdir	\
	-K incdir \
	-K bindir \
	-K mandir \
	-K libdir \
	-K dlibdir \
	-K lwopdir \
	-K fablwopdir \
	-K fabtmpdir \
	-K fabipcdir \
	-K fabinvokedir

# man buildscripts
./fab/fab/fab.devel --bslic-fab -k makeman.sh @man
./fab/fab/fab.devel --bslic-fab -k install.man.sh +n @install.man \
	-K destdir	\
	-K incdir \
	-K bindir \
	-K mandir \
	-K libdir \
	-K dlibdir \
	-K lwopdir \
	-K fablwopdir \
	-K fabtmpdir \
	-K fabipcdir \
	-K fabinvokedir

# devel buildscripts
./fab/fab/fab.devel --bslic-fab -k make64.devel.sh @devel
