# snake-arduino-IC
![GIF do Jogo Snake em Ação](https://upload.wikimedia.org/wikipedia/commons/5/55/Snake_can_be_completed.gif)
> Um console de jogo dedicado, construído com Arduino, para uma versão do clássico jogo Snake em um display de matriz de LED 32x8.

Este projeto foi desenvolvido por um grupo de estudantes do primeiro período de Engenharia da Computação como requisito para a Feira de Ciências da disciplina de Introdução à Computação.

---

## Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Componentes Necessários](#componentes-necessários-hardware)
- [Diagrama da Montagem](#diagrama-da-montagem)
- [Instalação do Software](#instalação-do-software)
- [Como Jogar](#como-jogar)
- [Equipe](#equipe)
- [Licença](#licença)

---

## Sobre o Projeto

O objetivo principal foi aplicar de forma prática os conhecimentos de hardware e software em um desafio interativo. A ideia foi construir um dispositivo de entretenimento totalmente funcional e autocontido, desde a concepção do circuito até a programação da lógica do jogo, focando em criar uma experiência polida e nostálgica do jogo Snake.

O console utiliza um Arduino Mega como cérebro, um display de matriz de LED 32x8 como tela, e um joystick analógico para a entrada do jogador.


![Colocar circuito montado aqui!!!!!!!!!!]()
> _Foto do Circuito montado._

---

## Componentes Necessários (Hardware)

| Qtd. | Componente |
| :--: | :--- |
| 1 | Placa Arduino Mega 2560 R3 |
| 1 | Módulo Matriz de LED 8x8 com MAX7219 (Kit 4-em-1) |
| 2 | Módulo Joystick Analógico 2 Eixos com Botão |
| 1 | Módulo Display LCD 16x2 com Interface I2C |
| 1 | Buzzer Passivo 5V |
| 1 | Fonte de Alimentação Externa 5V 3A com Plug P4 |
| 1 | Módulo Conector Jack P4 Fêmea para Protoboard |
| 1 | Kit de Jumpers (M-M, M-F e F-F) |
| 2 | Protoboard (Placa de Ensaio) 830 pontos |

---

## Diagrama da Montagem

A montagem do circuito foi desenhada utilizando o software Fritzing para garantir clareza e facilitar a replicação do projeto. A alimentação das matrizes de LED é feita pela fonte externa de 5V, enquanto o Arduino é alimentado via USB, com os terras (GND) de ambos os circuitos conectados para garantir uma referência comum.


![Colocar diagrama do circuito aqui!!!!!!!!]()

> _Esquemático do Circuito._

---

## Instalação do Software

Para rodar o jogo no seu próprio Arduino, siga os passos abaixo:

1.  **Clone o Repositório**
    ```sh
    git clone https://github.com/rafaelvlt/snake-arduino-IC.git
    ```

2.  **Abra na IDE do Arduino**
    Abra o arquivo `.ino` principal do projeto na sua IDE do Arduino.

3.  **Instale as Bibliotecas Necessárias**
    Este projeto requer bibliotecas externas. Instale-as através do Gerenciador de Bibliotecas da IDE (`Ferramentas > Gerenciar Bibliotecas...`):
    -   `MD_Parola`
    -   `MD_MAX72xx`

4.  **Faça o Upload**
    Conecte sua placa Arduino Mega, selecione a porta correta em `Ferramentas > Porta` e clique no botão de Upload.

---

## Como Jogar

1.  Após o upload, o jogo exibirá a tela inicial.
2.  Use o **joystick** para mover a cobra na direção desejada.
3.  O objetivo é "comer" os pontos que aparecem na tela, fazendo a cobra crescer.
4.  O jogo termina se a cobra colidir com as paredes ou com o próprio corpo.
5.  Tente bater o recorde!

---

## Equipe

Este projeto foi idealizado e construído por:

-   [Rafael Barbosa](https://github.com/rafaelvlt)
-   [Caio Amarante](https://github.com/IrineuACgasoso)
---

## Licença

Distribuído sob a licença MIT. Veja o arquivo `LICENSE` para mais informações.
