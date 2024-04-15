#include <stdio.h>    /* for printf statements when debugging */
#include <stdlib.h>   /* for malloc() and free() */
#include "mem.h"

/*
  Physical memory array. This is a static global array for all
  functions in this file.  An element in the array with a value of
  zero represents a free unit of memory.
*/
static dur_t* memory;

/*
 The size (i.e. number of units) of the physical memory array. This is
 a static global variable used by functions in this file.
 */
static int mem_size;


/*
 The last_placement_position variable contains the end position of the
 last allocated unit used by the next fit placement algorithm. Uncomment if using NEXTFIT
 */
static int last_placement_position;

/*
  Using the memory placement algorithm, strategy, allocate size
  units of memory that will reside in memory for duration time units.

  If successful, this function returns the number of contiguous blocks
  (a block is a contiguous "chuck" of units) of free memory probed while 
  searching for a suitable block of memory according to the placement 
  strategy specified.  If unsuccessful, return -1.

  If a suitable contiguous block of memory is found, the first size
  units of this block must be set to the value, duration.
 */

int mem_allocate(mem_strats_t strategy, int size, dur_t duration) {
    int start = last_placement_position;
    int probes = 0;

    if (strategy == NEXTFIT) {
        do {
            if (memory[start] == 0) {  // Found free memory
                int j;
                for (j = start; j < start + size && j < mem_size; j++) {
                    if (memory[j] != 0) break;
                }
                if (j == start + size) {  // Suitable block found
                    for (j = start; j < start + size; j++) memory[j] = duration;
                    last_placement_position = (j == mem_size) ? 0 : j;  // Update the last placement
                    return probes;
                }
            }
            probes++;
            start = (start + 1) % mem_size;  // Wrap around
        } while (start != last_placement_position);  // Stop if we've checked the whole memory
    }   
    if (strategy == BESTFIT) {
        int best_start = -1;
        int min_size = mem_size + 1;  // Larger than any possible block
        int current_start = -1;

        for (int i = 0; i < mem_size; i++) {
            if (memory[i] == 0) {
                if (current_start == -1) current_start = i;  // Mark the start of a free block
            } else if (current_start != -1) {
                // End of a free block found
                int block_size = i - current_start;
                if (block_size >= size && block_size < min_size) {
                    best_start = current_start;
                    min_size = block_size;
                }
                current_start = -1;  // Reset start marker
            }
            probes++;
        }

        // Check last block if it ends at the end of memory
        if (current_start != -1) {
            int block_size = mem_size - current_start;
            if (block_size >= size && block_size < min_size) {
                best_start = current_start;
                min_size = block_size;
            }
        }

        if (best_start != -1) {
            for (int j = best_start; j < best_start + size; j++) {
                memory[j] = duration;
            }
            return probes;
        }
    } else if (strategy == FIRSTFIT) {
        // Implement FIRSTFIT logic here, if still needed
        for (int i = 0; i < mem_size; i++) {
            if (memory[i] == 0) { // Found free memory
                int j;
                for (j = i; j < i + size; j++) {
                    if (j >= mem_size || memory[j] != 0) {
                        break;
                    }
                }
                if (j == i + size) {  // Suitable block found
                    for (j = i; j < i + size; j++) memory[j] = duration;
                    return probes;
                }
            }
            probes++;
        }
    }

    return -1;  // No suitable block found
}








// int mem_allocate(mem_strats_t strategy, int size, dur_t duration) {
//     int i, probes = 0;
//     if (strategy == FIRSTFIT) {
//         for (i = 0; i < mem_size; i++) {
//             if (memory[i] == 0) { // Found free memory
//                 int j, fit = 1;
//                 for (j = i; j < i + size; j++) {
//                     if (j >= mem_size || memory[j] != 0) {
//                         fit = 0;
//                         break;
//                     }
//                 }
//                 if (fit) {
//                     for (j = i; j < i + size; j++) memory[j] = duration;
//                     return probes;
//                 }
//             }
//             probes++;
//         }
//     }
//     return -1;
// }

/*
  Go through all of memory and decrement all positive-valued entries.
  This simulates one unit of time having transpired.  NOTE: when a
  memory cell is decremented to zero, it becomes "unallocated".
 */
int mem_single_time_unit_transpired() {
    int i, freed_blocks = 0;
    for (i = 0; i < mem_size; i++) {
        if (memory[i] > 0) {
            memory[i]--;
            if (memory[i] == 0) {
                freed_blocks++;
                // Further logic if needed to handle larger freed blocks
            }
        }
    }
    return freed_blocks;  // Return the number of blocks that became free
}


/*
  Return the number of fragments in memory.  A fragment is a
  contiguous free block of memory of size less than or equal to
  frag_size.
 */
int mem_fragment_count(int frag_size) {
    int i = 0, fragment_count = 0;

    while (i < mem_size) {
        if (memory[i] == 0) { // start of a free block
            int block_size = 0;
            while (i < mem_size && memory[i] == 0) {
                block_size++;
                i++;
            }
            if (block_size <= frag_size) {
                fragment_count++;
            }
        } else {
            i++;
        }
    }
    return fragment_count;  // Ensure that this function always returns a value.
}


/*
  Set the value of zero to all entries of memory.
 */
void mem_clear() {
    for (int i = 0; i < mem_size; i++) {
        memory[i] = 0;
    }
}

/*
 Allocate physical memory to size. This function should 
 only be called once near the beginning of your main function.
 */
void mem_init(int size)
{
  memory = malloc(sizeof(dur_t)*size);
  mem_size = size;
}

/*
 Deallocate physical memory. This function should 
 only be called once near the end of your main function.
 */
void mem_free()
{
  free(memory);
}

/*
  Print memory for testing/debugging purposes.  This is an optional
  routine to write, but highly recommended!  You will need to test and
  debug your allocation algorithms.  Calling this routine in your
  main() after every allocation (successful or not) will help in this
  endeavor.  NOTE: you should print the memory contents in contiguous
  blocks, rather than single units; otherwise, the output will be very
  long.
 */
void mem_print() {
    printf("Memory Dump:\n");
    for (int i = 0; i < mem_size; ) {
        int current_value = memory[i];
        int start = i;
        while (i < mem_size && memory[i] == current_value) {
            i++;
        }
        printf("Block from %d to %d -> Value: %d, Size: %d\n", start, i - 1, current_value, i - start);
    }
    printf("\n");
}
