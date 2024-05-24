gaylandを静的リンクする事が無理ですので、
SDL2からgayland対応を取り消す事が必要になります。

# libSDL2
```
odl https://www.libsdl.org/release/SDL2-2.30.3.tar.gz
tar zxfv SDL2-2.30.3.tar.gz
cd SDL2-2.30.3
./configure --enable-static --enable-video-wayland=no --prefix=/usr/pkg
gmake
doas gmake install
```

# libSDL2_image
```
odl https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.tar.gz
tar zxfv SDL2_image-2.8.2.tar.gz
cd SDL2_image-2.8.2
cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr/pkg -DCMAKE_INSTALL_LIBDIR=lib
ninja -C build
doas ninja -C build install
```
