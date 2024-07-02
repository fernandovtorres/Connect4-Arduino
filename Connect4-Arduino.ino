#include <SPI.h>
#include <ezBuzzer.h>

#include "Arduino.h"
#include "testlib.h"
#include "game.h"

#define CS_B 9
#define CS_A 10
#define DIN 11
#define CLK 13

#define PWD_BUZZER 3

// Endereços de controle (vindos do protocolo SPI)
#define DECODE_MODE 9
#define INTENSITY 10
#define SCAN_LIMIT 11
#define SHUTDOWN 12
#define DISPLAY_TEST 16

/*
    Função que desliga todos os leds da matriz
*/
static void initialize_matrix(uint8_t chip_select) {
    sendData(chip_select, DECODE_MODE, 0x00);
    sendData(chip_select, INTENSITY, 0x00);
    sendData(chip_select, SCAN_LIMIT, 0x0f);

    for (int i = 1; i <= 8; i++) {
        sendData(chip_select, i, B00000000);
    }

    sendData(chip_select, SHUTDOWN, 0x01);
}

/*
    Função que configura os pinos dos pushbuttons;
    
    Configurar os pinos somente como INPUT fazem com que eles sejam configurados incorretamente, 
    assim primeiramente atribuímos os valores dos pinos como LOW para depois configurarmos eles
    como INPUT.
*/
static void setDebouncedInput(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
}

static void victory_sound() {
    // digitalWrite(PWD_BUZZER, uint8_t val);
}

struct game game;

void setup() {
    Serial.begin(9600);

    pinMode(CS_A, OUTPUT);
    pinMode(CS_B, OUTPUT);

    pinMode(PWD_BUZZER, OUTPUT);

    setDebouncedInput(RIGHT_PLAYER_A);
    setDebouncedInput(LEFT_PLAYER_A);
    setDebouncedInput(ENTER_PLAYER_A);
    setDebouncedInput(RIGHT_PLAYER_B);
    setDebouncedInput(LEFT_PLAYER_B);
    setDebouncedInput(ENTER_PLAYER_B);

    SPI.setBitOrder(MSBFIRST);
    SPI.begin();

    initialize_matrix(CS_A);
    initialize_matrix(CS_B);

    for(int j = 0; j < GAME_DIM; j++) {
        for(int i = 0; i < GAME_DIM; i++) {
            game.board[j][i] = NONE;
        }
        game.last_position[j] = -1;
    }

    game.curr_player = 0;
}

uint8_t last_state = 0;

void loop() {
    uint8_t chip_select = game.curr_player == 0 ? CS_A : CS_B;

    struct move *move = make_move(chip_select, &game);

    Serial.print("move (x):");
    Serial.print(move->x);
    Serial.println();

    Serial.print("move (y):");
    Serial.print(move->y);
    Serial.println();

    game.board[move->y][move->x] = (game.curr_player == 0) ?
            RED_CHECKER : YLW_CHECKER;

    update_board(chip_select, &game);

    if (check_win(&game, move)) {
        Serial.print("Jogador ");
        Serial.print(game.curr_player == 0 ? "JOGADOR A" : "JOGADOR B");
        Serial.print(" venceu\r\n");

        victory_sound();

        // Loop infinito
        while(1){

        }
    }

    game.curr_player = !game.curr_player;
}
