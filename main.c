#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <curl/curl.h>

#define DELAY_MS 50

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
bool quit = false;
float aspectRatio;
int imgX = 10;
int imgY = 10;
int imgWidth;
int imgHeight;
int screenWidth;
int screenHeight;
int init = 0;
SDL_Rect renderQuad;
float newWidth;
float newHeight;

// マウス
int mouseX = 10;
int mouseY = 10;
int drag = 0;

// ズーム
float zoom = 1.0f;

// 回転
float angle = 0.0f;

// 反転
bool flippedH = false;
bool flippedV = false;

const char* sofname = "mivfx";
const char* version = "0.6.0";

bool dlfile(const char* url, const char* filename) {
  CURL* curl = curl_easy_init();
  if (!curl) {
    return false;
  }

  FILE* file = fopen(filename, "wb");
  if (!file) {
    return false;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  // Clownflareは面倒くさいわね・・・
  curl_easy_setopt(
    curl,
    CURLOPT_USERAGENT,
    "Mozilla/5.0 (Windows NT 10.0; rv:102.0) Gecko/20100101 Firefox/102.0"
  );
  // Pixivも結構面倒くさい
  if (
      strstr("s.pximg.net", url) == 0 ||
      strstr("i.pximg.net", url) == 0
  ) {
    curl_easy_setopt(curl, CURLOPT_REFERER, "https://www.pixiv.net/");
  }
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
  CURLcode res = curl_easy_perform(curl);
  fclose(file);

  if (res != CURLE_OK) {
    remove(filename);
    return false;
  }

  return true;
}

void RenderCopy(SDL_Rect renderQuad) {
  SDL_RenderCopyEx(
    renderer,
    texture,
    NULL,
    &renderQuad,
    angle,
    NULL,
    flippedV ? SDL_FLIP_VERTICAL : (flippedH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)
  );
}

void rotateWindow(int w, int h) {
  if (angle >= 360.0f) angle = 0.0f;
  else if (angle <= -360.0f) angle = 0.0f;
  else if (angle == 270.0f) angle = -90.0f;
  else if (angle == -180.0f) angle = 180.0f;

  imgWidth = h;
  imgHeight = w;
  aspectRatio = (float)imgWidth / imgHeight;
  SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

  SDL_RenderClear(renderer);
  SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
  RenderCopy(renderQuad);
  SDL_RenderPresent(renderer);
}

void windowevent(SDL_Event e) {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  if (e.type == SDL_QUIT) {
    quit = true;
  } else if (e.type == SDL_KEYDOWN) {
    if (e.key.keysym.sym == SDLK_q) {
      quit = true;
    } else if (e.key.keysym.sym == SDLK_e) {
      // GIFアニメーションの停止・続き、0.7.0から追加する予定
    } else if (e.key.keysym.sym == SDLK_r) {
      angle -= 90.0f;
      rotateWindow(imgWidth, imgHeight);
    } else if (e.key.keysym.sym == SDLK_t) {
      angle += 90.0f;
      rotateWindow(imgWidth, imgHeight);
    } else if (e.key.keysym.sym == SDLK_y) {
      SDL_RenderClear(renderer);
      SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

      SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
      RenderCopy(renderQuad);
      flippedV = !flippedV;
      SDL_RenderPresent(renderer);
    } else if (e.key.keysym.sym == SDLK_u) {
      SDL_RenderClear(renderer);
      SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

      SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
      RenderCopy(renderQuad);
      flippedH = !flippedH;
      SDL_RenderPresent(renderer);
    } else if (e.key.keysym.sym == SDLK_o) {
      // 画像をダウンロードする（リモート画像のみ）
    } else if (e.key.keysym.sym == SDLK_p) {
      // 画像をrsync|sftp|http postで使って共有する、0.7.0から追加する予定
    }
  } else if (e.type == SDL_MOUSEBUTTONDOWN) {
    if (e.button.button == SDL_BUTTON_LEFT) {
      drag = 1;
      SDL_GetMouseState(&mouseX, &mouseY);
    }
  } else if (e.type == SDL_MOUSEBUTTONUP) {
    if (e.button.button == SDL_BUTTON_LEFT) {
      drag = 0;
    }
  } else if (e.type == SDL_MOUSEMOTION) {
    if (drag) {
      int newMouseX, newMouseY;
      SDL_GetMouseState(&newMouseX, &newMouseY);
      SDL_RenderClear(renderer);

      if (newWidth != 0.0f) imgWidth = (newWidth);
      if (newHeight != 0.0f) imgHeight = (newHeight);
      imgX = newMouseX - (imgWidth / 2);
      imgY = newMouseY - (imgHeight / 2);
      SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

      SDL_RenderClear(renderer);
      RenderCopy(renderQuad);
      SDL_RenderPresent(renderer);
    }
  } else if (e.type == SDL_MOUSEWHEEL) {
    int curWidth = imgWidth;
    int curHeight = imgHeight;

    float zoomSpeed = 0.1f;
    if (e.wheel.y > 0) {
      zoom += zoomSpeed;
    } else if (e.wheel.y < 0) {
      zoom -= zoomSpeed;
    }

    if (zoom < 0.1f) {
      zoom = 0.1f;
    }

    // 画像のサイズが変わった場合
    newWidth = (float)imgWidth * zoom;
    newHeight = (float)imgHeight * zoom;
    float minLimit = 50.0f;

    // 画像は50x50以下じゃ駄目
    if (newWidth < minLimit) {
      newWidth = minLimit;
      newHeight = curHeight;
    } else if (newHeight < minLimit) {
      printf("%d\n", curWidth);
      newWidth = curWidth;
      newHeight = minLimit;
    }

    // テキスチャーのレンダーリングサイズの設定
    SDL_RenderClear(renderer);

    imgWidth = (int)newWidth;
    imgHeight = (int)newHeight;
    imgX = (windowWidth - imgWidth) / 2;
    imgY = (windowHeight - imgHeight) / 2;
    SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

    SDL_RenderClear(renderer);
    RenderCopy(renderQuad);
    SDL_RenderPresent(renderer);
  } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_RenderClear(renderer);

    // ウィンドウのサイズが変わった場合
    int newWidth = e.window.data1;
    int newHeight = e.window.data2;

    // 縦横比を変わらずに新しい大きさの算数
    float newAspectRatio = (float)newWidth / newHeight;
    int scaledWidth, scaledHeight;

    if (newAspectRatio != aspectRatio) {
      // 画像よりウィンドウの方が広い場合
      scaledHeight = angle == 90 || angle == -90 ? newWidth : newHeight;
      scaledHeight = newHeight;
      scaledWidth = (int)(scaledHeight * aspectRatio);
    } else {
      // 画像よりウィンドウの方が高い場合
      scaledWidth = angle == 90 || angle == -90 ? newHeight : newWidth;
      scaledHeight = angle == 90 || angle == -90 ? newWidth : newHeight;
    }

    // 画像は50x50以下じゃ駄目
    int minWidth = 50;
    int minHeight = 50;
    if (scaledWidth < minWidth) scaledWidth = minWidth;
    if (scaledHeight < minHeight) scaledHeight = minHeight;

    // 大きすぎの場合もふざけんな
    if (scaledWidth >= (screenWidth-20)) scaledWidth = screenWidth-20;
    if (scaledHeight >= (screenHeight-20)) scaledHeight = screenHeight-20;

    imgWidth = scaledWidth;
    imgHeight = scaledHeight;

    // テキスチャーのレンダーリングサイズの設定
    SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

    SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
    RenderCopy(renderQuad);
    SDL_RenderPresent(renderer);
  } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_MOVED) {
    SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

    SDL_RenderClear(renderer);
    RenderCopy(renderQuad);
    SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
    SDL_RenderPresent(renderer);
  } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_EXPOSED) {
    if (init == 1) return;
    init = 1;

    SDL_RenderClear(renderer);

    if (
        (imgWidth >= (screenWidth - 100)) &&
        imgHeight >= (screenHeight - 100)
    ) {
      imgWidth = (screenWidth - 100);
      imgHeight = (screenHeight - 100);
    } else if (
        (imgWidth >= (screenWidth - 100)) &&
        imgHeight <= (screenHeight - 100)
    ) {
      imgWidth = (screenWidth - 100);
      imgHeight = (imgWidth * aspectRatio);
    } else if (
        (imgWidth <= (screenWidth - 100)) &&
        imgHeight >= (screenHeight - 100)
    ) {
      imgHeight = (screenHeight - 100);
      imgWidth = (imgHeight * aspectRatio);
    }

    SDL_Rect renderQuad = { imgX, imgY, imgWidth, imgHeight };

    RenderCopy(renderQuad);
    SDL_SetWindowSize(window, imgWidth + (imgX * 2), imgHeight + (imgY * 2));
    SDL_RenderPresent(renderer);
  }
}

void usage() {
  printf("%s-%s\nusage: %s [file or url]\n", sofname, version, sofname);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    usage();
    return 1;
  }

  // SDL2とSDL2_imageの初期設定
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDLを初期設定出来なかった：%s\n", SDL_GetError());
    return 1;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    printf("SDL_imageを初期設定出来なかった：%s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  const char* imgfile = argv[1];

  // URLの場合、仮にダウンロードして
  bool isurl = strncmp(imgfile, "http://",  7) == 0 ||
               strncmp(imgfile, "https://", 8) == 0;
  char tmpname[] = "/tmp/netimg.png";
  if (isurl) {
    if (!dlfile(imgfile, tmpname)) {
      printf("画像をダウンロードに失敗。URL: %s\n", imgfile);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
      return 1;
    }
    imgfile = tmpname;
  }

  // 画像の読込
  SDL_Surface* imgsurface = IMG_Load(imgfile);
  if (imgsurface == NULL) {
    printf("画像の読込に失敗：%s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // 画像の大きさの受取
  imgWidth = imgsurface->w;
  imgHeight = imgsurface->h;
  aspectRatio = (float)imgWidth / imgHeight;

  // 画面の大きさの受取
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  screenWidth = DM.w;
  screenHeight = DM.h;

  // ウィンドウの大きさの設定
  int windowWidth = (imgWidth >= (screenWidth-20)) ? screenWidth-20 : imgWidth;
  int windowHeight = (imgHeight >= (screenHeight-20)) ? screenHeight-20 : imgHeight;

  // ウィンドウの作成
  SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, "1");
  window = SDL_CreateWindow(
    "mivfx",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    windowWidth,
    windowHeight,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
  );
  if (window == NULL) {
    printf("ウィンドウの作成に失敗：%s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // レンダーの作成
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("レンダーの作成に失敗：%s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  texture = SDL_CreateTextureFromSurface(renderer, imgsurface);
  SDL_FreeSurface(imgsurface);
  if (texture == NULL) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // メインループ
  SDL_Event e;
  while (!quit) {
    // イベント
    while (SDL_PollEvent(&e) != 0) {
      windowevent(e);
    }

    // 休ませる
    SDL_Delay(DELAY_MS);
  }

  // 掃除
  if (isurl) {
    remove(tmpname);
  }
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
