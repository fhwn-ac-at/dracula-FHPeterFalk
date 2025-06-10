#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int id;
    int* edges;
} Node;

void create_gameboard (Node* board, int size, FILE* f);