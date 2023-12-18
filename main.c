#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("使用方法： %s <画像ファイル>\n", argv[0]);
    return 1;
  }

  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Texture* texture = NULL;

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

  // 画像の読込
  SDL_Surface* imgsurface = IMG_Load(argv[1]);
  if (imgsurface == NULL) {
    printf("画像の読込に失敗：%s\n", IMG_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // 画像の大きさの受取
  int imgWidth = imgsurface->w;
  int imgHeight = imgsurface->h;
  /* float aspectRatio = (float)imgWidth / imgHeight; */

  // 画面の大きさの受取
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  int screenWidth = DM.w;
  int screenHeight = DM.h;

  // 画像は50x50以下じゃ駄目
  int minWidth = 50;
  int minHeight = 50;
  if (imgWidth < minWidth) imgWidth = minWidth;
  if (imgHeight < minHeight) imgHeight = minHeight;

  // ウィンドウの大きさの設定
  int windowWidth = (imgWidth > screenWidth) ? screenWidth : imgWidth;
  int windowHeight = (imgHeight > screenHeight) ? screenHeight : imgHeight;

  // ウィンドウの作成
  SDL_SetHint(SDL_HINT_VIDEO_WINDOW_SHARE_PIXEL_FORMAT, "1");
  window = SDL_CreateWindow("mivfx", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
  bool quit = false;
  while (!quit) {
    // イベント
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
        // ウィンドウのサイズが変わった場合
        int newWidth = e.window.data1;
        int newHeight = e.window.data2;

        // 縦横比を変わらずに新しい大きさの算数
        /* float newAspectRatio = (float)newWidth / newHeight; */
        int scaledWidth, scaledHeight;
        /* if (newAspectRatio > aspectRatio) { */
        // 画像よりウィンドウの方が広い場合
        scaledHeight = newHeight;
        scaledWidth = newWidth;
        //= (int)(scaledHeight * aspectRatio);
        /* } else { */
        /*   // 画像よりウィンドウの方が高い場合 */
        /*   scaledWidth = newWidth; */
        /*   scaledHeight = (int)(scaledWidth / aspectRatio); */
        /* } */

        // テキスチャーのれんダーリングサイズの設定
        SDL_Rect renderQuad = { (newWidth - scaledWidth) / 2, (newHeight - scaledHeight) / 2, scaledWidth, scaledHeight };
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
      }
    }

    // 画面の更新
    SDL_RenderClear(renderer);

    // テキスチャーの表示
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // 画面の更新
    SDL_RenderPresent(renderer);
  }

  // 掃除
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
