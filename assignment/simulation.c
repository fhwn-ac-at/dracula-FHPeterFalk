#include "simulation.h"

unsigned int get_urandom_uint(int fd) {
    unsigned int num;

    ssize_t res = read(fd, &num, sizeof(num));
    if (res != sizeof(num)) {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }
    return num;
}

//returns a random number based on the size of the chosen die
int roll_dice(int dice_max, int fd) {
    unsigned int random = get_urandom_uint(fd);
    int roll = (random % dice_max) + 1;
    return roll;
}

//initializes History struct with starting capacity of 8
void init_history(History* hist) {
    hist->capacity = 8;
    hist->amount = 0;
    hist->data = malloc(hist->capacity * sizeof(int));
    if (!hist->data) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

//appends data to dynamic array inside of history struct and updates corresponding struct members, doubles capacity every time the array reaches its limit
void append_history(History* hist, int data) {
    if (hist->amount == hist->capacity) {
        hist->capacity *= 2;
        int* new_data = realloc(hist->data, hist->capacity * sizeof(int));
        if (!new_data) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        hist->data = new_data;
    }
    hist->data[hist->amount++] = data;
}

void free_history(History* hist) {
    free(hist->data);
    hist->data = NULL;
    hist->amount = hist->capacity = 0;
}

void init_player(Player* p) {
    p->position = 0;
    init_history(&p->roll_history);
    init_history(&p->snake_history);
}

void free_player(Player *p) {
    free_history(&p->roll_history);
    free_history(&p->snake_history);
}

void simulate_game(Player *p, Node *board, int board_size,int dice_max, int turn_limit, int fd) {
    int turn = 0;
    while (p->position != board_size-1 && turn < turn_limit) {
        int dice_roll = roll_dice(dice_max, fd);
        p->position = board[p->position].edges[dice_roll-1];
        if (board[p->position].snake.is_true) {
            append_history(&p->snake_history, p->position);
            p->position = board[p->position].snake.to_where;
        }
        append_history(&p->roll_history, dice_roll);
        turn++;
    }
    if (p->position != board_size-1) {p->position = -1;}
}