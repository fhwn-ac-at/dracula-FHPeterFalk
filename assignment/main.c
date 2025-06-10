#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graph.h"

int main(int argc, char *argv[]) {

    int opt;
    int n = -1;              // columns
    int m = -1;              // rows
    char* filename = NULL;  // input file for board; -f flag
    bool exact_roll = false; // -e
    int turn_limit = -1;     // -l
    char* endptr;

    while ((opt = getopt(argc, argv, "n:m:f:el:")) != -1) {
        switch (opt) {
            case 'n':
                n = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: -n requires a valid integer!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                m = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: -m requires a valid integer!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                filename = optarg;
                break;
            case 'e':
                exact_roll = true;
                break;
            case 'l':
                turn_limit = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: -l requires a valid integer!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] [-l turnlimit]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (filename == NULL || turn_limit == -1 || m == -1 || n == -1 ||optind < argc) {
        fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] [-l turnlimit]\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (turn_limit <= 0 || m <= 0 || n <= 0) {
        fprintf(stderr, "parameters m, n and l must be greater than  0!\n");
        exit(EXIT_FAILURE);
    }

    FILE* boardfile = fopen(filename, "r");
    if (!boardfile) {
        fprintf(stderr, "Failed to open input file for the game board!");
        exit(EXIT_FAILURE);
    }

    //snakes and ladders is functionally linear
    //therefore although the board is technically 2D, it will be implemented with a 1D array
    int total_squares = n * m;
    Node gameboard[total_squares];

    create_gameboard(gameboard, total_squares, boardfile);

    fclose(boardfile);

}
