CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.
CFLAGS += -Iload
CFLAGS += -Itest
CFLAGS += -Ichain
CFLAGS += -Imultimap
CFLAGS += -Irbtree
CFLAGS += -Ifaults
CFLAGS += -Imap
CFLAGS += -Ilist
CFLAGS += -Iassert
CFLAGS += -Ivalyria
CFLAGS += -Iset
CFLAGS += -Idictionary
CFLAGS += -Ihashtable
CFLAGS += -Istrutil
CFLAGS += -Iarray
CFLAGS += -Ipstring
CFLAGS += -Illist
CFLAGS += -Ierrtab
CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria
CFLAGS += -I../liblogger

obj += load/load.o
obj += chain/chain.o
obj += multimap/multimap.o
obj += rbtree/rbtree.o
obj += faults/faults.o
obj += map/map.o
obj += list/list.o
obj += assert/assert.o
obj += set/set.o
obj += dictionary/dictionary.o
obj += hashtable/hashtable.o
obj += strutil/strutil.o
obj += array/array.o
obj += pstring/pstring.o
obj += errtab/VALYRIA.errtab.o

libvalyria.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
