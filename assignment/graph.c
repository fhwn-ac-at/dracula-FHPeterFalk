#include "graph.h"
#include <stdio.h>
#include <stdlib.h>

int* create_edge_list (int position, int dice_max, int totalsize, bool exact) {
    int* edge_list = malloc(sizeof(int) * dice_max);
    if (!edge_list) {
        fprintf(stderr, "malloc failure! terminating...\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < dice_max; i++) {
        int dest = position + i + 1;
        if (dest > totalsize) {
            if (exact) {
                dest = position;
            } else {
                dest = totalsize;
            }
        }
        edge_list[i] = dest;
    }
    return edge_list;
}

void create_gameboard(Node *board, int n, int m, bool exact, int dice_max, FILE *f) {
    bool reading_backwards = false;
    char c;  // Current character read from file
    int val;    // Parsed integer value from file
    char* endptr;   
    int col_pos = 0;    // Current column (within the row)
    int row_pos = m-1;  // Current row; start from the top row (file's first row is board's top)
    int arr_pos;    // Calculated index in the board array
    
    // If the board height is odd, start with reading_backwards=true to match serpentine layout
    if (m%2 != 0) { reading_backwards = true;}
    
    while ((c = fgetc(f)) != EOF) {
        // Too many rows
        if (row_pos < 0) {
            fprintf(stderr, "board file contains too many rows!\n");
            exit(EXIT_FAILURE);
        }
        
        // Alternate the direction (serpentine) for each row
        reading_backwards = !reading_backwards;

        // Parse each row until newline
        while (c != '\n' && c != EOF) {
            //buffer and buffer-index reset after every read value
            char number_buffer[11] = {0}; // Holds up to 10-digit numbers + null terminator
            int buffer_pos = 0;             
            
            // Too many columns in this row
            if (col_pos >= n) {
                fprintf(stderr, "board file contains too many columns in row: %d!\n", m-row_pos);
            }
            // Parse the next number; seperated by ','
            while (c != ',' && c != '\n' && c != EOF) {
                number_buffer[buffer_pos] = c; // Read digits of the number
                buffer_pos++;
                c = fgetc(f);
            }
            number_buffer[buffer_pos] = '\0'; // Null-terminate the string
            
            // Convert string to integer (with error checking)
            val = strtol(number_buffer, &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr, "Invalid value in board file! col: %d row: %d number_buffer: %s\n", col_pos+1, row_pos+1, number_buffer);
                exit(EXIT_FAILURE);
            }
            // Calculate linear board array index for current square, considering direction
            if (reading_backwards) {
                // Right-to-left fill
                arr_pos = row_pos * n + (n - 1 - col_pos) + 1;
            } else {
                // Left-to-right fill
                arr_pos = row_pos * n + col_pos + 1;
            }
            
            //fill corresponding Node struct:
            board[arr_pos].id = arr_pos;
            board[arr_pos].edges = create_edge_list(arr_pos, dice_max, n*m, exact);
            
            // Assign snake/ladder info to this square
            if (val) {
                board[arr_pos].snake.is_true = true;
                board[arr_pos].snake.to_where = val;
            } else {
                board[arr_pos].snake.is_true = false;
                board[arr_pos].snake.to_where = -1;                
            }
            
            // Move to next character (should be start of next number or a newline)
            c = fgetc(f);
            col_pos++;
        }
        
        // At end of row: Check if row had correct number of columns
        if (col_pos != n) {
            fprintf(stderr, "Wrong amount of columns in row %d!\n", row_pos+1);
            exit(EXIT_FAILURE);
        }
        // Prepare for next row
        row_pos--;
        col_pos = 0;
    }
    // At end of file: Check if total rows was correct
    if (row_pos != -1) {
        fprintf(stderr, "wrong amount of rows in file!\n");
        exit(EXIT_FAILURE);
    }

    //assign start square:
    board[0].id = 0;
    board[0].edges = create_edge_list(0, dice_max, n*m, exact);
    board[0].snake.is_true = false;
    board[0].snake.to_where = -1;
}

void print_board(Node* board, int n, int m) {
    printf("\nThe used gameboard is:\n");

    for (int row = m - 1; row >= 0; row--) { // Print from top row to bottom
        if (row % 2 == 0) { // Left to right
            for (int col = 0; col < n; col++) {
                int arr_pos = row * n + col + 1; // +1 for dummy
                if (board[arr_pos].snake.is_true)
                    printf("[%3d->%3d] ", board[arr_pos].id, board[arr_pos].snake.to_where);
                else
                    printf("[%3d     ] ", board[arr_pos].id);
            }
        } else { // Right to left
            for (int col = n - 1; col >= 0; col--) {
                int arr_pos = row * n + col + 1;
                if (board[arr_pos].snake.is_true)
                    printf("[%3d->%3d] ", board[arr_pos].id, board[arr_pos].snake.to_where);
                else
                    printf("[%3d     ] ", board[arr_pos].id);
            }
        }
        printf("\n");
    }
    printf("[Start ^]");
    printf("\n");
}

void validate_board(Node* board, int n, int m) {
    int total = n * m;
    // Create an array to count how many times each square is a landing
    int* landing_count = calloc(total + 1, sizeof(int)); // +1 for 1-based indexing

    for (int i = 1; i <= total; i++) {
        if (board[i].snake.is_true) {
            int dest = board[i].snake.to_where;

            //Destination must be on board and not self
            if (dest < 1 || dest > total || dest == i) {
                fprintf(stderr, "Invalid snake/ladder from %d to %d!\n", i, dest);
                exit(EXIT_FAILURE);
            }

            //Can't start a snake/ladder on a landing field
            if (board[dest].snake.is_true) {
                fprintf(stderr, "Square %d is both a landing and a start of a snake/ladder!\n", dest);
                exit(EXIT_FAILURE);
            }

            //Count how many times each square is a landing; so there are no snakes/ladders landing on the same field
            landing_count[dest]++;
            if (landing_count[dest] > 1) {
                fprintf(stderr, "Square %d is the landing field of more than one snake/ladder!\n", dest);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (board[total].snake.is_true) {
        fprintf(stderr, "final square cannot be the start of a snake/ladder!\n");
        exit(EXIT_FAILURE);
    }

    free(landing_count);

    //printf("gameboard is valid!\n");

}

void free_edges(Node* board, int totalsize) {
    for (int i = 0; i<totalsize; i++) {
        free(board[i].edges);
    }
}