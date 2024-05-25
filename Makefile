UNAME_S!=uname -s
UNAME_M!=uname -m

NAME!=cat main.c | grep "const char \*sofname" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
VERSION!=cat main.c | grep "const char \*version" | awk '{print $$5}' | \
	sed "s/\"//g" | sed "s/;//"
PREFIX=/usr/local

.if ${UNAME_S} == "Linux"
PREFIX=/usr
.endif

CC=cc
FILES=main.c
CFLAGS=-Wall -Wextra -g -I/usr/include -I/usr/local/include
LIBS=-lSDL2 -lSDL2_image -lcurl
LDFLAGS=-L/usr/lib -L/usr/local/lib
MANPREFIX=${PREFIX}/share/man

.if ${UNAME_S} == "OpenBSD"
MANPREFIX=${PREFIX}/man
LDFLAGS+=-L/usr/X11R6/lib
CFLAGS+=-I/usr/X11R6/include
.elif ${UNAME_S} == "NetBSD"
LDFLAGS+=-L/usr/X11R7/lib -L/usr/pkg/lib
CFLAGS+=-I/usr/X11R7/include -I/usr/pkg/include
.endif

all:
	${CC} ${CFLAGS} -o ${NAME} ${FILES} ${LDFLAGS} ${LIBS}
	strip ${NAME}

clean:
	rm -f ${NAME}

dist:
	mkdir -p ${NAME}-${VERSION} release/src
	cp -R LICENSE.txt Makefile *.md ${NAME}.desktop ${NAME}.1 *.c ${NAME}-${VERSION}
	tar zcfv release/src/${NAME}-${VERSION}.tar.gz ${NAME}-${VERSION}
	rm -rf ${NAME}-${VERSION}

release-openbsd:
	mkdir -p release/bin
	${CC} ${CFLAGS} -o release/bin/${NAME}-${VERSION}-openbsd-${UNAME_M} ${FILES}\
		-static ${LDFLAGS} -lc -lm -liconv -lsndio -lsamplerate -lX11\
		-lxcb -lXext -lXcursor -lXrender -lXfixes -lXi -lXrandr -lXss -lusbhid -lpthread\
		-ljxl -ljxl_cms -llcms2 -lhwy -lc++ -lc++abi -lbrotlidec\
		-lbrotlicommon -lbrotlienc -lyuv -ltiff -lz -ljpeg -llzma -lzstd -lavif\
		-ldav1d -laom -lsharpyuv -lwebpdemux -lwebp -lnghttp3 -lngtcp2_crypto_quictls\
		-lngtcp2 -lssl -lcrypto -lnghttp2 -lXau -lXdmcp
	strip release/bin/${NAME}-${VERSION}-openbsd-${UNAME_M}

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin\
		${DESTDIR}${MANPREFIX}/man1\
		${DESTDIR}${PREFIX}/share/applications
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	sed "s/VERSION/${VERSION}/g" < ${NAME}.1 > ${DESTDIR}${MANPREFIX}/man1/${NAME}.1
	cp -f ${NAME}.desktop ${DESTDIR}${PREFIX}/share/applications
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/share/applications/${NAME}.desktop
	rm -f ${DESTDIR}${PREFIX}/man1/${NAME}.1
	rm -f ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
