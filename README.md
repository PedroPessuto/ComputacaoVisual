# Processamento de Imagens com SDL3

Escala de cinza - Histograma - Equalizacao (HE por CDF)
Aplicativo em C (C99+) usando SDL3, SDL_image e SDL_ttf que:

- Carrega imagens PNG/JPG/BMP informadas pela linha de comando.
- Detecta se a imagem ja esta em escala de cinza; se nao, converte por luminancia.
- Exibe a imagem na janela principal e um painel auxiliar em uma janela secundaria.
- Renderiza histograma, media e desvio-padrao e classifica brilho/contraste.
- Equaliza o histograma (CDF global) e alterna entre versao original e equalizada.

## Sumario

- [Processamento de Imagens com SDL3](#processamento-de-imagens-com-sdl3)
  - [Sumario](#sumario)
  - [Como funciona (visao por etapas)](#como-funciona-visao-por-etapas)
    - [1) Carregamento de imagem](#1-carregamento-de-imagem)
    - [2) Analise e conversao para escala de cinza](#2-analise-e-conversao-para-escala-de-cinza)
    - [3) Interface com duas janelas (SDL3)](#3-interface-com-duas-janelas-sdl3)
    - [4) Analise e exibicao do histograma](#4-analise-e-exibicao-do-histograma)
    - [5) Equalizacao do histograma](#5-equalizacao-do-histograma)
  - [Compilacao](#compilacao)
    - [Linux/macOS](#linuxmacos)
    - [Windows (MSYS2/MinGW sugerido)](#windows-msys2mingw-sugerido)
  - [Execucao](#execucao)
  - [Controles](#controles)
  - [Estrutura do projeto](#estrutura-do-projeto)
  - [Contribuicoes do grupo](#contribuicoes-do-grupo)
    - [Henrique Arabe Neres de Farias - 10410152](#henrique-arabe-neres-de-farias---10410152)
    - [Ian Miranda da Cunha - 10409669](#ian-miranda-da-cunha---10409669)
    - [Pedro Pessuto Rodrigues Ferreira - 10409729](#pedro-pessuto-rodrigues-ferreira---10409729)
  - [Decisoes tecnicas principais](#decisoes-tecnicas-principais)
  - [Limitacoes e proximos passos](#limitacoes-e-proximos-passos)
  - [Item 6 - Salvar imagem (reservado)](#item-6---salvar-imagem-reservado)
  - [Licencas e creditos](#licencas-e-creditos)
  - [Duvidas comuns](#duvidas-comuns)

## Como funciona (visao por etapas)
- [Sumario](#sumario)

### 1) Carregamento de imagem

- Recebe o caminho da imagem via argumentos de linha de comando e carrega com `IMG_Load`.
- Faz log do caminho, trata erros de carregamento e informa dimensoes.

### 2) Analise e conversao para escala de cinza

- Verifica paleta ou percorre pixel a pixel para testar se `r == g == b`.
- Converte para cinza quando necessario usando `Y = 0.2125*R + 0.7154*G + 0.0721*B`, arredondando e preservando o alpha.
- A superficie em cinza passa a ser a base para as etapas seguintes.

### 3) Interface com duas janelas (SDL3)

- Janela principal exibe a imagem atual via textura, redimensionando para caber na area utilizavel do monitor.
- Janela secundaria posicionada ao lado mostra o painel de metricas, histograma e botao.

### 4) Analise e exibicao do histograma

- Calcula histograma 0..255 da imagem em cinza.
- Deriva media, desvio-padrao e classifica brilho (Escura/Media/Clara) e contraste (Baixo/Medio/Alto).
- Renderiza histograma normalizado e textos com SDL_ttf.

### 5) Equalizacao do histograma

- Inclui botao "Equalizar/Original" com estados visuais (normal, hover, pressionado, toggled).
- Constroi LUT pela CDF do histograma, gera superficie equalizada em cache e alterna sem recarregar o arquivo.
- Atualiza textura da janela principal, histograma e metricas ao alternar.

## Compilacao
- [Sumario](#sumario)
Requisitos: SDL3, SDL_image e SDL_ttf instaladas e disponiveis via `pkg-config`.

### Linux/macOS

```sh
gcc -std=c99 -O2 -Wall -Wextra -o app main.c \
  $(pkg-config --cflags --libs sdl3 SDL3_image SDL3_ttf)
```

Algumas distribuicoes usam nomes minusculos nos pacotes:

```sh
gcc -std=c99 -O2 -Wall -Wextra -o app main.c \
  $(pkg-config --cflags --libs sdl3 sdl3-image sdl3-ttf)
```

### Windows (MSYS2/MinGW sugerido)

1. Instale `mingw-w64-x86_64-SDL3`, `mingw-w64-x86_64-SDL3_image` e `mingw-w64-x86_64-SDL3_ttf`.
2. Compile com GCC + pkg-config conforme comandos acima.

## Execucao

```sh
./app caminho/para/imagem.png
```

Se a fonte nao for encontrada, confira `fonts/arial/arial.ttf` ou ajuste o caminho em `main.c`.

## Controles

- Clique no botao (janela secundaria) para alternar entre Equalizar <-> Original.
- `ESC` ou fechar qualquer janela encerra o programa.
- `S` - reservado para salvar imagem (ver Item 6).

## Estrutura do projeto

```text
.
|-- main.c
`-- fonts/
    `-- arial/
        `-- arial.ttf
```

## Contribuicoes do grupo
- [Sumario](#sumario)
### Henrique Arabe Neres de Farias - 10410152

- Etapas 1 e 3: inicializacao/finalizacao SDL3, carregamento com tratamento basico, janelas principal e secundaria.
- Etapa 4: painel do histograma, integracao com SDL_ttf e ajustes de layout.

### Ian Miranda da Cunha - 10409669

- Etapa 2: verificacao detalhada de escala de cinza e conversao por luminancia.
- Etapa 4: refinamentos do painel do histograma e correcoes na renderizacao de texto.
- Etapas 5.1-5.3: botao de equalizacao, estados visuais, LUT por CDF e toggle com atualizacao de metricas.

### Pedro Pessuto Rodrigues Ferreira - 10409729

- Estrutura inicial do projeto, comentarios de equipe e primeira versao da conversao para cinza.
- Checagem de paleta grayscale e ajustes auxiliares na conversao.
- Commits de suporte e documentacao em codigo.

## Decisoes tecnicas principais
- [Sumario](#sumario)
- API booleana do SDL3: `SDL_Init`/`TTF_Init` tratados como booleanos, com log de erro detalhado.
- Conversao por luminancia seguindo o enunciado (coeficientes 0.2125/0.7154/0.0721) e preservacao do alpha.
- Equalizacao global via CDF: LUT 0..255 com `cdf_min`, clamp e cache da superficie equalizada.
- Atualizacao consistente: ao alternar, recria textura, histograma e metricas na janela secundaria.

## Limitacoes e proximos passos
- [Sumario](#sumario)
- Equalizacao global pode realcar ruido e gerar "efeito pente" - comportamento esperado.
- Formatos de imagem pouco usuais nao foram testados.
- Proximos passos sugeridos:
  1. Implementar Item 6 (salvar imagem exibida).
  2. Adicionar `--help` na CLI e permitir customizar fonte via argumento.
  3. Criar Makefile com `pkg-config` para padronizar a compilacao.

## Item 6 - Salvar imagem (reservado)

Requisito pendente: ao pressionar `S`, salvar a imagem atualmente exibida (`current_surface`) em `output_image.png`, sobrescrevendo se existir.
Sugestao de abordagem:

- Capturar `SDL_EVENT_KEY_DOWN` com `S` na janela principal.
- Usar `IMG_SavePNG(current_surface, "output_image.png")` e registrar sucesso/falha no log.
- Apos a implementacao, mover esta secao para "Como funciona" e marcar o projeto como concluido.

## Licencas e creditos

- SDL3, SDL_image e SDL_ttf - consulte as licencas oficiais.
- Fonte do painel: `fonts/arial/arial.ttf` - substitua por fonte adequada ao seu projeto se necessario.

## Duvidas comuns
- [Sumario](#sumario)
#### A janela secundaria nao aparece. O que fazer?
- Certifique-se de que ela nao abriu atras da janela principal; a posicao e calculada com base nos limites do monitor.
#### O botao Equalizar nao muda nada. Como verificar?
- Cheque o log do terminal: se a LUT nao puder ser gerada ou a superficie equalizada falhar, uma mensagem de erro sera emitida e o botao voltara ao estado original.
