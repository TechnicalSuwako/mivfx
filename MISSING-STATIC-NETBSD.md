gaylandを静的リンクする事が無理ですので、
SDL2からgayland対応を取り消す事が必要になります。

# libSDL2
```
doas su
cd /usr
odl https://ftp.netbsd.org/pub/pkgsrc/stable/pkgsrc.tar.gz
tar zxfv pkgsrc.tar.gz
cd pkgsrc/bootstrap
./bootstrap
cd ../devel
rm -rf SDL2
odl https://076.moe/repo/3rdparty/src/netbsd-SDL2-no-gayland.tar.gz
tar zxfv netbsd-SDL2-no-gayland.tar.gz
rm -rf netbsd-SDL2-no-gayland.tar.gz
cd SDL2
make
make install
```
