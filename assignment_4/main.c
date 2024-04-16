#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Usage: %s <memory size> <duration> <runs> <seed>\n", argv[0]);
        return 1;
    }

    int mem_size = atoi(argv[1]);
    int duration = atoi(argv[2]);
    int runs = atoi(argv[3]);
    int seed = atoi(argv[4]);

    srand(seed);
    mem_init(mem_size);

    // Loop over strategies
    printf("Strategy   | Average Failures | Average Fragments | Average Probes\n");
    for (int strategy = BESTFIT; strategy <= NEXTFIT; strategy++) {
        mem_strats_t current_strategy = (mem_strats_t)strategy; // Cast int to enum
        //printf("\nCurrent strategy: %d\n", current_strategy);

        char* strategy_string = "";
        if (current_strategy == 0) strategy_string = "Best Fit";
        if (current_strategy == 1) strategy_string = "First Fit";
        if (current_strategy == 2) strategy_string = "Next Fit";

        double total_failures = 0, total_fragments = 0, total_probes = 0;

        for (int run = 0; run < runs; run++) {
            mem_clear();
            int failures = 0, probes = 0;

            for (int time_unit = 0; time_unit < duration; time_unit++) {
                int size = MIN_REQUEST_SIZE + (rand() % (MAX_REQUEST_SIZE - MIN_REQUEST_SIZE + 1));
                dur_t alloc_duration = MIN_DURATION + (rand() % (MAX_DURATION - MIN_DURATION + 1));
                int result = mem_allocate(current_strategy, size, alloc_duration);

                if (result == -1) {
                    failures++;
                } else {
                    probes += result;
                }

                mem_single_time_unit_transpired();
            }

            total_failures += failures;
            total_fragments += mem_fragment_count(10);  // Example fragment size threshold
            total_probes += probes;
        }

        //printf("Strategy   | Average Failures | Average Fragments | Average Probes\n");
        printf("%-10s | %16.2f | %17.2f | %14.2f\n",
               strategy_string,
               total_failures / runs,
               total_fragments / runs,
               total_probes / duration / runs);
    }

    //printf("\nBESTFIT = 0, FIRSTFIT = 1, NEXTFIT = 2\n");
    mem_free();
    return 0;
}
