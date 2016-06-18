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
  test_insert

SUMMARY
  builds the tree with inserts that append new leaf nodes

*/

xunit_unit xunit = {
    setup : pathtree_unit_setup
  , tests : (xunit_test*[]) {
      (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/midgard" }
            , { key : "/midgard/a" }
            , { key : "/midgard/ab" }
            , { key : "/midgard/abc" }
            , { key : "/midgard/abcd" }
            , { }
          }
       }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/asgard" }
            , { key : "/isengard" }
            , { key : "/midgard/a" }
            , { }
          }
       }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/midgard" }
            , { key : "/midgard/ab/bifrost" }
            , { key : "/asgard" }
            , { key : "/asgard/bifrost" }
            , { }
          }
        , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
              { key : "" }
            , { key : "/midgard/ab", val : "/midgard" }
            , { key : "/asgard/bifr", val : "/asgard" }
          }
       }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/midgard" }
            , { }
          }
        , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
              { key : "" }
            , { key : "/midgar" }
            , { key : "/midgarda" }
            , { key : "/midgard/a", val : "/midgard" }
            , { }
          }
      }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "" }
            , { key : "/midgard" }
            , { key : "/midgard/a" }
            , { }
          }
        , queries : (typeof(*((pathtree_test*)0)->queries)[]) {
              { key : "/midgard/ab", val : "/midgard" }
            , { key : "/midgard/a/b", val : "/midgard/a" }
            , { }
          }
      }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/asgard" }
            , { key : "/asgard/bifrost" }
            , { key : "/asgard/bifrost/midgard" }
            , { }
          }
       }}
    , (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/h/ragnarok/bifrost" }
            , { }
          }
       }}
    , 0
  }
};
