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

int mem_allocate(mem_strats_t strategy, int size, dur_t duration) {
    int probes = 0;
    if (strategy == FIRSTFIT) {
        for (int i = 0; i < mem_size; i++) {
            probes++; // Increment probes for each memory check.
            if (memory[i] == 0) { // Start of a potential block
                int freeCount = 0;
                while (i + freeCount < mem_size && memory[i + freeCount] == 0 && freeCount < size) {
                    freeCount++;
                }
                if (freeCount == size) { // Found a block of sufficient size
                    for (int j = 0; j < size; j++) {
                        memory[i + j] = duration; // Allocate the block
                    }
                    //printf("Probes: %i\n", probes);
                    return probes; // Return the number of probes used to find the block
                }
                i += freeCount; // Skip the checked free block
            }
        }
        return -1; // No suitable block found, return -1
    }
    if (strategy == NEXTFIT){
      int start = last_placement_position;
      do {
          probes++; // Increment probes for each memory check.
          if (memory[start] == 0) { // Start of a potential block
              int freeCount = 0;
              while (start + freeCount < mem_size && memory[start + freeCount] == 0 && freeCount < size) {
                  freeCount++;
              }
              if (freeCount == size) { // Found a block of sufficient size
                  for (int j = 0; j < size; j++) {
                      memory[start + j] = duration; // Allocate the block
                  }
                  last_placement_position = (start + size) % mem_size; // Update last placement
                  return probes; // Return the number of probes used to find the block
              }
              start += freeCount; // Skip the checked free block
          }
          start = (start + 1) % mem_size; // Wrap around at the end of the memory
      } while (start != last_placement_position); // Stop if we loop back to the initial position

      return -1; // No suitable block found, return -1
    } else if (strategy == BESTFIT) {
        int bestIndex = -1;
        int minSize = mem_size + 1; // Initialize to larger than any possible block

        for (int i = 0; i < mem_size; i++) {
            probes++; // Increment probes for each memory check.
            if (memory[i] == 0) { // Start of a potential block
                int freeCount = 0;
                while (i + freeCount < mem_size && memory[i + freeCount] == 0) {
                    freeCount++;
                }
                if (freeCount >= size && freeCount < minSize) { // Smaller but fitting block found
                    bestIndex = i;
                    minSize = freeCount;
                }
                i += freeCount; // Skip the checked free block
            }
        }

        if (bestIndex != -1) { // A suitable block has been found
            for (int j = 0; j < size; j++) {
                memory[bestIndex + j] = duration; // Allocate the block
            }
            return probes; // Return the number of probes used to find the block
        }
        
        return -1; // No suitable block found, return -1
    }
    return -1; // If not FIRSTFIT or no other strategies are defined
}


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
