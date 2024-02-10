
# Sudoku Solution Validator

A Sudoku puzzle uses a 9 × 9 grid in which each column and row, as well as each of the nine 3 × 3 subgrids, must contain all of the digits 1 through 9. The figure below presents an example of a valid Sudoku puzzle. This project consists of designing a multithreaded application that determines whether the solution to a Sudoku puzzle is valid.

## 9x9 Sudoku Example Solution

There are several different ways of multithreading this application. One suggested strategy is to create threads that check the following criteria:

- A thread to check that each column contains the digits 1 through 9
- A thread to check that each row contains the digits 1 through 9
- Nine threads to check that each of the 3 × 3 subgrids contains the digits 1 through 9

This would result in a total of eleven separate threads for validating a Sudoku puzzle. However, you are welcome to create even more threads for this project. For example, rather than creating one thread that checks all nine columns, you could create nine separate threads and have each of them check one column.

### I. Passing Parameters to Each Thread

The parent thread will create the worker threads, passing each worker the location that it must check in the Sudoku grid. This step will require passing several parameters to each thread. The easiest approach is to create a data structure using a struct. For example, a structure to pass the row and column where a thread must begin validating would appear as follows:

```c
/* structure for passing data to threads */
typedef struct {
    int row;
    int col;
} params_t;
```

Both Pthreads and Windows programs will create worker threads using a strategy similar to that shown below:

```c
params_t *data = (params_t *) malloc(sizeof(params_t));
data->row = 1;
data->col = 1;
/* Now create the thread passing it data as a parameter */
```

The data pointer will be passed to either the `pthread_create()` (Pthreads) function or the `CreateThread()` (Windows) function, which in turn will pass it as a parameter to the function that is to run as a separate thread.

### II. Returning Results to the Parent Thread

Each worker thread is assigned the task of determining the validity of a particular region of the Sudoku puzzle. Once a worker has performed this check, it must pass its results back to the parent. One good way to handle this is to create an array of integer values that is visible to each thread. The ith index in this array corresponds to the ith worker thread. If a worker sets its corresponding value to 1, it is indicating that its region of the Sudoku puzzle is valid. A value of 0 indicates otherwise. When all worker threads have completed, the parent thread checks each entry in the result array to determine if the Sudoku puzzle is valid.

### III. Specifications and Other Solution Constraints

For this assignment, you will be reading in a solution candidate from a file. The file contains 9 lines (rows) with each line containing 9 digits in the range 1...9. Your program must take the name of the candidate solution on the command-line. The main thread will output either "valid" or "not valid", depending on the validity of the solution. You may write your solution using either the Windows threads API or POSIX threads (MacOS, Linux). This is a C programming assignment. You must write your own code for this. No synchronization is required other than the simple, join(). Your solution must contain at least 11 threads (as mentioned above), but could utilize more threads if you like.
