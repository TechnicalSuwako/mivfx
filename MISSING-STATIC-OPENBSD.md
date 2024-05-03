# libtiff
```
$ odl http://www.libtiff.org/downloads/tiff-4.6.0t.tar.xz
$ bsdtar -xf tiff-4.6.0t.tar.xz
$ cd tiff-4.6.0t
$ ./configure --enable-static
$ make
$ doas make install
```

# libyuv
```
$ mkdir libyuv
$ cd libyuv
$ odl -o libyuv.tar.gz https://chromium.googlesource.com/libyuv/libyuv/+archive/331c361581896292fb46c8c6905e41262b7ca95f.tar.gz
$ bsdtar -xf libyuv.tar.gz
$ cmake -S . -B build -G Ninja -DBUILD_SHARED_LIBS=OFF
$ cmake --build build
$ doas cmake --install build
```

# libavif
```
$ doas chown -R $(whoami):$(whoami) /usr/ports
$ cd /usr/ports/graphics/libavif
$ make clean
$ sed -i 's/libyuv=ON/libyuv=OFF/" Makefile
$ sed -i 's/SHARED/STATIC/' Makefile
$ sed -i '31i\
		-DBUILD_SHARED_LIBS=OFF \\
' Makefile
$ make
$ make fake
$ doas cp /usr/ports/pobj/libavif-0.11.1/fake-powerpc/usr/local/lib/libavif.a /usr/local/lib
```

# libgtest
```
$ doas chown -R $(whoami):$(whoami) /usr/ports
$ cd /usr/ports/devel/gtest
$ make clean
$ sed -i 's/SHARED_LIBS/STATIC_LIBS/g' Makefile
$ sed -i 's/STATIC_LIBS=ON/SHARED_LIBS=OFF/' Makefile
$ sed -i 's/gtest_build_tests=ON/gtest_build_tests=OFF/' Makefile
$ make
$ make fake
doas cp /usr/ports/pobj/gtest-1.14.0/fake-powerpc/usr/local/lib/*.a /usr/local/lib
```

# libhwy
```
$ doas chown -R $(whoami):$(whoami) /usr/ports
$ cd /usr/ports/devel/highway
$ make clean
$ sed -i 's/SHARED_LIBS/STATIC_LIBS/g' Makefile
$ sed -i 's/STATIC_LIBS=ON/SHARED_LIBS=OFF/' Makefile
$ make
$ make fake
$ doas cp /usr/ports/pobj/highway-1.1.0/fake-amd64/usr/local/lib/*.a /usr/local/lib
```

# libjxl
```
$ doas chown -R $(whoami):$(whoami) /usr/ports
$ cd /usr/ports/graphics/libjxl
$ make clean
$ sed -i 's/SHARED_LIBS/STATIC_LIBS/g' Makefile
$ sed -i 's/-DJPEGXL_FORCE_SYSTEM_GTEST:BOOL=ON/-DJPEGXL_FORCE_SYSTEM_GTEST:BOOL=OFF/' Makefile
$ sed -i '46i\
		-DJPEGXL_STATIC=ON \\
' Makefile
$ doas cp /usr/local/lib/libbrotlicommon-static.a /usr/local/lib/libbrotlicommon.a
$ doas cp /usr/local/lib/libbrotlidec-static.a /usr/local/lib/libbrotlidec.a
$ doas cp /usr/local/lib/libbrotlienc-static.a /usr/local/lib/libbrotlienc.a
$ doas make
$ doas chown -R $(whoami):$(whoami) /usr/ports
$ make fake
$ doas cp /usr/ports/pobj/libjxl-0.10.1/fake-amd64/usr/local/lib/*.a /usr/local/lib
```
