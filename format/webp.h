#ifndef _WEBP_H
#define _WEBP_H

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <webp/decode.h>

XImage* read_webp(Display *d, const char *filename);

#endif
