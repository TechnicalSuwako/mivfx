#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "format/png.h"

int XErrorHandlerd(Display *d, XErrorEvent *event) {
  char error_text[120];
  XGetErrorText(d, event->error_code, error_text, sizeof(error_text));
  fprintf(stderr, "Xエラー: %s\n", error_text);
  return 0;
}

XImage* openimg(Display *d, const char *filename) {
  FILE *fp;
  fp = fopen(filename, "rb");
  if (!fp) {
    perror("ファイルを開けられません。");
    return NULL;
  }

  unsigned char buf[16];
  fread(buf, 1, 16, fp);
  fclose(fp);

  if (memcmp(buf, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8) == 0) { // PNG
    return read_png(d, filename);
  }

  fprintf(stderr, "不明なファイル種類。\n");
  return NULL;
}

int main(int argc, char **argv) {
  XSetErrorHandler(XErrorHandlerd);
  if (argc < 2) {
    printf("使用方法： %s <画像ファイル>\n", argv[0]);
    return 1;
  }

  Display *d = XOpenDisplay(NULL);
  if (d == NULL) {
    fprintf(stderr, "画面を開けられません。\n");
    return 1;
  }

  int scr = DefaultScreen(d);
  Window w = XCreateSimpleWindow(d, RootWindow(d, scr), 0, 0, 500, 500, 1, BlackPixel(d, scr), WhitePixel(d, scr));
  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapWindow(d, w);
  XFlush(d);

  GC gc = XCreateGC(d, w, 0, NULL);
  if (gc == NULL) {
    fprintf(stderr, "グラフィックス内容を創作に失敗しました。\n");
    return 1;
  }

  XImage *ximg = openimg(d, argv[1]);
  if (ximg == NULL) {
    fprintf(stderr, "画像を開けられません： %s\n", argv[1]);
    XFreeGC(d, gc);
    XCloseDisplay(d);
    return 1;
  }

  double scale = 1.0;

  while (1) {
    XEvent e;
    XNextEvent(d, &e);

    if (e.type == Expose) {
      int nw = (int)(ximg->width * scale);
      int nh = (int)(ximg->height * scale);
      XPutImage(d, w, gc, ximg, 0, 0, 0, 0, nw, nh);
    }

    if (e.type == KeyPress) {
      XWindowAttributes attrs;
      XGetWindowAttributes(d, w, &attrs);

      KeySym keysym = XLookupKeysym(&e.xkey, 0);

      if (keysym == XK_q) {
        break;
      }
    }
  }

  // 掃除
  XFreeGC(d, gc);
  XDestroyImage(ximg);
  XDestroyWindow(d, w);
  XCloseDisplay(d);

  return 0;
}
