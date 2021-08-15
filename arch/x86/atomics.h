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

#ifndef _ATOMIC_H
#define _ATOMIC_H

/* returns the value previously held in *p */
#define atomic_fetch_inc(p) ({                  \
  __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST);   \
})

/* returns the value previously held in *p */
#define atomic_fetch_dec(p) ({                  \
  __atomic_fetch_sub(p, 1, __ATOMIC_SEQ_CST);   \
})

#define atomic_load(p) ({                       \
  __atomic_load_n(p, __ATOMIC_SEQ_CST);         \
})

#define atomic_store(p, v) ({                   \
  __atomic_store_n(p, v, __ATOMIC_SEQ_CST);     \
})

#define atomic_exchange(p, v) ({                \
  __atomic_exchange_n(p, v, __ATOMIC_SEQ_CST);  \
})

#define atomic_cas(p, oldp, v) ({                                                      \
  __atomic_compare_exchange_n(p, oldp, v, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);  \
})

#endif
