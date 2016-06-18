/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include "pathtree_test.h"

/*

TEST
  test_displace

SUMMARY
  builds the tree with inserts that displace existing nodes

*/

xunit_unit xunit = {
    setup : pathtree_unit_setup
  , tests : (xunit_test*[]) {
      (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/asgard" }
          , { key : "/midgard" }
          , { }
        }
      , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
            { key : "/midgardd" }
          , { key : "/midgard/foo", val : "/midgard" }
          , { key : "/midgard/asgardd", val : "/midgard" }
          , { key : "/midgard/asgard/foo", val : "/midgard/asgard" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/ab/asgard" }
          , { key : "/midgard" }
          , { }
        }
      , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
            { key : "/midgard/ab", val : "/midgard" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/ab/asgard" }
          , { key : "/midgard/ab" }
          , { key : "/midgard" }
          , { }
        }
      , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
            { key : "/midgard", val : "/midgard" }
          , { key : "/midgarda" }
          , { key : "/midgard/a", val : "/midgard" }
          , { key : "/midgard/ab/a", val : "/midgard/ab" }
          , { key : "/midgard/asgard", val : "/midgard" }
          , { key : "/midgard/asgarda/a", val : "/midgard" }
          , { key : "/midgard/asgard/a", val : "/midgard" }
          , { key : "/midgard/ab/asgard/a", val : "/midgard/ab/asgard" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/ab/asgard" }
          , { key : "/midgard/ab/isengard" }
          , { }
        }
      , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
            { key : "/midgard/ab" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/ab/bifrost" }
          , { key : "/midgard/a" }   // residue : 1, 2 new nodes, match and residue child of new parent
          , { key : "/midgard/ab" }
          , { }
        }
      , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
            { key : "/midgard" }
          , { key : "/midgard/a/b", val : "/midgard/a" }   // residue : 1, 2 new nodes, match and residue child of new parent
          , { key : "/midgard/ab/c", val : "/midgard/ab" }   // residue : 1, 2 new nodes, match and residue child of new parent
          , { key : "/midgard/ab/bifr", val : "/midgard/ab" }
          , { key : "/midgard/ab/bifrostt", val : "/midgard/ab" }
          , { key : "/midgard/ab/bifrost/t", val : "/midgard/ab/bifrost" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/midgard/ab/bifrost" }
          , { key : "/midgard/abc" } // residue : 3, 2 new nodes, match and residue child of new parent
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/helgrind/ragnarok/bifrost" }
          , { key : "/helgrind/ragnarok/bifrost/heimdall" }
          , { key : "/helgrind/ragnarok/heimdall" }
          , { }
        }
      }}
    , (pathtree_test[]){{
        entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/bifrost/asgard" }
          , { key : "/bifrost/helgrind/ragnarok/bifrost" }
          , { key : "/bifrost/helgrind/ragnarok/isengard" }
          , { }
        }
      }}
    , 0
  }
};
