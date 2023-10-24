NAME=mivfx
VERSION=0.1.0
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr
CC=cc
FILES=main.c format/png.c format/webp.c
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lX11 -lpng -lwebp

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS}

clean:
	rm -f ${NAME}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
