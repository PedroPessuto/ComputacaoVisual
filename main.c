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

  // 2.1) É colorida ou esacala de cinza?
  bool is_gray = false;
  SDL_Palette *palette = SDL_GetSurfacePalette(image_surface);
  SDL_Surface *converted = NULL;

  if (!palette)
  {
    // Conversão para 8 bits (INDEX8) para garantir que tenha paleta
    converted = SDL_ConvertSurface(image_surface, SDL_PIXELFORMAT_INDEX8);
    palette = SDL_GetSurfacePalette(converted);

    if (palette && converted)
    {
      is_gray = is_palette_grayscale(palette);
      printf("A imagem eh [%s]\n", is_gray ? "CINZA" : "COLORIDA");
    }
    else
    {
      printf("Deu ruim ao pegar a paleta da imagem\n");
    }
  }

  // 2.2) TO DO

  // Libera os recursos alocados antes de finalizar o programa.
  if (converted)
    SDL_DestroySurface(converted);
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