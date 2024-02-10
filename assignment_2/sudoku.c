#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 9
#define UNASSIGNED 0

// Function declarations
int isSafe(int grid[N][N], int row, int col, int num);
int findUnassignedLocation(int grid[N][N], int *row, int *col);
int solveSudoku(int grid[N][N]);

// Helper function to print grid
void printGrid(int grid[N][N]) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            printf("%2d", grid[row][col]);
        }
        printf("\n");
    }
}

// Check if it will be legal to assign num to the given row, col
int isSafe(int grid[N][N], int row, int col, int num) {
    // Row has the unique (row-clash)
    for (int x = 0; x <= 8; x++)
        if (grid[row][x] == num)
            return 0;

    // Column has the unique numbers (column-clash)
    for (int x = 0; x <= 8; x++)
        if (grid[x][col] == num)
            return 0;

    // Corresponding square has unique numbers (box-clash)
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num)
                return 0;

    return 1;
}

// Searches the grid to find an entry that is still unassigned
int findUnassignedLocation(int grid[N][N], int *row, int *col) {
    for (*row = 0; *row < N; *row++)
        for (*col = 0; *col < N; *col++)
            if (grid[*row][*col] == UNASSIGNED)
                return 1;
    return 0;
}

// Takes a partially filled-in grid and attempts to assign values to all unassigned locations
int solveSudoku(int grid[N][N]) {
    int row, col;

    // If there is no unassigned location, we are done
    if (!findUnassignedLocation(grid, &row, &col))
        return 1; // success!

    // Consider digits 1 to 9
    for (int num = 1; num <= 9; num++) {
        // If looks promising
        if (isSafe(grid, row, col, num)) {
            // Make tentative assignment
            grid[row][col] = num;

            // Return, if success
            if (solveSudoku(grid))
                return 1;

            // Failure, unmake & try again
            grid[row][col] = UNASSIGNED;
        }
    }
    return 0; // This triggers backtracking
}

int main() {
    // 0 means unassigned cells
    int grid[N][N] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0, 0}};

    srand(time(NULL)); // Initialize random seed

    // Fill the diagonal of SRN x SRN matrices
    for (int i = 0; i < N; i += 3) {
        for (int row = i; row < i + 3; row++) {
            for (int col = i; col < i + 3; col++) {
                int num;
                do {
                    num = rand() % 9 + 1;
                } while (!isSafe(grid, row, col, num));
                grid[row][col] = num;
            }
        }
    }

    // Attempt to solve the puzzle
    if (solveSudoku(grid) == 1)
        printGrid(grid);
    else
        printf("No solution exists");

    return 0;
}
