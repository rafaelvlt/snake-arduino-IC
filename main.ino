#include <LedControl.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

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
int estado = 0; // 0 = menu, 1 = jogando, 2 = tela de espera (game over/recordes)
int opcaoMenu = 0;
const int totalOpcoesMenu = 2;
bool menuJaMostrado = false;

// Debounce
unsigned long ultimoApertoBotao = 0;
const long debounceDelay = 200; 

//NOTAS MUSICAIS (FREQUÊNCIAS EM HZ)
#define NOTA_DO4  262  // Dó 4 oitava
#define NOTA_RE4  294  // Ré 4 oitava
#define NOTA_MI4  330  // Mi 4 oitava
#define NOTA_FA4  349  // Fá 4 oitava
#define NOTA_SOL4 392  // Sol 4 oitava
#define NOTA_LA4  440  // Lá 4 oitava
#define NOTA_SI4  494  // Si 4 oitava
#define NOTA_DO5  523  // Dó 5 oitava
#define NOTA_SOL5 784  // Sol 5 oitava

//LÓGICA DE RECORDES
struct Recorde {
  char iniciais[5];
  int pontuacao;
};

#define NUMERO_DE_RECORDES 5
Recorde topPontuacoes[NUMERO_DE_RECORDES];

#define ENDERECO_EEPROM 0

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
  
   // Recordes
  carregarRecordes();

  // Prepara variáveis do jogo
  resetVariaveis();

  
  // Menu inicial
  estado = 0;

}

void loop() {
  //Menu
  if (estado == 0){
    gerenciarMenu();
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
  lcd.clear();
  

  if (opcaoMenu == 0) { 
    lcd.setCursor(0, 0);
    lcd.print(">");
    lcd.setCursor(1, 0);
    lcd.print("Iniciar Jogo");
    lcd.setCursor(1, 1);
    lcd.print("Recordes");
  } else if (opcaoMenu == 1) { 
    lcd.setCursor(1, 0);
    lcd.print("Iniciar Jogo");
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.setCursor(1, 1);
    lcd.print("Recordes");
  }
}
void gerenciarMenu() {
  if (!menuJaMostrado) {
    atualizarMenu();
    menuJaMostrado = true;
  }


  if (millis() - ultimoApertoBotao > debounceDelay) {
    if (digitalRead(botaoDireita) == LOW) {
      ultimoApertoBotao = millis(); 
      opcaoMenu++;
      if (opcaoMenu >= totalOpcoesMenu) opcaoMenu = 0;
      menuJaMostrado = false;
      tone(BUZZER, NOTA_LA4, 50);
    }
    // --- Lê o botão da Esquerda ---
    else if (digitalRead(botaoEsquerda) == LOW) {
      ultimoApertoBotao = millis();
      opcaoMenu--;
      if (opcaoMenu < 0) opcaoMenu = totalOpcoesMenu - 1;
      menuJaMostrado = false;
      tone(BUZZER, NOTA_LA4, 50);
    }

    else if (digitalRead(botaoCentro) == LOW) {
      ultimoApertoBotao = millis();
      switch (opcaoMenu) {
        case 0:
          iniciarSnake();
          break;
        case 1:
          mostrarRecordes();
          break;
      }
    }
  }
}

void iniciarSnake() {
  menuJaMostrado = false;
  som_iniciarJogo();
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

  //verifica colisão com próprio corpo
  for (int i = 0; i < snake_comprimento; i++) {
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      som_colisao();
      delay(750);
      game_over(1);
      return;
    }
  }
  //verifica colisão com inimigo
  for (int i = 0; i < snake2_comprimento; i++) {
    if (snake2X[i] == novo_X && snake2Y[i] == novo_Y) {
      som_colisao();
      delay(750);
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
    som_comerFruta();
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

  //verifica colisão com próprio corpo
  for (int i = 0; i < snake2_comprimento; i++) {
    if (snake2X[i] == novo_X && snake2Y[i] == novo_Y) {
      som_colisao();
      delay(750);
      game_over(2);
      return;
    }
  }
  //verifica colisão com inimigo
  for (int i = 0; i < snake_comprimento; i++) {
    if (snakeX[i] == novo_X && snakeY[i] == novo_Y) {
      som_colisao();
      delay(750);
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
    som_comerFruta();
    pontosP2++;

    lcd.setCursor(12, 0); 
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
void game_over(int jogadorQuePerdeu) {
  menuJaMostrado = false;
  estado = 2;
  som_gameOver();

  for (int m = 0; m < 4; m++) {
    matriz.clearDisplay(m);
  }

  // placar final
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P1:");
  lcd.print(pontosP1);
  lcd.setCursor(8, 0);
  lcd.print("P2:");
  lcd.print(pontosP2);

  lcd.setCursor(0, 1);
  if (jogadorQuePerdeu == 1) {
    lcd.print("P2 Venceu!");
  } else {
    lcd.print("P1 Venceu!");
  }


  // pega vencedor e pontuacao
  int jogadorVencedor = (jogadorQuePerdeu == 1) ? 2 : 1;
  int pontuacaoVencedor = (jogadorVencedor == 1) ? pontosP1 : pontosP2;

  // verifica se é novo recorde
  int posicaoNoRanking = verificarNovoRecorde(pontuacaoVencedor);

  // novo recorde
  if (posicaoNoRanking != -1) {
    inserirIniciais(posicaoNoRanking, pontuacaoVencedor, jogadorVencedor);
  } else {
    delay(4000); 
    resetVariaveis();
    estado = 0;
  }
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

void som_iniciarJogo() {
  tone(BUZZER, NOTA_LA4, 100);
  delay(120);
  tone(BUZZER, NOTA_LA4, 100);
  delay(120);
  tone(BUZZER, NOTA_DO5, 200);
}

void som_comerFruta() {
  tone(BUZZER, NOTA_DO5, 50); 
  delay(55); 
  tone(BUZZER, NOTA_SOL5, 60); 
}

void som_colisao() {
  tone(BUZZER, NOTA_DO4, 70);
  delay(80);                    
  tone(BUZZER, NOTA_DO4, 150); 
}

void som_gameOver() {
  tone(BUZZER, NOTA_SOL4, 250);
  delay(275);
  tone(BUZZER, NOTA_FA4, 250);
  delay(275);
  tone(BUZZER, NOTA_RE4, 300);
}

void salvarRecordes() {
  EEPROM.put(ENDERECO_EEPROM, topPontuacoes);
}

// carrega os recordes da EEPROM quando o arduino liga
void carregarRecordes() {
  
  EEPROM.get(ENDERECO_EEPROM, topPontuacoes);

  // caso não haja recordes, limpa a tabela de lixo da memória
  if (topPontuacoes[0].pontuacao < 0 || topPontuacoes[0].pontuacao > 30000) {
    for (int i = 0; i < NUMERO_DE_RECORDES; i++) {
      strcpy(topPontuacoes[i].iniciais, "---");
      topPontuacoes[i].pontuacao = 0;
    }
    salvarRecordes(); //salva a lista zerada para a próxima vez
  }
}

int verificarNovoRecorde(int novaPontuacao) {
  //percorre a lista de recordes do maior para o menor
  for (int i = 0; i < NUMERO_DE_RECORDES; i++) {
    if (novaPontuacao > topPontuacoes[i].pontuacao) {
      return i; //nova pontuacao é maior
    }
  }
  return -1; //a pontuação não foi alta o suficiente.
}

void adicionarNovoRecorde(int novaPontuacao, char novasIniciais[], int posicao) {
  //move os recordes antigos uma posição para baixo para abrir espaço
  for (int i = NUMERO_DE_RECORDES - 1; i > posicao; i--) {
    topPontuacoes[i] = topPontuacoes[i - 1];
  }

  //insere o novo recorde na posição correta
  strcpy(topPontuacoes[posicao].iniciais, novasIniciais);
  topPontuacoes[posicao].pontuacao = novaPontuacao;
}

void inserirIniciais(int posicao, int pontuacao, int jogador) {
  char iniciais[5] = {'A', 'A', 'A', 'A', '\0'};
  int letraAtual = 0;
  unsigned long ultimoMovimentoJoy = 0;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NOVO RECORDE P");
  lcd.print(jogador);
  lcd.print("!");

  while (letraAtual < 4) {
    lcd.setCursor(5, 1); 
    lcd.print(iniciais);

    // lógica para fazer a letra atual piscar
    lcd.setCursor(5 + letraAtual, 1);
    lcd.print(iniciais[letraAtual]);
    delay(250);
    lcd.setCursor(5 + letraAtual, 1);
    lcd.print(" ");
    delay(200);

    // lógica de debounce para o joystick
    if (millis() - ultimoMovimentoJoy > 250) {
      int joyX = (jogador == 1) ? analogRead(JOY1_X) : analogRead(JOY2_X);
      int joyY = (jogador == 1) ? analogRead(JOY1_Y) : analogRead(JOY2_Y);

      bool moveu = false;
      
     
      if (joyY > LIMITE_JOYSTICK_ALTO) { 
        iniciais[letraAtual]--;        
        moveu = true;
      }
      if (joyY < LIMITE_JOYSTICK_BAIXO) { 
        iniciais[letraAtual]++;         
        moveu = true;
      }

      // cicla
      if (iniciais[letraAtual] > 'Z') iniciais[letraAtual] = 'A';
      if (iniciais[letraAtual] < 'A') iniciais[letraAtual] = 'Z';

     
      if (joyX > LIMITE_JOYSTICK_ALTO) { 
        letraAtual++;                   
        moveu = true;
      }
      
      if (moveu) {
        ultimoMovimentoJoy = millis();
        tone(BUZZER, NOTA_DO5, 30);
      }
    }
  }
  lcd.setCursor(5, 1); 
  lcd.print(iniciais);

  adicionarNovoRecorde(pontuacao, iniciais, posicao);
  salvarRecordes();

  delay(1000);
  resetVariaveis();
  estado = 0;
  menuJaMostrado = false;
}

void mostrarRecordes() {
  estado = 2;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-- RECORDES --");
  
  for(int i = 0; i < NUMERO_DE_RECORDES; i++) {
    lcd.setCursor(0,1);
    lcd.clear(); 
    lcd.setCursor(0,1);
    lcd.print(i + 1);
    lcd.print(". ");
    lcd.print(topPontuacoes[i].iniciais);
    lcd.print(" - ");
    lcd.print(topPontuacoes[i].pontuacao);
    delay(2000);
  }
  estado = 0;
  menuJaMostrado = false;
}