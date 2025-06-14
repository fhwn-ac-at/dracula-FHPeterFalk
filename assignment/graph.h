#pragma once

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

//creates a gameboard based on the rules and parameters provided with the command line parameters using the provided gameboard file
void create_gameboard (Node* board, int n, int m, bool exact, int dice_max, FILE* f);

//prints the gameboard (including where Snakes and ladders lead) to stdout
void print_board(Node* board, int n, int m);

//validates the gameboard to ensure that Snakes and ladders are positioned in a manner that aligns with the rules
void validate_board(Node* board, int n, int m);

//frees all dynamically allocated edge arrays inside of the Node structs in the gameboard.
void free_edges(Node* board, int totalsize);