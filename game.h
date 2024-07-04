#ifndef _GAME_H_
#define _GAME_H_

#include <stdint.h>

#define GAME_DIM 8

#define LEFT_PLAYER_A 8
#define ENTER_PLAYER_A 7
#define RIGHT_PLAYER_A 6

#define LEFT_PLAYER_B 5
#define ENTER_PLAYER_B 4
#define RIGHT_PLAYER_B 2

enum position : int8_t {
    NONE = 0,
    RED_CHECKER,
    YLW_CHECKER,
    BOTH
};

struct game {
    enum position board[GAME_DIM][GAME_DIM];
    int8_t last_position[GAME_DIM];
    uint8_t curr_player;
};

struct move {
    int8_t x, y;
};

void update_board(uint8_t chip_select, struct game *game);

struct move* make_move(uint8_t chip_select, struct game *game);

void print_arrow(uint8_t chip_select, uint8_t ind, struct game *game);

int check_win(struct game *game, struct move *mv, uint8_t same_neighbours[8]);

void show_winning_row(struct game *game, struct move *mv, int dir, uint8_t same_neighbours[8]);

#endif
