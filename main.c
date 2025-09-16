// ========= EQUIPE ========= 
// Henrique Árabe Neres de Farias - 10410152
// Ian Miranda da Cunha  - 10409669
// Pedro Pessuto Rodrigues Ferreira - 10409729
// ==========================

#include <stdio.h>
#include <SDL3/SDL.h>

int main(int argc,char* argv[])
{

  if(argc != 2){
    fprintf(stderr, "Uso correto: %s <caminho_para_imagem.ext>\n", argv[0]);
    return 1;
  }

  const char* image_path = argv[1];
  printf("O programa vai tentar carregar a imagem:  %s\n", image_path);
  
  printf("Programa finalizado com sucesso.\n");
  return 0;
}
