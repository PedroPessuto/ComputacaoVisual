# Projeto 1 — Processamento de Imagens (SDL3/C)

Computação Visual — Ciência da Computação — Mackenzie — Turma 07N — 2025.2

## 👥 Grupo
- Henrique Árabe Neres de Farias — 10410152  
- Ian Miranda da Cunha — 10409669  
- Pedro Pessuto Rodrigues Ferreira — 10409729  

---

## 🎯 Objetivo
Implementar, em linguagem C e utilizando a biblioteca SDL3, um software de processamento de imagens capaz de:

- Carregar arquivos de imagem nos formatos PNG, JPG e BMP
- Detectar e converter imagens para escala de cinza (quando necessário)
- Calcular e exibir o histograma de intensidades (0–255)
- Mostrar métricas estatísticas (média e desvio padrão)
- Classificar a imagem em termos de brilho e contraste
- Equalizar o histograma e alternar entre a versão original e a equalizada
- Permitir o salvamento da imagem processada no arquivo `output_image.png`

---

## ⚙️ Funcionamento

### Entrada
O programa recebe como argumento na linha de comando o caminho da imagem a ser carregada:
```bash
./main.exe imagens/exemplo.png
```

### Janelas
- Janela Principal: exibe a imagem original ou equalizada.
- Janela Secundária: mostra o histograma, estatísticas, classificação de brilho/contraste e um botão de alternância (toggle).

### Funcionalidades
- Conversão automática para escala de cinza (quando necessário)
- Histograma interativo, recalculado a cada alteração
- Classificação de brilho:
  - Escura: média < 85
  - Média: 85 ≤ média < 170
  - Clara: média ≥ 170
- Classificação de contraste:
  - Baixo: desvio < 40
  - Médio: 40 ≤ desvio < 80
  - Alto: desvio ≥ 80
- Equalização de histograma via LUT
- Botão para alternar entre a versão original e a equalizada
- Salvamento rápido da imagem exibida pressionando a tecla S

---

## 📊 Processamento de Imagem

### Histograma
Para uma imagem em tons de cinza (0–255), cada pixel é contabilizado no vetor de frequências `h`:

- Definição:
  ```
  h(i) = quantidade de pixels com intensidade i, para i ∈ [0, 255]
  ```
- Renderização:
  - O histograma é desenhado como 256 barras normalizadas na janela secundária, ajustando a altura ao espaço disponível.

### Estatísticas

- Média (μ):
  ```
  μ = ( Σ[i=0→255] [ i * h(i) ] ) / ( Σ[i=0→255] h(i) )
  ```

- Variância (σ²):
  ```
  σ² = ( Σ[i=0→255] [ h(i) * (i - μ)² ] ) / ( Σ[i=0→255] h(i) )
  ```

- Desvio padrão (σ):  
  Calculado sem `math.h`, usando aproximação via método de Newton–Raphson para √σ².

### Equalização de Histograma

1) CDF (função de distribuição acumulada):
```
cdf(i) = Σ[j=0→i] h(j)
```

2) Geração da LUT (tabela de transformação):
```
lut[i] = ((cdf(i) - cdf_min) * 255) / (N - cdf_min)
```
- Onde:
  - `cdf_min` = menor valor de CDF > 0
  - `N` = número total de pixels

3) Aplicação:
- Para cada pixel `p`, substitui-se por `lut[p]`, obtendo a imagem equalizada.

---

## 🧠 Detalhes de Implementação

- Detecção e conversão para escala de cinza:
  - Caso a imagem carregada seja colorida (RGB), é aplicada uma conversão para tons de cinza (ex.: ponderada por luminância ou média simples).
- Cálculo do histograma:
  - Varre-se a imagem em tons de cinza acumulando frequências em `h[256]`.
- Estatísticas e classificação:
  - Após a construção do histograma, são calculadas média e desvio padrão, em seguida aplicando-se as regras de classificação de brilho e contraste.
- Equalização:
  - A CDF é normalizada para [0, 255] e aplicada via LUT.
- Interatividade:
  - Um botão na janela secundária alterna a exibição entre a versão original e a equalizada.
  - A tecla `S` salva a imagem atualmente exibida como `output_image.png`.

---

## 🖥️ Compilação e Execução

### macOS / Linux
Pré-requisitos:
- GCC ou Clang
- SDL3, SDL3_image, SDL3_ttf
- `pkgconf` (ou `pkg-config`) configurado

Comando sugerido:
```bash
gcc -o main main.c $(pkgconf --cflags --libs sdl3 sdl3-image sdl3-ttf)
./main imagens/exemplo.png
```
Caso use `pkg-config`, substitua `pkgconf` por `pkg-config`.

### Windows
Pré-requisitos:
- Baixar as bibliotecas SDL3, SDL3_image e SDL3_ttf
- Colocar as .dll na pasta do projeto (mesmo diretório do executável)

Compilação (exemplo com MinGW):
```bash
gcc main.c -I<path_para_includes> -L<path_para_libs> -lSDL3 -lSDL3_image -lSDL3_ttf -o main.exe
```

Execução:
```bash
./main.exe assets/teste_jpg.jpg
```

Observações:
- Garanta que as DLLs correspondam às versões e arquiteturas corretas (x64 vs x86).

---

## 🧩 Formatos Suportados
- PNG
- JPG/JPEG
- BMP

Ao carregar uma imagem colorida, o programa converte automaticamente para escala de cinza antes de processar o histograma, estatísticas e equalização.

---

## 🕹️ Controles e Interação
- Botão de alternância na janela secundária: alterna entre imagem original e equalizada.
- Tecla `S`: salva a imagem exibida como `output_image.png`.
- Fechar qualquer janela encerra graciosamente a aplicação (eventos de quit do SDL).

---

## 🔢 Regras de Classificação

- Brilho:
  - Escura: `média < 85`
  - Média: `85 ≤ média < 170`
  - Clara: `média ≥ 170`

- Contraste:
  - Baixo: `desvio < 40`
  - Médio: `40 ≤ desvio < 80`
  - Alto: `desvio ≥ 80`

Esses limiares são aplicados após o cálculo do histograma e das estatísticas.

---

## 👨‍💻 Contribuições Individuais

- Henrique
  - Item 1: Entrada do programa e carregamento da imagem
  - Item 3: Criação das janelas principais e secundárias
  - Item 4: Histograma, estatísticas, classificação e equalização

- Pedro
  - Item 2: Conversão para escala de cinza e verificação da imagem
  - Item 6: Salvamento da imagem processada

- Ian
  - Item 2: Detecção de tons de cinza
  - Item 5: Implementação da interação com o botão de alternância

---

## 🧪 Testes Recomendados
- Imagens completamente pretas e completamente brancas
- PNG/JPG/BMP com diferentes dimensões e profundidades

---

## 🧰 Dependências
- SDL3  
- SDL3_image  
- SDL3_ttf

Instalação (Linux/macOS) via gerenciador de pacotes ou compilando a partir do código-fonte. No Windows, baixe os binários e inclua as DLLs no diretório do projeto.

---

## 📝 Execução — Exemplos
```bash
# Linux/macOS
gcc -o main src/main.c $(pkgconf --cflags --libs sdl3 sdl3-image sdl3-ttf)
./main assets/imagens/exemplo.png

# Windows (exemplo)
gcc src/main.c -Ithird_party\SDL3\include -Lthird_party\SDL3\lib -lSDL3 -lSDL3_image -lSDL3_ttf -o main.exe
.\main.exe assets\imagens\exemplo.bmp
```

---
