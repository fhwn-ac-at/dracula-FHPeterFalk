#include <stdio.h>
#include <stdlib.h>

typedef struct Edge {
    int from;
    int to;
    int weight;
} Edge;

int** create_matrix(int node_number, Edge* edge_list) {

    //allocate array for matrix
    int** matrix = malloc(sizeof(int*)*node_number);
    for (int i= 0; i < node_number; i++) {
        matrix[i] = malloc(sizeof(int)*node_number);
    }

    //fill data
    int el_index = 0;
    while (edge_list[el_index].weight != -1) {
        Edge e = edge_list[el_index];
        matrix[e.from][e.to] = e.weight;
        el_index++;       
    }
    //return finished matrix
    return matrix;
}

void print_matrix(int node_number, int matrix[node_number][node_number]) {
    for (int i = 0; i < node_number; i++) {
        for (int j =0; j < node_number; j++) {
            int value = matrix[i][j];
            printf("%d ", value);
        }
        printf("\n");
    }
}

int main(void) {

    Edge list[] = {
        {0, 1, 10},
        {1, 2, 20},
        {2, 3, 30},
        {3, 4, 40},
        {4, 5, 50},
        {5, 6, 60},
        {6, 7, 70},
        {7, 8, 80},
        {8, 9, 90},
        {-1, -1, -1}, //sentinel
    };

    printf("Test1\n");

    int** my_matrix = create_matrix(7, list);

    printf("test2\n");

    print_matrix(7, my_matrix);
}