# Connect4-Arduino

# Integrantes do Grupo

- Nicholas Eiti Dan - 14600749
- João Gabriel Araújo de Bastos - 15462633
- Matheus Vieira Fernandes - 14615993
- Fernando Valentim Torres - 15452340

# Descrição do Projeto

O projeto realizado é uma representação do jogo Connect4 utilizando um arduino. O projeto foi feito utilizando duas matrizes de LED simulando o "tabuleiro" de cada jogador, o jogo acontece da seguinte forma, cada jogador joga por turnos sua peça e seu objetivo é fazer com que se alinhem (em qualquer direção) quatro de suas peças, o impecilho é que o oponente pode colocar uma peça dele visando atrapalhar a estratégia rival. Para a nossa versão, acrescentamos uma nova dificuldade: um jogador não consegue ver as peças do outro, apesar de elas continuarem interagindo entre si, o que traz um novo nível estratégico para a partida.

## Debouncing

Quando pressionamos um botão, na prática, o botão oscila temporariamente entre os estados "pressionado" e "não pressionado", uma vez que o contato entre os terminais é criado e rompido várias vezes. Assim, o que deveria ser apenas uma entrada acaba sendo processado como várias entradas consecutivas. Esse fenômeno se chama "bouncing". Para evitar isso, usamos uma técnica de debouncing em hardware, que consiste em adicionar um capacitor em paralelo com cada push button. O capacitor irá carregar enquanto o botão não estiver pressionado, e irá manter o nível de tensão da saída estável por alguns milissegundos após o botão ser pressionado, eliminando o bouncing. Cada capacitor tem uma capacitância de 100nF. Resistores "pull-down" de 10k foram usados para limitar a corrente que passa pelo circuito, estendendo o tempo de descarga dos capacitores. Experimentos no Falstad ajudaram a concluir que esse valor de resistência resulta em uma latência adequada.

# Esquemática do circuito

A esquemática do circuito foi desenvolvida no Eagle (consideramos o Tinkercad, que não tem o IC controlador da matriz de LEDs, e o Wokwi, que não tem os capacitores usados para hardware debouncing).

![Esquemática do circuito](/schematic/schematic.png)

# Imagens

![foto_1](/images/foto_1.jpg)
![foto_2](/images/foto_2.jpg)
![foto_3](/images/foto_3.jpg)

# Vídeo explicativo
[Vídeo](https://www.youtube.com/watch?v=uCsXNSOKcWg)
