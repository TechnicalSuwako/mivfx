NAME=mivfx
VERSION=0.3.0
# Linux、Haiku、かIllumos = /usr、FreeBSDかOpenBSD = /usr/local、NetBSD = /usr/pkg
PREFIX=/usr
CC=cc
FILES=main.c
CFLAGS=-Wall -Wextra -g
LDFLAGS=-lSDL2 -lSDL2_image

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
