#include <LedControl.h>
#include <LiquidCrystal_I2C.h>

//  Matriz de LED (MAX7219) 
#define MLED_DIN 51
#define MLED_CLK 52
#define MLED_CS  53
LedControl matriz = LedControl(MLED_DIN, MLED_CLK, MLED_CS, 4);
#define MATRIZ_LARGURA 32
#define MATRIZ_ALTURA 8
#define TAMANHO_MAXIMO 256

// Joystick 
#define JOY_X A0
#define JOY_Y A1
#define LIMITE_JOYSTICK_BAIXO 400
#define LIMITE_JOYSTICK_ALTO  600

// LCD + Botões
#define botaoEsquerda 2
#define botaoCentro   3
#define botaoDireita  4

LiquidCrystal_I2C lcd(0x20, 16, 2);

// Debounce simples
bool clicadoEsquerda = 0, liberadoEsquerda = 0;
bool clicadoCentro   = 0, liberadoCentro   = 0;
bool clicadoDireita  = 0, liberadoDireita  = 0;

// Estado do jogo: 0 = menu, 1 = jogando
int estado = 0;

// Snake 
int snakeX[TAMANHO_MAXIMO];
int snakeY[TAMANHO_MAXIMO];
int snake_comprimento_inicial = 3;
int snake_comprimento = 3;
int direcao_X = 1, direcao_Y = 0;

// Fruta
int frutaX, frutaY;

// Tempo
unsigned long ultimoMovimento = 0;
int movimentoDelay = 150;

// Pontuação
long pontuacao = 0;

// Protótipos
void ler_joystick();
void movimento_snake();
void gerar_fruta();
void desenhar_pixel(int x, int y, bool estado);
void desenhar_matriz();
void game_over();
void atualizarMenu();
void iniciarSnake();
void mostrarPlacar(long pontos);

void setup() {
  // Matriz
  for (int i = 0; i < 4; i++) {
    matriz.shutdown(i, false);
    matriz.setIntensity(i, 8);
    matriz.clearDisplay(i);
  }

  // LCD + Botões
  lcd.init();
  lcd.backlight();

  pinMode(botaoEsquerda, INPUT);
  pinMode(botaoCentro,   INPUT);
  pinMode(botaoDireita,  INPUT);

  // Joystick
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  // Aleatório
  randomSeed(analogRead(A2));

  // Cobra inicial
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  gerar_fruta();

  // Menu inicial
  estado = 0;
  atualizarMenu();
}

void loop() {
  // --- Botão Centro: inicia o jogo quando estiver no menu ---
  if (digitalRead(botaoCentro) == HIGH) { clicadoCentro = 1; liberadoCentro = 0; }
  else                                   { liberadoCentro = 1; }
  if (clicadoCentro && liberadoCentro) {
    if (estado == 0) iniciarSnake();
    clicadoCentro = 0; liberadoCentro = 0;
  }

  // Loop do jogo
  if (estado == 1) {
    ler_joystick();
    if (millis() - ultimoMovimento > (unsigned long)movimentoDelay) {
      ultimoMovimento = millis();
      movimento_snake();
      desenhar_matriz();
    }
  }
}

void atualizarMenu() {
  if (estado != 0) return;
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Jogar Snake");
  lcd.setCursor(0, 1); lcd.print("Press. Centro");
}

void iniciarSnake() {
  // reset visual
  for (int m = 0; m < 4; m++) matriz.clearDisplay(m);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Iniciando...");
  delay(500);

  // reset estado
  estado = 1;
  pontuacao = 0;
  movimentoDelay = 150;
  snake_comprimento = snake_comprimento_inicial;
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  direcao_X = 1; direcao_Y = 0;
  gerar_fruta();
}

void ler_joystick() {
  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);

  if (x < LIMITE_JOYSTICK_BAIXO && direcao_X != 1) {        // Esquerda
    direcao_X = -1; direcao_Y = 0;
  } else if (x > LIMITE_JOYSTICK_ALTO && direcao_X != -1) {  // Direita
    direcao_X = 1;  direcao_Y = 0;
  } else if (y < LIMITE_JOYSTICK_BAIXO && direcao_Y != -1) { // Cima
    direcao_X = 0;  direcao_Y = 1;
  } else if (y > LIMITE_JOYSTICK_ALTO && direcao_Y != 1) {   // Baixo
    direcao_X = 0;  direcao_Y = -1;
  }
}

void movimento_snake() {
  int novo_X = snakeX[0] + direcao_X;
  int novo_Y = snakeY[0] + direcao_Y;

  // Wrap
  if (novo_X < 0) novo_X = MATRIZ_LARGURA - 1;
  else if (novo_X >= MATRIZ_LARGURA) novo_X = 0;

  if (novo_Y < 0) novo_Y = MATRIZ_ALTURA - 1;
  else if (novo_Y >= MATRIZ_ALTURA) novo_Y = 0;

  // Colisão com o corpo
  for (int i = 0; i < snake_comprimento; i++) {
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      game_over();
      return;
    }
  }

  // Move corpo
  for (int i = snake_comprimento; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  // Nova cabeça
  snakeX[0] = novo_X;
  snakeY[0] = novo_Y;

  // Comeu fruta
  if (novo_X == frutaX && novo_Y == frutaY) {
    if (snake_comprimento < TAMANHO_MAXIMO) snake_comprimento++;
    pontuacao += 10;
    if (movimentoDelay > 60) movimentoDelay -= 3;
    gerar_fruta();
  }
}

void gerar_fruta() {
  bool posicao_valida = false;
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
  int modulo = x / 8;
  int coluna = x % 8;
  matriz.setLed(modulo, y, coluna, estado);
}

void desenhar_matriz() {
  for (int m = 0; m < 4; m++) matriz.clearDisplay(m);
  for (int i = 0; i < snake_comprimento; i++) {
    desenhar_pixel(snakeX[i], snakeY[i], true);
  }
  desenhar_pixel(frutaX, frutaY, true);
}

void mostrarPlacar(long pontos) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Game Over!");
  lcd.setCursor(0, 1); lcd.print("Pts: "); lcd.print(pontos);
  delay(2000);
}

void game_over() {
  // Animação simples
  for (int t = 0; t < 3; t++) {
    for (int m = 0; m < 4; m++) matriz.clearDisplay(m);
    delay(150);
    for (int i = 0; i < snake_comprimento; i++) desenhar_pixel(snakeX[i], snakeY[i], true);
    desenhar_pixel(frutaX, frutaY, true);
    delay(150);
  }

  estado = 0; // volta ao menu
  mostrarPlacar(pontuacao);
  atualizarMenu();

  // Prepara próxima partida
  snake_comprimento = snake_comprimento_inicial;
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  direcao_X = 1; direcao_Y = 0;
  gerar_fruta();
}
