#include <LedControl.h>

//Matriz de Led
#define MLED_DIN 51
#define MLED_CLK 52
#define MLED_CS  53
LedControl matriz = LedControl(MLED_DIN, MLED_CLK, MLED_CS, 4); // 4 módulos 8x8 lado a lado
#define MATRIZ_LARGURA 32
#define MATRIZ_ALTURA 8
#define TAMANHO_MAXIMO 256

//joystick
#define JOY_X A0
#define JOY_Y A1
#define LIMITE_JOYSTICK_BAIXO 400
#define LIMITE_JOYSTICK_ALTO  600

//variáveis
//A matriz ser até o tamanho máximo garante que sempre haverá espaço pra cobrar expandir
int snakeX[TAMANHO_MAXIMO]; 
int snakeY[TAMANHO_MAXIMO];
int snake_comprimento_inicial = 3; //inicia com 3 de comprimento
int snake_comprimento = snake_comprimento_inicial;
//controle
int direcao_X = 1;
int direcao_Y = 0;

//coordenadas da fruta
int frutaX, frutaY;

//checagem de tempo
unsigned long ultimoMovimento = 0;
int movimentoDelay = 150;

void setup() {
  //inicializa os 4 módulos da matriz de led
  for (int i = 0; i < 4; i++) {
    matriz.shutdown(i, false); //módulo sem economia de energia
    matriz.setIntensity(i, 8); //brilho
    matriz.clearDisplay(i); //limpa display
  }
  //inicia um gerador de números aleatórios que depende de ruídos de um pino
  randomSeed(analogRead(A2));

  //posicão inicial da cobra
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  //gera primeira comida na tela
  gerar_fruta();
}
void loop() {
  //sempre escuta por inputs
  ler_joystick();
  //se tiver passado o tempo até se movimentar, chama funções necessárias para isso
  if (millis() - ultimoMovimento > movimentoDelay) {
    ultimoMovimento = millis();
    movimento_snake();
    desenhar_matriz();
  }
}
void ler_joystick() {
  //leitura do joystick
  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);
  //seta as direções, não consegue mover para a direita se estiver indo pra esquerda inicialmente, etc
  //Esquerda
  if (x < LIMITE_JOYSTICK_BAIXO && direcao_X != 1) { 
    direcao_X = -1; direcao_Y = 0; 
  }
  //Direita
  else if (x > LIMITE_JOYSTICK_ALTO && direcao_X != -1) { 
    direcao_X = 1; direcao_Y = 0; 
  }
  //Cima
  else if (y < LIMITE_JOYSTICK_BAIXO && direcao_Y != -1) {
    direcao_X = 0; direcao_Y = 1; 
  }
  //Baixo
  else if (y > LIMITE_JOYSTICK_ALTO && direcao_Y != 1) {
    direcao_X = 0; direcao_Y = -1;
    }
}
void movimento_snake() {
  int novo_X = snakeX[0] + direcao_X; //nova coordenada do X cabeça
  int novo_Y = snakeY[0] + direcao_Y; //nova coordenada do Y cabeça

  // atravessa a parede, em dúvida se será mantido ou terá game-over
  if (novo_X < 0) novo_X = MATRIZ_LARGURA - 1;

  else if (novo_X >= MATRIZ_LARGURA) novo_X = 0;

  if (novo_Y < 0) novo_Y = MATRIZ_ALTURA - 1;

  else if (novo_Y >= MATRIZ_ALTURA) novo_Y = 0;

  // colidiu com o corpo
  for (int i = 0; i < snake_comprimento; i++) {
    //se a proxima coordenada da cobra é igual o corpo, chama game_over e retorna a função para não ter movimento
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      game_over();
      return;
    }
  }
  //movimentação do corpo da cobra, o segmento do indice posterior assume o do indice anterior
  for (int i = snake_comprimento; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  //movimenta a cabeça
  snakeX[0] = novo_X;
  snakeY[0] = novo_Y;

  // comeu
  if (novo_X == frutaX && novo_Y == frutaY) {
    if (snake_comprimento < TAMANHO_MAXIMO) snake_comprimento++;
    gerar_fruta();
  }
}
void gerar_fruta() {
  bool posicao_valida = false;
  //enquanto a posicao da nova fruta não for válida, continua gerando
  while (!posicao_valida) {
    frutaX = random(0, MATRIZ_LARGURA);
    frutaY = random(0, MATRIZ_ALTURA);
    posicao_valida = true;
    for (int i = 0; i < snake_comprimento; i++) {
      if (snakeX[i] == frutaX && snakeY[i] == frutaY) {
        posicao_valida = false;
        break;
      }
    }
  }
}
void desenhar_pixel(int x, int y, bool estado) {
  //em qual dos 4 módulos da matriz de LED o pixel deve ser desenhado
  int modulo = x / 8;
  //descobre a coluna do módulo que o pixel deve ser desenhado
  int coluna = x % 8;
  //acende o led na coordenada
  matriz.setLed(modulo, y, coluna, estado);
}
void desenhar_matriz() {
  //apaga todos os leds de todos os módulos
  for (int m = 0; m < 4; m++) {
    matriz.clearDisplay(m);
  }
  //desenha todos os segmentos da cobra
  for (int i = 0; i < snake_comprimento; i++) {
    desenhar_pixel(snakeX[i], snakeY[i], true);
  }
  //desenha a fruta
  desenhar_pixel(frutaX, frutaY, true);
}
void game_over() {
  //apaga todos os módulos da matriz
  for (int m = 0; m < 4; m++) {
    matriz.clearDisplay(m);
  }
  //espera 3 segundos e seta o comprimento pro inicial
  delay(3000);
  snake_comprimento = snake_comprimento_inicial;
  //seta as coordenadas da nova cobra inicial novamente
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  //coloca a snake para ir para direita
  direcao_X = 1;
  direcao_Y = 0;
  gerar_fruta();
}
