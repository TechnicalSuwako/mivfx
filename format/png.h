#ifndef _PNG_H
#define _PNG_H

#include <stdlib.h>
#include <X11/Xlib.h>
#include <png.h>

XImage* read_png(Display *d, const char *filename);

#endif
