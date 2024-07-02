#include "testlib.h"
#include "game.h"

#include <Arduino.h>

const static struct move neighbour_pos[] = {
    { 0,  1 }, {  1,  1 }, {  1,  0 }, {  1, -1 },
    { 0, -1 }, { -1, -1 }, { -1,  0 }, { -1,  1 }
};

static int col_is_not_full(struct game *game, int column)
{

    for (int i = 0; i < GAME_DIM; i++) {
        // if (game->board[i][column] == NONE)
        //     return i;
        if (game->board[column][i] == NONE)
            return i;
    }

    return -1;
}

static inline bool is_nonempty_pos(struct game *game, struct move *mv)
{
    if ((mv->x < 0 || mv->y < 0) || (mv->x >= GAME_DIM || mv->y >= GAME_DIM))
        return false;

    return game->board[mv->y][mv->x] != NONE;
}

bool check_win(struct game *game, struct move *mv)
{
    if (game->board[mv->y][mv->x] == NONE)
        return false;

    char same_neighbours[4] = {0};

    for (int i = 0; i < 8; i++) {
        struct move chk_pos = { mv->x, mv->y };

        for (int j = 0; j < 4; j++) {
            chk_pos.x += neighbour_pos[i].x;
            chk_pos.y += neighbour_pos[i].y;

            if (is_nonempty_pos(game, &chk_pos) &&
                    game->board[mv->y][mv->x] == game->board[chk_pos.y][chk_pos.x])
                same_neighbours[i % 4]++;
            else
                break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (same_neighbours[i] >= 3)
            return true;
    }

    return false;
}

void print_arrow(int chip_select, int player, uint8_t ind, struct game *game) {
    // A verificação se a coluna está cheia deve ser feita em outra função
    byte buffer = 0;
    enum position pos;
    if(player == 0) {
        pos = RED_CHECKER;
    }
    else {
        pos = YLW_CHECKER;
    }

    for (int i = 0; i < GAME_DIM; i++) {
        buffer |= ((game->board[ind][7-i] == pos) << i);
    }

    buffer |= B00000001;

    sendData(chip_select, ind+1, buffer);
}

int find_next_valid_index(struct game *game, int start_ind, int dir) {
    for (int i = (start_ind + dir + GAME_DIM) % GAME_DIM; i != start_ind; i = (i + dir + GAME_DIM) % GAME_DIM) {
        if (game->last_position[i] != 7)
            return i;
    }

    return -1;
}

struct move* make_move(uint8_t chip_select, struct game *game) {
    static struct move mv = {0};

    uint8_t start_column;
    uint8_t end_column;

    int ind, tmp;

    for (int j = 0; j < GAME_DIM; j++) {
        if (game->last_position[j] != GAME_DIM-1) {
            start_column = j;
            break;
        }
    }

    for (int j = GAME_DIM-1; j >= 0; j--) {
        if (game->last_position[j] != GAME_DIM-1) {
            end_column = j;
            break;
        }
    }

    ind = start_column;

    uint8_t right_key = game->curr_player == 0 ? RIGHT_PLAYER_A : RIGHT_PLAYER_B;
    uint8_t left_key = game->curr_player == 0 ? LEFT_PLAYER_A : LEFT_PLAYER_B;
    uint8_t enter_key = game->curr_player == 0 ? ENTER_PLAYER_A : ENTER_PLAYER_B;

    uint8_t last_enter = LOW;
    uint8_t curr_enter = LOW;
    uint8_t last_left = LOW;
    uint8_t curr_left = LOW;
    uint8_t last_right = LOW;
    uint8_t curr_right = LOW;

    unsigned long start_millis = millis();
    unsigned long curr_millis = start_millis;

    while((curr_enter = digitalRead(enter_key)) != HIGH) {
        if ((curr_left = digitalRead(left_key)) == HIGH && last_left == LOW) {
            ind = find_next_valid_index(game, ind, -1);
        }
        last_left = curr_left;

        if ((curr_right = digitalRead(right_key)) == HIGH && last_right == LOW) {
            ind = find_next_valid_index(game, ind, 1);
        }
        last_right = curr_right;

        delay(10);

        enum position pos = game->curr_player == 0 ? RED_CHECKER : YLW_CHECKER;

        update_board(chip_select, game);
        
        curr_millis = millis();

        if (curr_millis - start_millis <= 500) {
            print_arrow(chip_select, game->curr_player, ind, game);
        }
        else if (curr_millis - start_millis >= 1000) {
            start_millis = curr_millis;
        }
    }

    mv.y = ind;
    mv.x = game->last_position[ind]+1;

    game->last_position[ind]++;

    return &mv;
}

void update_board(uint8_t chip_select, struct game *game) {
    enum position pos = game->curr_player == 0 ? RED_CHECKER : YLW_CHECKER;

    for (int j = 0; j < GAME_DIM; j++) {
        byte buffer = 0;

        for (int i = 0; i < GAME_DIM; i++) {
            buffer |= ((game->board[j][7-i] == pos) << i);
        }

        sendData(chip_select, j+1, buffer);
    }
}