// ========= EQUIPE =========
// Henrique Árabe Neres de Farias - 10410152
// Ian Miranda da Cunha  - 10409669
// Pedro Pessuto Rodrigues Ferreira - 10409729
// ==========================

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

static bool is_palette_grayscale(const SDL_Palette *palette);
static bool surface_is_grayscale(SDL_Surface *surface);
static SDL_Surface *convert_grayscale(SDL_Surface *surface);
static SDL_Window* create_main_window(SDL_Surface *image_surface);
static SDL_Renderer* create_renderer(SDL_Window *window);
static SDL_Texture* create_texture_from_surface(SDL_Renderer *renderer, SDL_Surface *surface);
static SDL_Window* create_secondary_window(SDL_Window *main_window, int width, int height);
static SDL_Renderer* create_secondary_renderer(SDL_Window *secondary_window);
static void compute_histogram(SDL_Surface *surface, int histogram[256]);
static void compute_histogram_stats(const int histogram[256], int *total_out, double *mean_out, double *stddev_out);
static void classify_histogram(double mean, double stddev, const char **brightness_out, const char **contrast_out);
static void render_histogram(SDL_Renderer *renderer, int histogram[256], int width, int height, int top);
static void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y);

int main(int argc, char *argv[])
{

  // Verifica se o numero de argumentos da linha de comando esta correto.
  if (argc != 2)
  {
    fprintf(stderr, "Uso correto: %s <caminho_para_imagem.ext>\n", argv[0]);
    return 1;
  }

  const char *image_path = argv[1];
  printf("O programa vai tentar carregar a imagem:  %s\n", image_path);

  if (SDL_Init(SDL_INIT_VIDEO) < 0){
    fprintf(stderr, "Erro ao inicializar a SDL: %s\n", SDL_GetError());
    return 1;
  }

  if (TTF_Init() < 0){
    fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  TTF_Font *font = TTF_OpenFont("fonts/arial/arial.ttf", 16);
  if (!font) {
    fprintf(stderr, "Erro ao carregar fonte: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  printf("Carregando a imagem de: %s\n", image_path);

  // Tenta carregar a imagem do caminho especificado para uma superficie na memoria.
  SDL_Surface *image_surface = IMG_Load(image_path);

  // Trata erro no carregamento da imagem (ex: arquivo nao encontrado, formato invalido).
  if (image_surface == NULL)
  {
    fprintf(stderr, "Erro ao carregar a imagem: %s\n", SDL_GetError());
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  printf("Imagem carregada com sucesso. Dimensoes: %d x %d\n", image_surface->w, image_surface->h);

  // ========== ETAPA 2 ========

  // 2.1) imagem colorida ou escala de cinza?
  bool is_gray = surface_is_grayscale(image_surface);
  printf("A imagem eh [%s]\n", is_gray ? "CINZA" : "COLORIDA");

  // 2.2) conversao para escala de cinza (apenas se necessario)
  if (!is_gray)
  {
    SDL_Surface *converted = convert_grayscale(image_surface);
    if (!converted)
    {
      fprintf(stderr, "Falha ao converter a imagem para escala de cinza: %s\n", SDL_GetError());
      SDL_DestroySurface(image_surface);
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_Quit();
      return 1;
    }

    SDL_DestroySurface(image_surface);
    image_surface = converted;
    is_gray = true;
    printf("Imagem convertida para escala de cinza.\n");
  }

  // TODO: etapas seguintes (GUI, histograma, etc.)

  //3.1 Criar janela principal

  SDL_Window *window = create_main_window(image_surface);
  if(!window){
    SDL_DestroySurface(image_surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = create_renderer(window);
  if(!renderer){
    SDL_DestroyWindow(window);
    SDL_DestroySurface(image_surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Texture *texture = create_texture_from_surface(renderer, image_surface);
  if(!texture){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySurface(image_surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  //3.2 Criar Janela Secundaria

  SDL_Window *secondary_window = create_secondary_window(window, 400, 400);
  if(!secondary_window){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySurface(image_surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *secondary_renderer = create_secondary_renderer(secondary_window);
  if(!secondary_renderer){
    SDL_DestroyWindow(secondary_window);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySurface(image_surface);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  int histogram[256];
  compute_histogram(image_surface, histogram);
  
  int total_pixels_count;
  double mean_intensity;
  double stddev_intensity;
  compute_histogram_stats(histogram, &total_pixels_count, &mean_intensity, &stddev_intensity);

  const char *brightness_classification = "";
  const char *contrast_classification = "";
  classify_histogram(mean_intensity, stddev_intensity, &brightness_classification, &contrast_classification);

  //Loop de eventos para exibir a imagem

  bool running = true;
  while(running){
    SDL_Event e;
    while(SDL_PollEvent(&e)){
      if(e.type == SDL_EVENT_QUIT){
        running = false;
      }else if(e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED){
        running = false;
      }else if(e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE){
        running = false;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    int sec_w, sec_h;
    SDL_GetWindowSize(secondary_window, &sec_w, &sec_h);

    SDL_SetRenderDrawColor(secondary_renderer, 20, 20, 20, 255);
    SDL_RenderClear(secondary_renderer);

    const int text_panel_height = 110;
    const int text_padding = 12;

    SDL_SetRenderDrawColor(secondary_renderer, 40, 40, 40, 220);
    SDL_FRect info_panel = {0, 0, (float)sec_w, (float)text_panel_height};
    SDL_RenderFillRect(secondary_renderer, &info_panel);

    int histogram_height = sec_h - text_panel_height - text_padding;
    if (histogram_height < 10) histogram_height = 10;
    render_histogram(secondary_renderer, histogram, sec_w, histogram_height, text_panel_height + text_padding);

    char buffer[128];

    snprintf(buffer, sizeof(buffer), "Media: %.2f", mean_intensity);
    render_text(secondary_renderer, font, buffer, 12, 12);

    snprintf(buffer, sizeof(buffer), "Desvio Padrao: %.2f", stddev_intensity);
    render_text(secondary_renderer, font, buffer, 12, 37);

    snprintf(buffer, sizeof(buffer), "Brilho: %s", brightness_classification);
    render_text(secondary_renderer, font, buffer, 12, 62);

    snprintf(buffer, sizeof(buffer), "Contraste: %s", contrast_classification);
    render_text(secondary_renderer, font, buffer, 12, 87);

    SDL_RenderPresent(secondary_renderer);
  }
  
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroySurface(image_surface);
  SDL_DestroyRenderer(secondary_renderer);
  SDL_DestroyWindow(secondary_window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  printf("Programa finalizado com sucesso.\n");
  return 0;
}

static bool is_palette_grayscale(const SDL_Palette *palette)
{
  if (!palette)
    return false;

  for (int i = 0; i < palette->ncolors; i++)
  {
    const SDL_Color c = palette->colors[i];
    if (!(c.r == c.g && c.g == c.b))
      return false;
  }

  return true;
}

static SDL_Surface *convert_grayscale(SDL_Surface *surface)
{

  if (!surface)
  {
    SDL_SetError("convert_grayscale: surface nula");
    return NULL;
  }

  SDL_Surface *dst = SDL_DuplicateSurface(surface);
  if (!dst)
  {
    fprintf(stderr, "Falha ao duplicar a superficie: %s\n", SDL_GetError());
    return NULL;
  }

  bool locked = false;
  if (SDL_MUSTLOCK(dst))
  {
    if (!SDL_LockSurface(dst))
    {
      fprintf(stderr, "Falha ao travar a superficie: %s\n", SDL_GetError());
      SDL_DestroySurface(dst);
      return NULL;
    }
    locked = true;
  }

  const int w = dst->w;
  const int h = dst->h;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      Uint8 r, g, b, a;
      if (!SDL_ReadSurfacePixel(dst, x, y, &r, &g, &b, &a))
      {
        fprintf(stderr, "Falha ao ler pixel (%d,%d): %s\n", x, y, SDL_GetError());
        goto fail;
      }

      const float grayf = 0.2126f * (float)r + 0.7152f * (float)g + 0.0722f * (float)b;
      const Uint8 gray = (Uint8)(grayf + 0.5f);

      if (!SDL_WriteSurfacePixel(dst, x, y, gray, gray, gray, a))
      {
        fprintf(stderr, "Falha ao escrever pixel (%d,%d): %s\n", x, y, SDL_GetError());
        goto fail;
      }
    }
  }

  if (locked)
    SDL_UnlockSurface(dst);

  return dst;

fail:
  if (locked)
    SDL_UnlockSurface(dst);

  SDL_DestroySurface(dst);
  return NULL;
}

static bool surface_is_grayscale(SDL_Surface *surface)
{
  if (!surface)
    return false;

  SDL_Palette *palette = SDL_GetSurfacePalette(surface);
  if (palette)
    return is_palette_grayscale(palette);

  bool locked = false;
  if (SDL_MUSTLOCK(surface))
  {
    if (!SDL_LockSurface(surface))
    {
      fprintf(stderr, "Falha ao travar a superficie para leitura: %s\n", SDL_GetError());
      return false;
    }
    locked = true;
  }

  bool grayscale = true;
  Uint8 r, g, b, a;

  for (int y = 0; y < surface->h; y++)
  {
    for (int x = 0; x < surface->w; x++)
    {
      if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a))
      {
        fprintf(stderr, "Falha ao ler pixel da imagem: %s\n", SDL_GetError());
        grayscale = false;
        goto done;
      }

      if (!(r == g && g == b))
      {
        grayscale = false;
        goto done;
      }
    }
  }

done:
  if (locked)
    SDL_UnlockSurface(surface);

  return grayscale;
}

static SDL_Window* create_main_window(SDL_Surface *image_surface){
  int target_w = image_surface->w;
  int target_h = image_surface->h;

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  SDL_Rect usable;
  if (display != 0 && SDL_GetDisplayUsableBounds(display, &usable)){
    float scale_w = (float)usable.w / (float)target_w;
    float scale_h = (float)usable.h / (float)target_h;
    float scale = scale_w < scale_h ? scale_w : scale_h;
    if (scale < 1.0f){
      target_w = (int)(target_w * scale);
      target_h = (int)(target_h * scale);
      if (target_w < 1) target_w = 1;
      if (target_h < 1) target_h = 1;
    }
  }

  SDL_Window *window = SDL_CreateWindow(
    "Janela Principal",
    target_w,
    target_h,
    SDL_WINDOW_RESIZABLE
  );

  if(!window){
    fprintf(stderr, "Falha ao criar janela principal: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  return window;
}

static SDL_Renderer* create_renderer(SDL_Window *window){
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if(!renderer){
    fprintf(stderr, "Falha ao criar renderer: %s\n", SDL_GetError());
    return NULL;
  }
  return renderer;
}

static SDL_Texture* create_texture_from_surface(SDL_Renderer *renderer, SDL_Surface *surface){
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(!texture){
    fprintf(stderr, "Falha ao criar textura: %s\n", SDL_GetError());
    return NULL;
  }
  return texture;
}

static SDL_Window* create_secondary_window(SDL_Window *main_window, int width, int height){
  int main_x, main_y, main_w, main_h;
  SDL_GetWindowPosition(main_window, &main_x, &main_y);
  SDL_GetWindowSize(main_window, &main_w, &main_h);

  SDL_Window *window = SDL_CreateWindow("Janela Secundaria", width, height, 0);
  if(!window){
    fprintf(stderr, "Falha ao criar janela secundaria: %s\n", SDL_GetError());
    return NULL;
  }

  int target_x = main_x + main_w + 10;
  int target_y = main_y;

  SDL_DisplayID display = SDL_GetDisplayForWindow(main_window);
  SDL_Rect bounds;
  if (display != 0 && SDL_GetDisplayBounds(display, &bounds)){
    int max_x = bounds.x + bounds.w - width;
    if (target_x > max_x){
      target_x = main_x - width - 10;
    }
    if (target_x < bounds.x){
      target_x = bounds.x;
    }

    int max_y = bounds.y + bounds.h - height;
    if (target_y > max_y){
      target_y = max_y;
    }
    if (target_y < bounds.y){
      target_y = bounds.y;
    }
  }

  SDL_SetWindowPosition(window, target_x, target_y);
  return window;
}

static SDL_Renderer* create_secondary_renderer(SDL_Window *secondary_window){
  SDL_Renderer *renderer = SDL_CreateRenderer(secondary_window, NULL);
  if(!renderer){
    fprintf(stderr, "Falha ao criar renderer da janela secundaria: %s\n", SDL_GetError());
    return NULL;
  }
  return renderer;
}

static void compute_histogram(SDL_Surface *surface, int histogram[256]){
    if (!surface) {
        for (int i = 0; i < 256; ++i) histogram[i] = 0;
        return;
    }

    for (int i = 0; i < 256; ++i) histogram[i] = 0;

    bool locked = false;
    if (SDL_MUSTLOCK(surface)) {
        if (!SDL_LockSurface(surface)) {
            fprintf(stderr, "Falha ao travar a superficie para histograma: %s\n", SDL_GetError());
            return;
        }
        locked = true;
    }

    const int w = surface->w;
    const int h = surface->h;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            Uint8 r, g, b, a;
            if (!SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a)) {
                fprintf(stderr, "Falha ao ler pixel (%d,%d) para histograma: %s\n", x, y, SDL_GetError());
                if (locked) SDL_UnlockSurface(surface);
                return;
            }

            int intensity = r;

            if (intensity < 0) intensity = 0;
            if (intensity > 255) intensity = 255;

            histogram[intensity]++;
        }
    }

    if (locked) SDL_UnlockSurface(surface);
}

static void compute_histogram_stats(const int histogram[256], int *total_out, double *mean_out, double *stddev_out)
{
    long long total_pixels = 0;
    long long sum_intensities = 0;
    double sum_squared_diffs = 0.0;

    for (int i = 0; i < 256; ++i) {
        total_pixels += histogram[i];
        sum_intensities += (long long)histogram[i] * i;
    }

    if (total_pixels == 0) {
        if (total_out) *total_out = 0;
        if (mean_out) *mean_out = 0.0;
        if (stddev_out) *stddev_out = 0.0;
        return;
    }

    double mean = (double)sum_intensities / (double)total_pixels;

    for (int i = 0; i < 256; ++i) {
        double diff = (double)i - mean;
        sum_squared_diffs += diff * diff * (double)histogram[i];
    }

    double variance = sum_squared_diffs / (double)total_pixels;

    double stddev = variance;
    if (stddev > 0) {
        for(int i = 0; i < 20; i++){
          stddev = 0.5 * (stddev + variance / stddev);
        }
    } else {
        stddev = 0.0;
    }

    if (total_out) *total_out = (int)total_pixels;
    if (mean_out) *mean_out = mean;
    if (stddev_out) *stddev_out = stddev;
}

static void classify_histogram(double mean, double stddev, const char **brightness_out, const char **contrast_out){
  if(mean < 85){
    *brightness_out = "Escura";
  }else if(mean < 170){
    *brightness_out = "Media";
  }else{
    *brightness_out = "Clara";
  }

  if(stddev < 40){
    *contrast_out = "Baixo";
  }else if(stddev < 80){
    *contrast_out = "Medio";
  }else{
    *contrast_out = "Alto";
  }
}

static void render_histogram(SDL_Renderer *renderer, int histogram[256], int width, int height, int top){
  int max_val = 0;
  for (int i = 0; i < 256; i++){
    if(histogram[i] > max_val){
      max_val = histogram[i];
    }
  }
  if(max_val == 0){
    max_val = 1;
  }

  SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

  float bar_width = (float)width / 256.0f;

  for(int i = 0; i < 256; i++){
    float norm_height = (float)histogram[i] / (float)max_val;
    int bar_height = (int)(norm_height * height);
    if (bar_height < 1){
      bar_height = 1;
    }

    SDL_FRect rect;
    rect.x = i * bar_width;
    rect.y = top + (height - bar_height);
    rect.w = bar_width + 1.0f;
    rect.h = bar_height;

    SDL_RenderFillRect(renderer, &rect);
  }
}

static void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y) {
    if (font == NULL || text == NULL || renderer == NULL) {
        fprintf(stderr, "render_text: Font, text ou renderer é nulo.\n");
        return;
    }

    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, strlen(text), white);
    if (text_surface == NULL) {
        fprintf(stderr, "render_text: Erro ao criar surface do texto: %s\n", SDL_GetError());
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_DestroySurface(text_surface);

    if (text_texture == NULL) {
        fprintf(stderr, "render_text: Erro ao criar textura do texto: %s\n", SDL_GetError());
        return;
    }

    float tw, th;
    SDL_GetTextureSize(text_texture, (float*)&tw, (float*)&th);

    SDL_FRect dst = {(float)x, (float)y, tw, th};
    
    SDL_RenderTexture(renderer, text_texture, NULL, &dst);

    SDL_DestroyTexture(text_texture);
}