# snake-arduino-IC
![GIF do Jogo Snake em Ação](https://tenor.com/Y1ob.gif)
> Um console de jogo dedicado, construído com Arduino, para uma versão do clássico jogo Snake em um display de matriz de LED 32x8.

Este projeto foi desenvolvido por um grupo de estudantes do primeiro período de Engenharia da Computação como requisito para a Feira de Ciências da disciplina de Introdução à Computação.

---

## Índice

- [Sobre o Projeto](#sobre-o-projeto)
- [Componentes Necessários](#componentes-necessários-hardware)
- [Diagrama da Montagem e Foto do Circuito Montado](#diagrama-da-montagem-e-foto-do-circuito-montado)
- [Instalação do Software](#instalação-do-software)
- [Como Jogar](#como-jogar)
- [Equipe](#equipe)
- [Licença](#licença)

---

## Sobre o Projeto

O objetivo principal foi aplicar de forma prática os conhecimentos de hardware e software em um desafio interativo. A ideia foi construir um dispositivo de entretenimento totalmente funcional e autocontido, desde a concepção do circuito até a programação da lógica do jogo, focando em criar uma experiência polida e nostálgica do jogo Snake.

O console utiliza um Arduino Mega como cérebro, um display de matriz de LED 32x8 como tela, e um joystick analógico para a entrada do jogador.

![Foto do Console Montado](https://i.imgur.com/link_para_foto_do_seu_projeto.jpg)

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

## Diagrama da Montagem e Foto do Circuito Montado

A montagem do circuito foi desenhada utilizando o software Fritzing para garantir clareza e facilitar a replicação do projeto. A alimentação das matrizes de LED é feita pela fonte externa de 5V, enquanto o Arduino é alimentado via USB, com os terras (GND) de ambos os circuitos conectados para garantir uma referência comum.


COLOCAR DIAGRAMA AQUI!!!!!!!!!!!!!!!!!!

![Diagrama Fritzing do Circuito](https://i.imgur.com/link_para_seu_diagrama.png)

> _Esquemático do Circuito._

COLOCAR FOTO DO CIRCUITO AQUI!!!!

[![Foto do Circuito Montado](https://i.imgur.com/link_para_foto_do_seu_circuito_montado.jpg)](https://i.imgur.com/link_para_foto_do_seu_circuito_montado.jpg)

> _Foto do Circuito montado._

Nesta foto, é possível visualizar a disposição dos componentes na protoboard e as interconexões realizadas com os jumpers. A alimentação das matrizes de LED é feita pela fonte externa de 5V, com a atenção necessária à conexão do terra (GND) em comum com o Arduino (que é alimentado via USB para programação e testes).


---

## Instalação do Software

Para rodar o jogo no seu próprio Arduino, siga os passos abaixo:

1.  **Clone o Repositório**
    ```sh
    git clone [https://github.com/rafaelvlt/snake-arduino-IC.git](https://github.com/rafaelvlt/snake-arduino-IC.git)
    ```
2.  **Abra na IDE do Arduino**
    Abra o arquivo `.ino` principal do projeto na sua IDE do Arduino.

3.  **Instale as Bibliotecas Necessárias**
    Este projeto requer bibliotecas externas. Instale-as através do Gerenciador de Bibliotecas da IDE (`Ferramentas > Gerenciar Bibliotecas...`):
    -   `Biblioteca1`
    -   `Biblioteca2`

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

-   **[Rafael Barbosa <rabs4>]** - [https://github.com/rafaelvlt]
-   **[Nome do Integrante ]** - [Link do GitHub ou LinkedIn]
-   **[Nome do Integrante 3]** - [Link do GitHub ou LinkedIn]
-   **[Nome do Integrante 4]** - [Link do GitHub ou LinkedIn]
-   **[Nome do Integrante 5]** - [Link do GitHub ou LinkedIn]
-   **[Nome do Integrante 6]** - [Link do GitHub ou LinkedIn]

---

## Licença

Distribuído sob a licença MIT. Veja o arquivo `LICENSE` para mais informações.
