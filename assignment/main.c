#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "graph.h"
#include "simulation.h"

int main(int argc, char *argv[]) {

    int opt;
    int n = -1;              // columns
    int m = -1;              // rows
    char* filename = NULL;  // input file for board; -f flag
    bool exact_roll = false; // -e
    int turn_limit = -1;     // -l
    int dice_max = -1;  //-d
    char* endptr;

    while ((opt = getopt(argc, argv, "n:m:f:el:d:")) != -1) {
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
            case 'd':
                dice_max = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: -d requires a valid integer!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] -l turnlimit -d dice_max\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (filename == NULL || turn_limit == -1 || m == -1 || n == -1 || dice_max == -1 ||optind < argc) {
        fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] -l turnlimit -d dice_max\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (turn_limit <= 0 || m <= 0 || n <= 0 || dice_max <= 0) {
        fprintf(stderr, "parameters m, n, l and d must be greater than  0!\n");
        exit(EXIT_FAILURE);
    }

    FILE* boardfile = fopen(filename, "r");
    if (!boardfile) {
        fprintf(stderr, "Failed to open input file for the game board!\n");
        exit(EXIT_FAILURE);
    }

    //open file descriptor to read from urandom
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    //snakes and ladders is functionally linear
    //therefore although the board is technically 2D, it will be implemented with a 1D array
    int total_squares = (n * m) + 1; //+1 due to adding a "Start square"
    Node gameboard[total_squares];

    create_gameboard(gameboard, n, m, exact_roll, dice_max, boardfile);

    fclose(boardfile);

    validate_board(gameboard, n, m);

    print_board(gameboard, n, m);

    Player p;
    p.id = 1;
    init_player(&p);

    simulate_game(&p, gameboard, total_squares, dice_max, fd);

    printf("Player id: %d\n", p.id);
    printf("Player rolls:\n");
    for (int i = 0; i<p.roll_history.amount; i++) {
        printf("roll %d: %d\n", i, p.roll_history.data[i]);
    }
    printf("Player Snakes:\n");
    for (int i = 0; i<p.snake_history.amount; i++) {
        printf("Snake %d: %d\n", i, p.snake_history.data[i]);
    }

    for (int i = 100; i > 94; i--) {
        for (int j = 0; j < 6; j++) {
            printf("Square: %d; Edge: %d = %d\n", i, j, gameboard[i].edges[j]);
        }
    }
    
    free_history(&p.roll_history);
    free_history(&p.snake_history);

    free_edges(gameboard, total_squares);

    //close file descriptor
    close(fd);

}
