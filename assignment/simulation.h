#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "graph.h"

typedef struct History {
    int* data;
    int amount;
    int capacity;
} History;

typedef struct Player {
    int id;
    int position;
    History roll_history;
    History snake_history;
} Player;

void init_history(History* hist);

void append_history(History* hist, int data);

void free_history(History* hist);

void init_player(Player* p);

void simulate_game(Player* p, Node* board, int board_size, int dice_max, int fd);