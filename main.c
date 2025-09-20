// ========= EQUIPE =========
// Henrique Árabe Neres de Farias - 10410152
// Ian Miranda da Cunha  - 10409669
// Pedro Pessuto Rodrigues Ferreira - 10409729
// ==========================

#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

static bool is_palette_grayscale(const SDL_Palette *palette);
static bool surface_is_grayscale(SDL_Surface *surface);
static SDL_Surface *convert_grayscale(SDL_Surface *surface);

int main(int argc, char *argv[])
{

  // Verifica se o numero de argumentos da linha de comando esta correto.
  if (argc != 2)
  {
    fprintf(stderr, "Uso correto: %s <caminho_para_imagem.ext>\n", argv[0]);
    return 1;
  }

  // Armazena o caminho da imagem do segundo argumento (argv[1]).
  const char *image_path = argv[1];
  printf("O programa vai tentar carregar a imagem:  %s\n", image_path);

  // Inicializa a biblioteca SDL, especificamente o subsistema de video.
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    fprintf(stderr, "Erro ao inicializar a SDL: %s\n", SDL_GetError());
    return 1;
  }

  printf("Carregando a imagem de: %s\n", image_path);

  // Tenta carregar a imagem do caminho especificado para uma superficie na memoria.
  SDL_Surface *image_surface = IMG_Load(image_path);

  // Trata erro no carregamento da imagem (ex: arquivo nao encontrado, formato invalido).
  if (image_surface == NULL)
  {
    fprintf(stderr, "Erro ao carregar a imagem: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  printf("Imagem carregada com sucesso. Dimensoes: %d x %d\n", image_surface->w, image_surface->h);

  // ========== ETAPA 2 ========

  // 2.1) imagem colorida ou escala de cinza?
  bool is_gray = surface_is_grayscale(image_surface);
  printf("A imagem eh [%s]\n", is_gray ? "CINZA" : "COLORIDA");

  // 2.2) TO DO
  SDL_Surface *gray_image = convert_grayscale(image_surface);
  if (!gray_image)
  {
    fprintf(stderr, "Não foi possivel converter %s\n", SDL_GetError());
  }
  else
  {
    printf("Deu certo\n");
  }

  // Libera os recursos alocados antes de finalizar o programa.
  SDL_DestroySurface(gray_image);
  SDL_DestroySurface(image_surface);
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

  // Duplica
  SDL_Surface *dst = SDL_DuplicateSurface(surface);
  if (!dst)
  {
    fprintf(stderr, "Falha ao duplicar a superfície: %s\n", SDL_GetError());
    return NULL;
  }

  // Trava
  if (!SDL_LockSurface(dst))
  {
    fprintf(stderr, "Falha ao travar a superfície: %s\n", SDL_GetError());
    SDL_DestroySurface(dst);
    return NULL;
  }

  const int w = dst->w;
  const int h = dst->h;

  // Sobreescreve o pixel para a escala de cinza
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      Uint8 r, g, b, a;
      if (!SDL_ReadSurfacePixel(dst, x, y, &r, &g, &b, &a))
      {
        fprintf(stderr, "Falha ao ler pixel (%d,%d): %s\n", x, y, SDL_GetError());
        SDL_UnlockSurface(dst);
        SDL_DestroySurface(dst);
        return NULL;
      }

      const float grayf = 0.2126f * (float)r + 0.7152f * (float)g + 0.0722f * (float)b;
      // Aredondamento para inteiro sem truncamento para baixo
      const Uint8 gray = (Uint8)(grayf + 0.5f);

      if (!SDL_WriteSurfacePixel(dst, x, y, gray, gray, gray, a))
      {
        fprintf(stderr, "Falha ao escrever pixel (%d,%d): %s\n", x, y, SDL_GetError());
        SDL_UnlockSurface(dst);
        SDL_DestroySurface(dst);
        return NULL;
      }
    }
  }

  SDL_UnlockSurface(dst);
  return dst;
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