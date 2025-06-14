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

//initializes Player struct; sets position to 0, initializes the History structs inside of that player
void init_player(Player* p);

//frees all dynamic arrays contained in the Player struct
void free_player(Player* p);

//simulates a game according to Snakes and Ladders rules
//Gameboard, the size of the board, the dice used and the maximal amount of turns need to be provided as parameters
//a file descriptor to /dev/urandom is also provided to generate random numbers
//Game data is stored inside the player struct
//if a simulation fails, the player struct will have the position -1
void simulate_game(Player* p, Node* board, int board_size, int dice_max, int turn_limit, int fd);