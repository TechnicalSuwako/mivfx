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

const char* sofname = "mivfx";
const char* version = "0.5.0";

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

void windowevent(SDL_Event e) {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  if (e.type == SDL_QUIT) {
    quit = true;
  } else if (e.type == SDL_KEYDOWN) {
    if (e.key.keysym.sym == SDLK_q) {
      quit = true;
    } else if (e.key.keysym.sym == SDLK_a) {
      // GIFアニメーションの停止・続き、0.6.0から追加する予定
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

      if (newWidth == 0.0f) renderQuad.w = imgWidth;
      else renderQuad.w = (int)newWidth;
      if (newHeight == 0.0f) renderQuad.h = imgHeight;
      else renderQuad.h = (int)newHeight;
      renderQuad.x = newMouseX - (renderQuad.w / 2);
      renderQuad.y = newMouseY - (renderQuad.h / 2);

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
      SDL_RenderPresent(renderer);
    }
  } else if (e.type == SDL_MOUSEWHEEL) {
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
    if (newWidth < minLimit || newHeight < minLimit) {
      newWidth = minLimit;
      newHeight = minLimit;
    } else if (newWidth < minLimit && newHeight >= minLimit) {
      newWidth = minLimit;
    } else if (newWidth >= minLimit && newHeight < minLimit) {
      newHeight = minLimit;
    }

    // テキスチャーのレンダーリングサイズの設定
    SDL_RenderClear(renderer);

    renderQuad.w = (int)newWidth;
    renderQuad.h = (int)newHeight;
    renderQuad.x = (windowWidth - renderQuad.w) / 2;
    renderQuad.y = (windowHeight - renderQuad.h) / 2;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    SDL_RenderPresent(renderer);
  } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
    // ウィンドウのサイズが変わった場合
    int newWidth = e.window.data1;
    int newHeight = e.window.data2;

    // 縦横比を変わらずに新しい大きさの算数
    float newAspectRatio = (float)newWidth / newHeight;
    int scaledWidth, scaledHeight;
    if (newAspectRatio != aspectRatio) {
      // 画像よりウィンドウの方が広い場合
      scaledHeight = newHeight;
      scaledWidth = (int)(scaledHeight * aspectRatio);
    } else {
      // 画像よりウィンドウの方が高い場合
      scaledWidth = newWidth;
      scaledHeight = newHeight;
    }

    // 画像は50x50以下じゃ駄目
    int minWidth = 50;
    int minHeight = 50;
    if (scaledWidth < minWidth) scaledWidth = minWidth;
    if (scaledHeight < minHeight) scaledHeight = minHeight;

    // 大きすぎの場合もふざけんな
    if (scaledWidth >= (screenWidth-20)) scaledWidth = screenWidth-20;
    if (scaledHeight >= (screenHeight-20)) scaledHeight = screenHeight-20;

    // テキスチャーのレンダーリングサイズの設定
    renderQuad.x = imgWidth;
    renderQuad.y = imgHeight;
    renderQuad.w = scaledWidth;
    renderQuad.h = scaledHeight;

    SDL_SetWindowSize(window, scaledWidth, scaledHeight);
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
  } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_EXPOSED) {
    SDL_Rect renderQuad = { imgWidth, imgHeight, imgWidth, imgHeight };
    SDL_RenderClear(renderer);

    renderQuad.w = (windowWidth - 20);
    renderQuad.h = (windowHeight - 20);
    renderQuad.x = (windowWidth - renderQuad.w) / 2;
    renderQuad.y = (windowHeight - renderQuad.h) / 2;

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

    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    if (init == 0) SDL_SetWindowSize(window, imgWidth + 20, imgHeight + 20);
    init = 1;
    SDL_RenderPresent(renderer);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("%s-%s\nusage: %s <file or url>\n", sofname, version, sofname);
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
