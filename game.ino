#include "testlib.h"
#include "game.h"

#include <Arduino.h>

#define CS_B 9
#define CS_A 10

/*
    A maneira com que este array está ordenado, permite que as verificações das combinações sejam
    feitas de maneira linear
*/
const static struct move neighbour_pos[] = {
    { 0,  1 }, {  1,  1 }, {  1,  0 }, {  1, -1 },
    { 0, -1 }, { -1, -1 }, { -1,  0 }, { -1,  1 }
};

/*
    Função que verifica se uma coluna não está cheia, se ela não está, retorna o índice da primeira
    linha não vazia da coluna
*/
static int col_is_not_full(struct game *game, int column)
{
    for (int i = 0; i < GAME_DIM; i++) {
        if (game->board[column][i] == NONE)
            return i;
    }

    return -1;
}

/*
    Verifica se uma posição está vazia ou não
*/
static inline bool is_nonempty_pos(struct game *game, struct move *mv)
{
    if ((mv->x < 0 || mv->y < 0) || (mv->x >= GAME_DIM || mv->y >= GAME_DIM))
        return false;

    return game->board[mv->y][mv->x] != NONE;
}

/*
    Função que verifica se o jogador da rodada venceu o jogo
*/
int check_win(struct game *game, struct move *mv, uint8_t same_neighbours[8])
{
    if (game->board[mv->y][mv->x] == NONE)
        return false;

    for (int i = 0; i < 8; i++) {
        struct move chk_pos = { mv->x, mv->y };

        for (int j = 0; j < 4; j++) {
            chk_pos.x += neighbour_pos[i].x;
            chk_pos.y += neighbour_pos[i].y;

            if (is_nonempty_pos(game, &chk_pos) &&
                    game->board[mv->y][mv->x] == game->board[chk_pos.y][chk_pos.x])
                same_neighbours[i]++;
            else
                break;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (same_neighbours[i] + same_neighbours[i + 4] >= 3)
            return i;
    }

    return -1;
}

void show_winning_row(struct game *game, struct move *mv, int dir, uint8_t same_neighbours[8])
{
    int best_dir = same_neighbours[dir] > same_neighbours[dir + 4] ? dir : dir + 4;

    struct move initial_pos = {
        mv->x + neighbour_pos[best_dir].x * same_neighbours[best_dir],
        mv->y + neighbour_pos[best_dir].y * same_neighbours[best_dir]
    };

    for (int total = same_neighbours[best_dir] + same_neighbours[(best_dir + 4) % 8]; total >= 3; total--) {
        struct move cur_pos;

        for (int i = 0; i < 6; i++) {
            cur_pos = initial_pos;

            for (int j = 0; j < 4; j++) {
                game->board[cur_pos.y][cur_pos.x] = i % 2 == 0 ? BOTH : NONE;
                cur_pos.x += neighbour_pos[(best_dir + 4) % 8].x;
                cur_pos.y += neighbour_pos[(best_dir + 4) % 8].y;
            }

            int tmp = game->curr_player;

            game->curr_player = 0;
            update_board(CS_A, game);
            game->curr_player = 1;
            update_board(CS_B, game);

            game->curr_player = tmp;

            delay(200);
        }

        cur_pos = initial_pos;

        for (int i = 0; i < 4; i++) {
            game->board[cur_pos.y][cur_pos.x] = game->curr_player == 0 ? RED_CHECKER : YLW_CHECKER;

            cur_pos.x += neighbour_pos[(best_dir + 4) % 8].x;
            cur_pos.y += neighbour_pos[(best_dir + 4) % 8].y;
        }

        initial_pos.x += neighbour_pos[(best_dir + 4) % 8].x;
        initial_pos.y += neighbour_pos[(best_dir + 4) % 8].y;
    }

    int tmp = game->curr_player;

    game->curr_player = 0;
    update_board(CS_A, game);
    game->curr_player = 1;
    update_board(CS_B, game);

    game->curr_player = tmp;
}

/*
    Função que imprime um ponteiro no display de leds
*/
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
        buffer <<= 1;
        buffer |= game->board[ind][i] == pos;
    }

    buffer |= B00000001;

    sendData(chip_select, ind+1, buffer);
}

/*
    Função que procura a próxima coluna livre para inserção

    A variável dir pode ser + ou - 1, indicando para qual direção queremos ir (+: direita; -: esquerda)
*/
int find_next_valid_index(struct game *game, int start_ind, int dir) {
    for (int i = (start_ind + dir + GAME_DIM) % GAME_DIM; i != start_ind; i = (i + dir + GAME_DIM) % GAME_DIM) {
        if (game->last_position[i] != 7)
            return i;
    }

    return -1;
}

/*
    Função que interpreta os inputs dos jogadores e registra o movimento feito por eles
*/
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

/*
    Atualiza o estado de somente uma matriz de led selecionada pelo campo chip_select
*/
void update_board(uint8_t chip_select, struct game *game) {
    enum position pos = game->curr_player == 0 ? RED_CHECKER : YLW_CHECKER;

    for (int j = 0; j < GAME_DIM; j++) {
        uint8_t buffer = 0;

        for (int i = 0; i < GAME_DIM; i++) {
            buffer <<= 1;
            buffer |= (game->board[j][i] == pos) || (game->board[j][i] == BOTH);
        }

        sendData(chip_select, j+1, buffer);
    }
}
