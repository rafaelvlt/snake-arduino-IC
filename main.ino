#include <LedControl.h>
#include <LiquidCrystal_I2C.h>

// === MATRIZ DE LED ===
#define MLED_DIN 51
#define MLED_CLK 52
#define MLED_CS  53
LedControl matriz = LedControl(MLED_DIN, MLED_CLK, MLED_CS, 4); // 4 módulos 8x8 lado a lado
#define MATRIZ_LARGURA 32
#define MATRIZ_ALTURA 8
#define TAMANHO_MAXIMO 256

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço comum 0x27

//Botões
#define botaoEsquerda 2
#define botaoCentro   3
#define botaoDireita  4
// Debounce simples
bool botaoCentroPressionado = false; 
// Estado do jogo: 0 = menu, 1 = jogando
int estado = 0;
// === BUZZER ===
#define BUZZER 9

// === JOYSTICKS ===
#define JOY1_X A0
#define JOY1_Y A1
#define JOY2_X A3
#define JOY2_Y A4
#define LIMITE_JOYSTICK_BAIXO 400
#define LIMITE_JOYSTICK_ALTO  600

// === COBRA 1 ===
int snakeX[TAMANHO_MAXIMO];
int snakeY[TAMANHO_MAXIMO];
int snake_comprimento_inicial = 3;
int snake_comprimento = snake_comprimento_inicial;
int direcao_X = 1;  // Direita
int direcao_Y = 0;

// === COBRA 2 ===
int snake2X[TAMANHO_MAXIMO];
int snake2Y[TAMANHO_MAXIMO];
int snake2_comprimento_inicial = 3;
int snake2_comprimento = snake2_comprimento_inicial;
int direcao2_X = -1; // Esquerda
int direcao2_Y = 0;

// === FRUTA ===
int frutaX, frutaY;

// === TEMPO ===
unsigned long ultimoMovimento = 0;
int movimentoDelay = 150;

// === PONTUAÇÃO ===
int pontosP1 = 0;
int pontosP2 = 0;

// MENU
bool menuJaMostrado = false;

void setup() {
  //Buzzer
  pinMode(BUZZER, OUTPUT);

  // Inicializa matriz
  for (int i = 0; i < 4; i++) {
    matriz.shutdown(i, false);
    matriz.setIntensity(i, 8);
    matriz.clearDisplay(i);
  }

  // Inicializa LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P1:0   P2:0");

  //Botões
  pinMode(botaoEsquerda, INPUT_PULLUP);
  pinMode(botaoCentro,   INPUT_PULLUP);
  pinMode(botaoDireita,  INPUT_PULLUP);

  // Seed de random
  randomSeed(analogRead(A2));

  // Prepara variáveis do jogo
  resetVariaveis();

  
  // Menu inicial
  estado = 0;
  atualizarMenu();
}

void loop() {
  //Menu
  if (estado == 0){
    atualizarMenu();

    if (digitalRead(botaoCentro) == LOW) { 
      botaoCentroPressionado = true;
    }

    if (digitalRead(botaoCentro) == HIGH && botaoCentroPressionado) {
      botaoCentroPressionado = false; 
      iniciarSnake();                
    }
  }

  //Jogo
  else if (estado == 1){
    ler_joystick1();
    ler_joystick2();

    if (millis() - ultimoMovimento > movimentoDelay) {
      ultimoMovimento = millis();

      movimento_snake1();
      movimento_snake2();

      desenhar_matriz();
    }
  }
}

void atualizarMenu() {
  if (estado == 0 && !menuJaMostrado) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Jogar Snake");
    lcd.setCursor(0, 1);
    lcd.print("Press. Centro");
    menuJaMostrado = true;
  }
}

void iniciarSnake() {
  menuJaMostrado = false;
  
  // reset visual
  for (int m = 0; m < 4; m++) matriz.clearDisplay(m);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Iniciando...");
  delay(500);
  lcd.clear();
  lcd.print("P1:0     P2:0");
  
  // reset estado
  estado = 1;
}

/*void mostrarPlacar(long pontos) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Game Over!");
  lcd.setCursor(0, 1); lcd.print("Pts: "); lcd.print(pontos);
  delay(2000);
} */

// === LEITURA JOYSTICK P1 ===
void ler_joystick1() {
  int x = analogRead(JOY1_X);
  int y = analogRead(JOY1_Y);

  if (abs(x - 512) < 80 && abs(y - 512) < 80) return;

  if (x < LIMITE_JOYSTICK_BAIXO && direcao_X != 1) { 
    direcao_X = -1; direcao_Y = 0; 
  } else if (x > LIMITE_JOYSTICK_ALTO && direcao_X != -1) { 
    direcao_X = 1; direcao_Y = 0; 
  } else if (y < LIMITE_JOYSTICK_BAIXO && direcao_Y != -1) {
    direcao_X = 0; direcao_Y = 1; 
  } else if (y > LIMITE_JOYSTICK_ALTO && direcao_Y != 1) {
    direcao_X = 0; direcao_Y = -1;
  }
}

// === LEITURA JOYSTICK P2 ===
void ler_joystick2() {
  int x = analogRead(JOY2_X);
  int y = analogRead(JOY2_Y);

  if (abs(x - 512) < 80 && abs(y - 512) < 80) return;

  if (x < LIMITE_JOYSTICK_BAIXO && direcao2_X != 1) { 
    direcao2_X = -1; direcao2_Y = 0; 
  } else if (x > LIMITE_JOYSTICK_ALTO && direcao2_X != -1) { 
    direcao2_X = 1; direcao2_Y = 0; 
  } else if (y < LIMITE_JOYSTICK_BAIXO && direcao2_Y != -1) {
    direcao2_X = 0; direcao2_Y = 1; 
  } else if (y > LIMITE_JOYSTICK_ALTO && direcao2_Y != 1) {
    direcao2_X = 0; direcao2_Y = -1;
  }
}

// === MOVIMENTO COBRA 1 ===
void movimento_snake1() {
  int novo_X = snakeX[0] + direcao_X;
  int novo_Y = snakeY[0] + direcao_Y;

  if (novo_X < 0) novo_X = MATRIZ_LARGURA - 1;
  else if (novo_X >= MATRIZ_LARGURA) novo_X = 0;
  if (novo_Y < 0) novo_Y = MATRIZ_ALTURA - 1;
  else if (novo_Y >= MATRIZ_ALTURA) novo_Y = 0;

  for (int i = 0; i < snake_comprimento; i++) {
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      game_over(1);
      return;
    }
  }
  for (int i = 0; i < snake2_comprimento; i++) {
    if (snake2X[i] == novo_X && snake2Y[i] == novo_Y) {
      game_over(1);
      return;
    }
  }

  for (int i = snake_comprimento; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] = novo_X;
  snakeY[0] = novo_Y;

  if (novo_X == frutaX && novo_Y == frutaY) {
    tone(BUZZER, 1000, 100);
    pontosP1++;

    //pontuacao
    lcd.setCursor(3, 0); 
    lcd.print(pontosP1); 

    if (snake_comprimento < TAMANHO_MAXIMO) snake_comprimento++;
    gerar_fruta();
  }
}

// === MOVIMENTO COBRA 2 ===
void movimento_snake2() {
  int novo_X = snake2X[0] + direcao2_X;
  int novo_Y = snake2Y[0] + direcao2_Y;

  if (novo_X < 0) novo_X = MATRIZ_LARGURA - 1;
  else if (novo_X >= MATRIZ_LARGURA) novo_X = 0;
  if (novo_Y < 0) novo_Y = MATRIZ_ALTURA - 1;
  else if (novo_Y >= MATRIZ_ALTURA) novo_Y = 0;

  for (int i = 0; i < snake2_comprimento; i++) {
    if (snake2X[i] == novo_X && snake2Y[i] == novo_Y) {
      game_over(2);
      return;
    }
  }
  for (int i = 0; i < snake_comprimento; i++) {
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      game_over(2);
      return;
    }
  }

  for (int i = snake2_comprimento; i > 0; i--) {
    snake2X[i] = snake2X[i - 1];
    snake2Y[i] = snake2Y[i - 1];
  }
  snake2X[0] = novo_X;
  snake2Y[0] = novo_Y;

  if (novo_X == frutaX && novo_Y == frutaY) {
    tone(BUZZER, 1200, 100);
    pontosP2++;

    lcd.setCursor(11, 0); 
    lcd.print(pontosP2);  

    if (snake2_comprimento < TAMANHO_MAXIMO) snake2_comprimento++;
    gerar_fruta();
  }
}

// === FRUTA ===
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
    for (int i = 0; i < snake2_comprimento; i++) {
      if (snake2X[i] == frutaX && snake2Y[i] == frutaY) {
        posicao_valida = false;
        break;
      }
    }
  }
}

// === DESENHO ===
void desenhar_pixel(int x, int y, bool estado) {
  int modulo = 3 - (x / 8);
  int coluna = x % 8;
  matriz.setLed(modulo, y, coluna, estado);
}

void desenhar_matriz() {
  for (int m = 0; m < 4; m++) {
    matriz.clearDisplay(m);
  }

  for (int i = 0; i < snake_comprimento; i++) {
    desenhar_pixel(snakeX[i], snakeY[i], true);
  }
  for (int i = 0; i < snake2_comprimento; i++) {
    desenhar_pixel(snake2X[i], snake2Y[i], true);
  }
  desenhar_pixel(frutaX, frutaY, true);
}

// === GAME OVER ===
void game_over(int jogador) {
  menuJaMostrado = false;
  estado = 2;
  estado = 2;

  //som de derrota
  tone(BUZZER, 200, 2000);
  delay(1000);
  noTone(BUZZER);

  for (int m = 0; m < 4; m++) {
    matriz.clearDisplay(m);
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P1:");
  lcd.print(pontosP1);
  lcd.setCursor(8,0);
  lcd.print("P2:");
  lcd.print(pontosP2);

  lcd.setCursor(0,1);
  if (jogador == 1) {
    lcd.print("P2 Venceu!");
  } else {
    lcd.print("P1 Venceu!");
  }
  delay(4000);
  resetVariaveis();
  estado = 0;
  atualizarMenu();
}

void resetVariaveis() {
  pontosP1 = 0;
  pontosP2 = 0;
  snake_comprimento = snake_comprimento_inicial;
  snake2_comprimento = snake2_comprimento_inicial;

  // Posiciona Cobra 1
  for (int i = 0; i < snake_comprimento; i++) {
    snakeX[i] = 4 - i;
    snakeY[i] = 3;
  }
  // Posiciona Cobra 2
  for (int i = 0; i < snake2_comprimento; i++) {
    snake2X[i] = MATRIZ_LARGURA - 5 + i;
    snake2Y[i] = 4;
  }

  direcao_X = 1;
  direcao_Y = 0;
  direcao2_X = -1;
  direcao2_Y = 0;

  gerar_fruta();
}