Projeto 1 – Processamento de Imagens
📚 Disciplina

Computação Visual – Ciência da Computação – Mackenzie – Turma 07N – 2025.2

👥 Grupo

Henrique Árabe Neres de Farias – 10410152

Ian Miranda da Cunha – 10409669

Pedro Pessuto Rodrigues Ferreira – 10409729

🎯 Objetivo

Este projeto tem como finalidade implementar, em linguagem C e utilizando a biblioteca SDL3, um software de processamento de imagens capaz de:

Carregar arquivos de imagem nos formatos PNG, JPG e BMP

Detectar e converter imagens para escala de cinza

Calcular e exibir o histograma de intensidades

Mostrar métricas estatísticas (média e desvio padrão)

Classificar a imagem em termos de brilho e contraste

Equalizar o histograma e alternar entre a versão original e a equalizada

Permitir o salvamento da imagem processada no arquivo output_image.png

⚙️ Funcionamento
🔹 Entrada

O programa recebe como argumento na linha de comando o caminho da imagem a ser carregada:

./main.exe imagens/exemplo.png

🔹 Janelas

Janela Principal: exibe a imagem original ou equalizada.

Janela Secundária: mostra o histograma, estatísticas, classificação de brilho/contraste e um botão de alternância.

🔹 Funcionalidades

Conversão automática para escala de cinza (quando necessário)

Histograma interativo, recalculado a cada alteração

Classificação de brilho:

Escura: média < 85

Média: 85 ≤ média < 170

Clara: média ≥ 170

Classificação de contraste:

Baixo: desvio < 40

Médio: 40 ≤ desvio < 80

Alto: desvio ≥ 80

Equalização de histograma via LUT

Botão de alternância entre a versão original e a equalizada

Salvamento rápido da imagem exibida pressionando a tecla S

📊 Processamento de Imagem
🔹 Histograma

Cada pixel em tons de cinza (0–255) é contabilizado no vetor de frequências:

h(i) = quantidade de pixels com intensidade i


O histograma é renderizado como 256 barras normalizadas na janela secundária.

🔹 Estatísticas

Média (µ):

µ = ( Σ(i=0→255) [ i * h(i) ] ) / ( Σ(i=0→255) h(i) )


Variância (σ²):

σ² = ( Σ(i=0→255) [ h(i) * (i - µ)² ] ) / ( Σ(i=0→255) h(i) )


Desvio padrão (σ):
Aproximado via Newton–Raphson para calcular √σ² sem uso de math.h.

🔹 Equalização de Histograma

Construção da CDF acumulada:

cdf(i) = Σ(j=0→i) h(j)


Geração da LUT de transformação:

lut[i] = ((cdf(i) - cdf_min) * 255) / (N - cdf_min)


Aplicação da LUT em cada pixel → obtendo a imagem equalizada.

👨‍💻 Contribuições Individuais

Henrique

Item 1: Entrada do programa e carregamento da imagem

Item 3: Criação das janelas principais e secundárias

Item 4: Histograma, estatísticas, classificação e equalização

Pedro

Item 2: Conversão para escala de cinza e verificação da imagem

Item 6: Salvamento da imagem processada

Ian

Item 2: Detecção de tons de cinza

Item 5: Implementação da interação com o botão de alternância

🚀 Compilação e Execução
🔹 MacOS / Linux
gcc -o main main.c $(pkgconf --cflags --libs sdl3 sdl3-image sdl3-ttf)
./main imagens/exemplo.png

🔹 Windows

Baixar as bibliotecas SDL3, SDL3_image e SDL3_ttf

Colocar as .dll na pasta do projeto

Compilar com:

gcc main.c -I<path_para_includes> -L<path_para_libs> -lSDL3 -lSDL3_image -lSDL3_ttf -o main.exe


Executar:

./main.exe img/exemplo.bmp
