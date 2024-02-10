#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 11
#define PUZZLE_SIZE 9

int sudoku[PUZZLE_SIZE][PUZZLE_SIZE];
int validation[NUM_THREADS] = {0}; // Array to hold results from threads

typedef struct {
    int row;
    int col;
} params_t;

// Validates a single row
void* validate_row(void* param) {
    params_t* params = (params_t*)param;
    int row = params->row;
    int flag[PUZZLE_SIZE + 1] = {0}; // Tracker for digits 1-9

    for (int i = 0; i < PUZZLE_SIZE; i++) {
        int num = sudoku[row][i];
        if (flag[num] == 1) {
            return NULL; // Duplicate found
        }
        flag[num] = 1;
    }
    validation[0] = 1; // Indicates row is valid
    return NULL;
}

// Validates a single column
void* validate_col(void* param) {
    params_t* params = (params_t*)param;
    int col = params->col;
    int flag[PUZZLE_SIZE + 1] = {0};

    for (int i = 0; i < PUZZLE_SIZE; i++) {
        int num = sudoku[i][col];
        if (flag[num] == 1) {
            return NULL;
        }
        flag[num] = 1;
    }
    validation[1] = 1; // Indicates column is valid
    return NULL;
}

// Validates a single 3x3 subgrid
void* validate_subgrid(void* param) {
    params_t* params = (params_t*)param;
    int rowStart = params->row;
    int colStart = params->col;
    int flag[PUZZLE_SIZE + 1] = {0};

    for (int i = rowStart; i < rowStart + 3; i++) {
        for (int j = colStart; j < colStart + 3; j++) {
            int num = sudoku[i][j];
            if (flag[num] == 1) {
                return NULL;
            }
            flag[num] = 1;
        }
    }
    validation[rowStart + colStart/3 + 2] = 1; // Adjusted for thread indexing
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <sudoku_puzzle_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    // Load Sudoku puzzle
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        for (int j = 0; j < PUZZLE_SIZE; j++) {
            fscanf(file, "%d", &sudoku[i][j]);
        }
    }
    fclose(file);

    pthread_t threads[NUM_THREADS];
    params_t params[PUZZLE_SIZE]; // Params for threads

    // Create a thread for validating each row and column
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        params[i].row = i;
        params[i].col = i;
        if (i == 0) {
            pthread_create(&threads[i], NULL, validate_row, &params[i]);
            pthread_create(&threads[i+1], NULL, validate_col, &params[i]);
        }
        // Create threads for subgrids
        if (i % 3 == 0) {
            for (int j = 0; j < PUZZLE_SIZE; j += 3) {
                params_t* subgrid_param = (params_t*)malloc(sizeof(params_t));
                subgrid_param->row = i;
                subgrid_param->col = j;
                pthread_create(&threads[2 + i + j/3], NULL, validate_subgrid, subgrid_param);
            }
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check results
    for (int i = 0; i < NUM_THREADS; i++) {
        if (validation[i] == 0) {
            printf("not valid\n");
            return 0;
        }
    }
    printf("valid\n");
    return 0;
}

