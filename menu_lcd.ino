//Botões
#define botaoEsquerda 6  
#define botaoCentro 7
#define botaoDireita 8 


#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);


// Variáveis dos botões para debounce (utilizando bool para reduzir o uso de memória)
bool clicadoEsquerda = 0, liberadoEsquerda = 0;
bool clicadoCentro   = 0, liberadoCentro   = 0;
bool clicadoDireita  = 0, liberadoDireita  = 0;

int opcao = 0;  // 0 = Jogar Snake, 1 = Mostrar Placar

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Jogar Snake");

  pinMode(botaoEsquerda, INPUT);
  pinMode(botaoCentro, INPUT);
  pinMode(botaoDireita, INPUT);
}

void loop() {
  // --- Botão Direita ---
  if (digitalRead(botaoDireita) == HIGH) {
    clicadoDireita = 1;
    liberadoDireita = 0;
  } else {
    liberadoDireita = 1;
  }

  if (clicadoDireita && liberadoDireita) {
    opcao = 1;
    atualizarMenu();
    clicadoDireita = 0;
    liberadoDireita = 0;
  }

  // --- Botão Esquerda ---
  if (digitalRead(botaoEsquerda) == HIGH) {
    clicadoEsquerda = 1;
    liberadoEsquerda = 0;
  } else {
    liberadoEsquerda = 1;
  }

  if (clicadoEsquerda && liberadoEsquerda) {
    opcao = 0;
    atualizarMenu();
    clicadoEsquerda = 0;
    liberadoEsquerda = 0;
  }

  // --- Botão Centro ---
  if (digitalRead(botaoCentro) == HIGH) {
    clicadoCentro = 1;
    liberadoCentro = 0;
  } else {
    liberadoCentro = 1;
  }

  if (clicadoCentro && liberadoCentro) {
    if (opcao == 0) {
      iniciarSnake();
    } else {
      mostrarPlacar();
    }
    atualizarMenu();
    clicadoCentro = 0;
    liberadoCentro = 0;
  }
}

void atualizarMenu() {
  lcd.clear();
  if (opcao == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Jogar Snake");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Mostrar Placar");
  }
}

void iniciarSnake() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando Snake");
  delay(2000);
}

void mostrarPlacar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Placar:");
  lcd.setCursor(0, 1);
  lcd.print("RafaBrawlhallaPlayer: 150 pts"); // Exemplo de Placar
  delay(3000);
}
