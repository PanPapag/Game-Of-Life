#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common_functions.h"

// load a grid form a file
char** load_from_file(FILE* input) {
    // the input file should be as following:
    // 1st line: size (n * n)
    // each line after: n chars (followed by space), 0->dead 1->alive
    char* first_line = malloc(2 * sizeof(char));
    fscanf(input, "%s\n", first_line);
    int size = atoi(first_line);
    char** grid = malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++) {
        grid[i] = malloc(size * sizeof(char));
        for (int j = 0; j < size; j++) {
            char current;
            // read each character from the file, and assign it to the grid
            current = getc(input);
            printf("%c\n", current);
            grid[i][j] = current;
        }
        // read the \n
        getc(input);
        printf("\n");
    }
    return grid;
}

char** evolution(char** old_grid, int size) {
    char** new_grid = malloc(size * sizeof(char*));
    for (int i = 0; i < size; i++) {
        new_grid[i] = malloc(size * sizeof(char));
        for (int j = 0; j < size; j++) {
            // compute how many organizations exist near the cell we are about to evolve
            int alive_neighbours = (old_grid[i-1][j-1] == '1') + (old_grid[i-1][j] == '1') + (old_grid[i-1][j+1] == '1') +
                                    (old_grid[i][j-1] == '1') + (old_grid[i][j+1] == '1') +
                                    (old_grid[i+1][j-1] == '1') +(old_grid[i+1][j] == '1') +(old_grid[i+1][j+1] == '1');
            // rules regarding an alive cell
            if (old_grid[i][j] == '1') {
                // 0 or 1 neighbours -> the cell dies
                if (alive_neighbours < 2) {
                    new_grid[i][j] = '0';
                }
                // 2 or 3 neighbours -> the cell survives
                else if (alive_neighbours < 4) {
                    new_grid[i][j] = '1';
                }
                // more than 4 neighbours -> the cell dies due to overpopulation
                else {
                    new_grid[i][j] = '0';
                }
            }
            // rules regarding dead cells
            else {
                // exactly 3 neighbours -> a new cell is born
                if (alive_neighbours == 3) {
                    new_grid[i][j] = '1';
                }
            }
        }
        free(old_grid[i]);
    }
    free(old_grid);

    return new_grid;
}
