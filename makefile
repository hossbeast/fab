# programs
CC              :=gcc
FLEX						:=flex
BISON						:=bison
SHELL           :=/bin/bash

# paths
SRCDIR           =.
VPATH           +=${SRCDIR}
VPATH						+=$(foreach d,${SRCDIR}/${d},$(shell find -mindepth 1 -maxdepth 1 -type d -printf "%f\n" | grep -v '\.'))

NAME             =listwise
L_NAME           =lib${NAME}
LIB              =${L_NAME}.so
INCDIR           =/usr/include
LIBDIR          ?=/usr/lib64
DLIBDIR         ?=/usr/lib
VER              =0.1

OPLIST					:=$(shell ls -1 op | grep -v 'op\.'| sed -e 's/.*/op\/&\/&.so/')
VPATH           +=$(shell ls -1 op | sed -e 's/.*/op\/&/')

INSTALL         :=install
INSTALL_PROGRAM :=${INSTALL}
INSTALL_DATA    :=${INSTALL} -m 644

#
# default goal
#
all: ${LIB} ${OPLIST}

#
# required objects
#
SOURCES          =$(shell ls -1 *.c */*.c)
SOURCES					+=generator/generator.lex.c
SOURCES					+=generator/generator.tab.c

#
# compiler flags
#  C/L OPTS  - internal to makefile flags
#  C/L FLAGS - user specified compiler flags
# 
COPTS						+=-m64 -O3 -Werror -D_GNU_SOURCE -fPIC -I${SRCDIR} -I${SRCDIR}/common -I${SRCDIR}/re -I${SRCDIR}/generator
LOPTS            =-lpcre -ldl -shared -Wl,--version-script=exports -Wl,-soname,${LIB}

%.o : COPTS     +=-c

#
# recipes
#

# link the library
${LIB}: ${SOURCES:.c=.o} exports
	${CC} ${COPTS} ${CFLAGS} -o $@ *.o */*.o ${LOPTS} ${LFLAGS}

# link an operator so
%.so: %.o
	${CC} ${COPTS} ${CFLAGS} -o $@ $^ -L${SRCDIR} -llistwise -shared -Wl,-soname,$(shell basename $*).so

#
# how to build various things
#

# make an exports from SOURCES - useful when creating an .so
exports : ${SOURCES}
	@echo "{ global: " > $@
	 sed 's/.*API\(DATA\)\?[[:space:]]\+\([a-zA-Z][0-9a-zA-Z_]*\).*/\2;/p; d' $^ >> $@
	@echo "local: *; };" >> $@

# .y.c's from local or common_c .y's
%.tab.c %.tab.h: %.y
	${BISON} --warnings=error -o $*.tab.c -d ${BISONFLAGS} $^

# .lex.c's from local or common_c .l's
%.lex.c %.lex.h: %.l %.tab.h
	${FLEX} -o $*.lex.c --header-file=$*.lex.h ${FLEXFLAGS} $<

%.c : %.y
%.c : %.l

%.o : %.c
	$(CC) $(COPTS) $(CFLAGS) $< -o $@

# dependencies
generator/generator.o : generator/generator.tab.h
generator/generator.o : generator/generator.lex.h

lstack.o : common/xmem.o

re.o : common/xmem.o

op.o : common/xmem.o

object.o : common/idx.o

op/cp/cp.so : common/xmem.o

op/s/s.so : common/parseint.o
op/s/s.so : common/xmem.o

op/u/u.so : common/xstring.o
op/u/u.so : common/xmem.o

op/ss/ss.so : common/xstring.o
op/ss/ss.so : common/xmem.o

op/sn/sn.so : common/parseint.o
op/sn/sn.so : common/xmem.o

#
# phony targets
#

.PHONY: install uninstall clean

install: ${LIB} ${OPLIST}
	${INSTALL} -d                      ${DESTDIR}/${LIBDIR}
	${INSTALL_PROGRAM} ${LIB}          ${DESTDIR}/${LIBDIR}/${LIB}.${VER}
	ln -fs ${LIB}.${VER}               ${DESTDIR}/${LIBDIR}/${LIB}
	${INSTALL} -d                      ${DESTDIR}/${INCDIR}
	${INSTALL} -d                      ${DESTDIR}/${INCDIR}/listwise
	${INSTALL} listwise.h              ${DESTDIR}/${INCDIR}/listwise.h
	${INSTALL} listwise/operator.h     ${DESTDIR}/${INCDIR}/listwise/operator.h
	${INSTALL} listwise/generator.h    ${DESTDIR}/${INCDIR}/listwise/generator.h
	${INSTALL} listwise/ops.h          ${DESTDIR}/${INCDIR}/listwise/ops.h
	${INSTALL} listwise/lstack.h       ${DESTDIR}/${INCDIR}/listwise/lstack.h
	${INSTALL} listwise/object.h       ${DESTDIR}/${INCDIR}/listwise/object.h
	
	${INSTALL} -d               ${DESTDIR}/${DLIBDIR}/listwise
	for x in ${OPLIST}; do \
		${INSTALL_PROGRAM} $$x    ${DESTDIR}/${DLIBDIR}/listwise/$$(basename $$x); \
	done

uninstall:
	rm -f												${DESTDIR}/${LIBDIR}/${LIB}.${VER}
	rm -f												${DESTDIR}/${LIBDIR}/${LIB}
	rm -rf											${DESTDIR}/${DLIBDIR}/listwise
	rm -f                       ${DESTDIR}/${INCDIR}/listwise.h

clean ::
	find . \( -false $(foreach txt,*.o *.so *.lex.* *.tab.* exports,-o -name '$(txt)') \) -delete
