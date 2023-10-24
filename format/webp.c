#include "webp.h"

XImage* read_webp(Display *d, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("ファイルを開けられません。");
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  size_t filesize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *data = malloc(filesize);
  fread(data, filesize, 1, fp);
  fclose(fp);

  int width, height;
  uint8_t *imgdata = WebPDecodeRGBA(data, filesize, &width, &height);
  free(data);

  if (imgdata == NULL) {
    fprintf(stderr, "WEBP画像を逆符号化に失敗しました。\n");
    return NULL;
  }

  // RGBAからARGBに交換（X11の為）
  uint32_t *argbdata = malloc(4 * width * height);
  uint32_t *dest = argbdata;
  uint8_t *src = imgdata;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      uint8_t r = *src++;
      uint8_t g = *src++;
      uint8_t b = *src++;
      uint8_t a = *src++;
      *dest++ = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }
  free(imgdata);

  XImage* img = XCreateImage(d, CopyFromParent, 24, ZPixmap, 0, (char*)argbdata, width, height, 32, 0);
  if (img == NULL) {
    free(argbdata);
    fprintf(stderr, "WEBPのXImageを創作に失敗しました。\n");
    return NULL;
  }

  return img;
}
