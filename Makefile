NAME=mivfx
VERSION=0.4.0
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr/local
CC=cc
FILES=main.c
CFLAGS=-Wall -Wextra -g
LIBS=-lSDL2 -lSDL2_image -lcurl
LDFLAGS=-L${PREFIX}/lib
CPPFLAGS=-I${PREFIX}/include

all:
	${CC} ${CFLAGS} ${CPPFLAGS} -o ${NAME} ${FILES} ${LDFLAGS} ${LIBS}

clean:
	rm -f ${NAME}

dist: clean
	mkdir -p ${NAME}-${VERSION}
	cp -R LICENSE.txt Makefile README.md CHANGELOG.md\
		*.c ${NAME}-${VERSION}
	tar zcfv ${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
