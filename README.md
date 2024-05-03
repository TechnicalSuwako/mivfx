# mivfx - minimalist image view for x
ミニマリストな画像ビューアー

# インストールする方法

## OpenBSD
```sh
doas pkg_add sdl2 sdl2_image curl
make
doas make install
```

## FreeBSD
```sh
doas pkg install sdl2 sdl2_image curl
make
doas make install
```

## NetBSD
```sh
doas pkgin install SDL2-2.28.5 SDL2_image curl
make
doas make install
```

## CRUX
```sh
doas prt-get depinst libsdl2 sdl2_image curl bmake
bmake
doas bmake install
```

## Void Linux
```sh
doas xbps-install -S SDL2 SDL2_image curl bmake
bmake
doas bmake install
```

## Artix Linux
```sh
doas pacman -S sdl2 sdl2_image curl bmake
bmake
doas bmake install
```

## Devuan GNU/Linux
```sh
doas apt install libsdl2-dev libsdl2-image-dev libcurl4 bmake
bmake
doas bmake install
```
