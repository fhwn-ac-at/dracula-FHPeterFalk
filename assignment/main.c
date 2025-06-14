#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

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
    int sim_amount = -1;
    char* endptr;

    while ((opt = getopt(argc, argv, "n:m:f:el:d:s:")) != -1) {
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
            case 's':
                sim_amount = strtol(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr, "Error: -s requires a valid integer!\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] -l turnlimit -d dice_max -s simulation_amount\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (filename == NULL || turn_limit == -1 || m == -1 || n == -1 || dice_max == -1 || sim_amount == -1 ||optind < argc) {
        fprintf(stderr, "Usage: %s -n cols -m rows -f boardfile [-e] -l turnlimit -d dice_max -s simulation_amount\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (turn_limit <= 0 || m <= 0 || n <= 0 || dice_max <= 0 || sim_amount <= 0) {
        fprintf(stderr, "parameters m, n, l, d and s must be greater than  0!\n");
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

    double roll_average = 0.0;
    int roll_min = INT_MAX;
    int roll_min_id = -1;
    int* roll_min_series = NULL;
    int* snakes_hit = calloc(total_squares, sizeof(int));
    if (!snakes_hit) { perror("malloc"); exit(EXIT_FAILURE); }
    int failure_amount = 0;

    
    for (int i = 0; i < sim_amount; i++) {
        //initialize new player
        Player p;
        p.id = i;
        init_player(&p);

        //run simulation
        simulate_game(&p, gameboard, total_squares, dice_max, turn_limit, fd);
        int rolls = p.roll_history.amount;
        
        if (p.position != -1) {
            //update statistics
            //update smallest amount of rolls
            if (rolls < roll_min) {
                free(roll_min_series);

                roll_min = p.roll_history.amount;
                roll_min_id = p.id;
                roll_min_series = malloc(roll_min * sizeof(int));
                if (!roll_min_series) { perror("malloc"); exit(EXIT_FAILURE); }
                memcpy(roll_min_series, p.roll_history.data, roll_min *sizeof(int));
            }
            //update average
            roll_average += ((double)rolls - roll_average) / (i + 1);
            //update snakes hit
            for (int i = 0; i < p.snake_history.amount; i++) {
                snakes_hit[p.snake_history.data[i]]++;
            }
        } else {
            failure_amount++;
        }


        //free player struct
        free_player(&p);
    }

    //calculate total snakes/ladders hit
    int total_hits = 0;
    for (int i = 1; i < total_squares; i++) { //start square will never be a snake/ladder so i can start at 1
        if (snakes_hit[i] != 0) {
            total_hits += snakes_hit[i];
        }
    }

    //diplay statistics:
    printf("\nthe average amount of rolls to win was: %f\n", roll_average);
    printf("the shortest number of roles, by Player %d: %d\n", roll_min_id, roll_min);
    printf("Series: \n");
    for (int i = 0; i < roll_min; i++) {
        printf("roll %d: %d\n", i+1, roll_min_series[i]);
    }
    printf("Absolute and relative frequency of snakes and ladders hit: \n");
    for (int i = 1; i < total_squares; i++) {
        if (snakes_hit[i] != 0) {
            double relative = snakes_hit[i] / (float)total_hits;
            printf("Snake/ladder at %d hit: %d times, relative: %f\n", i, snakes_hit[i], relative);
        }
    }
    printf("Simulations failed: %d\n", failure_amount);
    
    //memory cleanup
    free_edges(gameboard, total_squares);
    free(roll_min_series);
    free(snakes_hit);

    //close file descriptor
    close(fd);

}
