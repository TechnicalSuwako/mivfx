UNAME_S!=uname -s

NAME!=cat main.c | grep "const char\* sofname" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
VERSION!=cat main.c | grep "const char\* version" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
PREFIX=/usr/local

.if ${UNAME_S} == "Linux"
PREFIX=/usr
.elif ${UNAME_S} == "NetBSD"
PREFIX=/usr/pkg
.endif

CC=cc
FILES=main.c
CFLAGS=-Wall -Wextra -g -I${PREFIX}/include
LIBS=-lSDL2 -lSDL2_image -lcurl
LDFLAGS=-L${PREFIX}/lib
MANPREFIX=${PREFIX}/share/man

.if ${UNAME_S} == "OpenBSD"
MANPREFIX=${PREFIX}/man
LDFLAGS+=-L/usr/lib -L/usr/X11R6/lib
CFLAGS+=-I/usr/include -I/usr/X11R6/include
.elif ${UNAME_S} == "FreeBSD"
LDFLAGS+=-L/usr/lib
CFLAGS+=-I/usr/include
.elif ${UNAME_S} == "NetBSD"
LDFLAGS+=-L/usr/lib -L/usr/local/lib -L/usr/X11R7/lib
CFLAGS+=-I/usr/include -I/usr/local/include -I/usr/X11R7/include
.endif

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS} ${LIBS}
	strip ${NAME}

clean:
	rm -f ${NAME}

dist: clean
	mkdir -p ${NAME}-${VERSION} release/src
	cp -R LICENSE.txt Makefile README.md CHANGELOG.md ${NAME}.desktop\
		*.c ${NAME}-${VERSION}
	tar zcfv release/src/${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME}.desktop ${DESTDIR}${PREFIX}/share/applications
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}
	rm -f ${DESTDIR}${PREFIX}/share/applications/${NAME}.desktop

.PHONY: all clean install uninstall
