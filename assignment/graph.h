#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//Ladders are just reverse Snakes so no distinction needed
typedef struct Snake {
    bool is_true;
    int to_where;
} Snake;

typedef struct Node {
    int id;
    int* edges;
    Snake snake;
} Node;

void create_gameboard (Node* board, int n, int m, bool exact, int dice_max, FILE* f);

void print_board(Node* board, int n, int m);

void validate_board(Node* board, int n, int m);