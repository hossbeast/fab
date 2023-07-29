CFLAGS += -ggdb3 -Wall -Werror -D_GNU_SOURCE -fno-builtin-log -Wmissing-prototypes
CFLAGS += -Wno-misleading-indentation -Wno-trigraphs
CFLAGS += -fno-common

CFLAGS +=  -DXAPI -DXAPI_MODE_STACKTRACE 

CFLAGS += -I.

CFLAGS += -Icategory
CFLAGS += -Iload
CFLAGS += -Istream
CFLAGS += -Iopts
CFLAGS += -Iarguments
CFLAGS += -Iconfig
CFLAGS += -Ilog
CFLAGS += -Ilogging
CFLAGS += -Ifinalize
CFLAGS += -Ierrtab
CFLAGS += -Iexpr
CFLAGS += -Ifilter
CFLAGS += -Ilogger

CFLAGS += -I../libcommon
CFLAGS += -I../include
CFLAGS += -I../libxapi
CFLAGS += -I../libxlinux
CFLAGS += -I../libnarrator
CFLAGS += -I../libvalyria

obj += category/category.o
obj += load/load.o
obj += stream/stream.o
obj += opts/opts.o
obj += arguments/arguments.o
obj += config/config.o
obj += log/log.o
obj += logging/logging.o
obj += finalize/finalize.o
obj += errtab/LOGGER.errtab.o
obj += expr/expr.o
obj += filter/filter.o

liblogger.final.a : $(obj)
	rm $@ 2>/dev/null ; ar rcs $@ $^

%.o : %.c
	cc -o $@ -c $(CFLAGS) $<
