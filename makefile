# programs
CC              :=gcc
FLEX						:=flex
BISON						:=bison
SHELL           :=/bin/bash
COMMON					:=../common

# paths
SRCDIR           =.
VPATH           +=${SRCDIR}
VPATH						+=$(shell find ${SRCDIR} -mindepth 1 -maxdepth 1 -type d -name '[a-z]*' -printf "%f\n" | grep -v doc | grep -v fablib | grep -v liblistwise)

INSTALL         :=install

#
# compiler flags
#  C/L OPTS  - internal to makefile flags
#  C/L FLAGS - user specified compiler flags
# 
COPTS						+=-m64 -g -O0 -Wall -Werror -fms-extensions -fno-builtin-log -D_GNU_SOURCE -I${COMMON} $(foreach x,${VPATH},-I${SRCDIR}/$(x))
%.o : COPTS     +=-c

LOPTS						+=-llistwise

#
# default goal
#
fab : main.o
	${CC} ${COPTS} ${CFLAGS} ${LFLAGS} -o $@ ${COMMON}/*.o $(foreach x,$(VPATH),$(x)/*.o) ${LOPTS}

#
# recipes
#

# .y.c's from local or common_c .y's
%.tab.c %.tab.h: %.y
	${BISON} --warnings=error -o $*.tab.c -d ${BISONFLAGS} $^

# .lex.c's from local or common_c .l's
%.lex.c %.lex.h: %.l %.tab.h
	${FLEX} -d -o $*.lex.c --header-file=$*.lex.h ${FLEXFLAGS} $<

%.tok.c %.tok.h : %.tab.c
	@echo "#ifndef _$(subst .,_,$(*F))_TOK_H"         							 	 >$(*D)/$(*F).tok.h
	@echo "#define _$(subst .,_,$(*F))_TOK_H"         								>>$(*D)/$(*F).tok.h
	@echo "const char* $(subst .,_,$(*F))_tokname(int);"							>>$(*D)/$(*F).tok.h
	@sed   -ne '/yytokentype/,/;/ p' < $<													\
	  | sed -e 's/\(yytokentype\)/$(subst .,_,$(*F))_\1/'         \
	  | sed -e '/ *[A-Z]/s/\([A-Z]\)/$(subst .,_,$(*F))_\1/'			    >>$(*D)/$(*F).tok.h
	@echo "#endif"                                  									>>$(*D)/$(*F).tok.h
	@echo "typedef unsigned short yytype_uint16;"   									>$(*D)/$(*F).tok.c
	@sed -ne '/yytname\[\]/,/};/ p'  < $< | sed -e 's/\\"\|'"'"'//g'  >>$(*D)/$(*F).tok.c
	@sed -ne '/yytoknum\[\]/,/};/ p' < $<            									>>$(*D)/$(*F).tok.c
	@grep 'define YYNTOKENS'        < $<            									>>$(*D)/$(*F).tok.c
	@echo '#define API __attribute__((visibility("protected")))'			>>$(*D)/$(*F).tok.c
	@echo '#include <stdio.h>'																				>>$(*D)/$(*F).tok.c
	@echo "const char API * $(subst .,_,$(*F))_tokname(int tok)"			>>$(*D)/$(*F).tok.c
	@echo "{"																													>>$(*D)/$(*F).tok.c
	@echo '  int x;'																									>>$(*D)/$(*F).tok.c
	@echo '  for(x = 0; x < YYNTOKENS; x++)'													>>$(*D)/$(*F).tok.c
	@echo '  {'																												>>$(*D)/$(*F).tok.c
	@echo '  	 if(yytoknum[x] == tok)'																>>$(*D)/$(*F).tok.c
	@echo '		 {'																											>>$(*D)/$(*F).tok.c
	@echo '      return yytname[x];'																	>>$(*D)/$(*F).tok.c
	@echo '    }'																											>>$(*D)/$(*F).tok.c
	@echo '  }'																												>>$(*D)/$(*F).tok.c
	@echo '  return 0;'																								>>$(*D)/$(*F).tok.c
	@echo '}'																													>>$(*D)/$(*F).tok.c


ff/ff.tokens.c : ff/ff.tok.c
	cp $< $@

ff/ff.tokens.h : ff/ff.tok.h
	cp $< $@

%.c : %.y
%.c : %.l

%.o : %.c
	$(CC) $(COPTS) $(CFLAGS) $< -o $@

# dependencies
main.o  :	${COMMON}/coll.o					\
					args/args.o						\
				  ${COMMON}/pstring.o			\
				  ${COMMON}/unitstring.o		\
				  ${COMMON}/xmem.o					\
				  ${COMMON}/xstring.o			\
					dirutil/dirutil.o			\
					${COMMON}/cksum.o				\
					map/map.o							\
					strstack/strstack.o		\
				  ff/ff.o								\
				  ffn/ffn.o							\
				  bp/bp.o								\
				  fml/fml.o							\
				  gn/gn.o								\
					gn/gnlw.o							\
				  log/log.o							\
				  tmp/tmp.o							\
					list/list.o						\
					var/var.o							\
				  ts/ts.o								\
					dsc/dsc.o							\
					dep/dep.o							\
					identity/identity.o		\
					hashblock/hashblock.o	\
					depblock/depblock.o		\
					canon/canon.o					\
					path/path.o						\
					bake/bake.o						\
					ffproc/ffproc.o				\
					enclose/enclose.o

ff/ff.o : ff/ff.tokens.h ff/ff.tab.o ff/ff.lex.o ff/ff.tokens.o ff/ff.dsc.tab.o

gn/gn.o	: ${COMMON}/coll.o ${COMMON}/unitstring.o

#
# phony targets
#

.PHONY: install uninstall clean

install: fab
	${INSTALL} -d                ${DESTDIR}/usr/local/bin
	${INSTALL} fab               ${DESTDIR}/usr/local/bin/fab
	chown fabsys:fabsys          ${DESTDIR}/usr/local/bin/fab
	chmod u+s                    ${DESTDIR}/usr/local/bin/fab
	chmod g+s                    ${DESTDIR}/usr/local/bin/fab
	${INSTALL} gcc-dep           ${DESTDIR}/usr/local/bin/gcc-dep
	${INSTALL} -d                ${DESTDIR}/var/cache/fab
	chown fabsys:fabsys          ${DESTDIR}/var/cache/fab
	${INSTALL} -d                ${DESTDIR}/var/tmp/fab
	chown fabsys:fabsys          ${DESTDIR}/var/tmp/fab
	${INSTALL} -d                ${DESTDIR}/usr/lib/fab/std
	${INSTALL} fablib/std/c.fab  ${DESTDIR}/usr/lib/fab/std/c.fab
	${INSTALL} fablib/std/l.fab  ${DESTDIR}/usr/lib/fab/std/l.fab
	${INSTALL} fablib/std/y.fab  ${DESTDIR}/usr/lib/fab/std/y.fab

uninstall:
	rm -f                        ${DESTDIR}/fab
	rm -f                        ${DESTDIR}/gcc-dep
	rm -rf                       ${DESTDIR}/var/cache/fab
	rm -rf                       ${DESTDIR}/var/tmp/fab
	rm -rf                       ${DESTDIR}/usr/lib/fab

clean ::
	rm -f fab 1>/dev/null 2>&1 ; true
	find . \( -false $(foreach txt,*.o *.i *.s *.so *.lex.* *.tab.* *.tok.* *.tokens.* exports,-o -name '$(txt)') \) -delete
