#include "png.h"

XImage* read_png(Display *d, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("ファイルを開けられません。");
    return NULL;
  }
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info = png_create_info_struct(png);

  png_init_io(png, fp);
  png_read_info(png, info);

  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  int bit_depth = png_get_bit_depth(png, info);
  int color_type = png_get_color_type(png, info);

  if (bit_depth == 16) {
    png_set_strip_16(png);
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png);
  }
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }
  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  png_read_update_info(png, info);

  png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
  for (int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png, info));
  }

  png_read_image(png, row_pointers);

  char *imgdata = malloc(4 * width * height);
  char *dst = imgdata;

  for (int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      *dst++ = px[2]; // 青
      *dst++ = px[1]; // 緑
      *dst++ = px[0]; // 赤
      *dst++ = px[3]; // 透明
    }
  }

  // 掃除
  for (int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  png_destroy_read_struct(&png, &info, NULL);
  fclose(fp);

  // XImageを創作
  XImage* img = XCreateImage(d, CopyFromParent, 24, ZPixmap, 0, imgdata, width, height, 32, 0);
  if (img == NULL) {
    free(imgdata);
    fprintf(stderr, "PNGのXImageを創作に失敗しました。\n");
    return NULL;
  }

  return img;
}
