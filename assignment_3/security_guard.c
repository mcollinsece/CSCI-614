#include <stdio.h>
#include <stdlib.h>  // for exit(), rand(), strtol()
#include <pthread.h>
#include <time.h>    // for nanosleep()
#include <errno.h>   // for EINTR error check in millisleep()

#include "binary_semaphore.h"

#define MIN_SLEEP      20   // minimum sleep time in milliseconds
#define MAX_SLEEP     100   // maximum sleep time in milliseconds
#define START_SEED     11   // arbitrary value to seed random number generator

int guard_state;         // waiting, in the hall, or in the room
int num_students;        // number of students in the room

binary_semaphore mutex;  // to protect shared variables
binary_semaphore room_empty; // to signal that the room is empty
binary_semaphore room_not_full; // to signal that the room is not full

unsigned int *seeds;     // rand seeds for guard and students generating delays

int capacity;       // maximum number of students in a room
int num_checks;     // number of checks the guard makes

void millisleep(long millisecs)
{
  struct timespec req;
  req.tv_sec  = millisecs / 1000;
  millisecs -= req.tv_sec * 1000;
  req.tv_nsec = millisecs * 1000000;
  while(nanosleep(&req, &req) == -1 && errno == EINTR)
    ;
}

int rand_range(unsigned int *seedptr, long min, long max)
{
  return min + rand_r(seedptr) % (max - min + 1);
}

void study(long id)
{
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("student %2ld studying in room with %2d students for %3d millisecs\n",
     id, num_students, ms);
  millisleep(ms);
}

void do_something_else(long id)
{
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}

void assess_security()
{
  guard_state = 1;
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard assessing room security for %3d millisecs...\n", ms);
  millisleep(ms);
  printf("\tguard done assessing room security\n");
}

void guard_walk_hallway()
{
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard walking the hallway for %3d millisecs...\n", ms);
  millisleep(ms);
}

void guard_check_room() {
    semWaitB(&mutex);
    if (num_students > 0) {
        printf("\tguard waiting to enter room with %d students...\n", num_students);
        guard_state = -1;
        semSignalB(&mutex);
        semWaitB(&room_empty);
        semWaitB(&mutex);
    }
    printf("\tguard assessing room security for %d millisecs...\n", rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP));
    guard_state = 0;
    printf("\tguard done assessing room security\n");
    printf("\tguard left room\n");
    semSignalB(&mutex);
    semSignalB(&room_not_full);
    printf("\tguard walking the hallway for %d millisecs...\n", rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP));
}

void student_study_in_room(long id) {
    semWaitB(&mutex);
    while (guard_state != 0 || num_students >= capacity) {
        semSignalB(&mutex);
        semWaitB(&room_not_full);
        semWaitB(&mutex);
    }
    num_students++;
    printf("student %ld entering room, total now %d\n", id, num_students);
    semSignalB(&mutex);

    printf("student %ld studying in room with %d students for %d millisecs\n", id, num_students, rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP));

    semWaitB(&mutex);
    num_students--;
    printf("student %ld left room, total now %d\n", id, num_students);
    if (num_students == 0 && guard_state == -1) {
        semSignalB(&room_empty);
    }
    semSignalB(&mutex);
    if (num_students < capacity) {
        semSignalB(&room_not_full);
    }
}

void* guard(void* arg)
{
  int i;
  srand(seeds[0]);

  for (i = 0; i < num_checks; i++) {
    guard_check_room();
    guard_walk_hallway();
  }

  return NULL;
}

void* student(void* arg)
{
  long id = (long) arg;
  srand(seeds[id]);

  while (1) {
    student_study_in_room(id);
    do_something_else(id);
  }

  return NULL;
}

int main(int argc, char** argv)
{
  int n;
  pthread_t  cthread;
  pthread_t* sthreads;
  long i;

  if (argc < 4) {
    fprintf(stderr, "USAGE: %s num_threads capacity num_checks\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);
  capacity = atoi(argv[2]);
  num_checks = atoi(argv[3]);

  seeds = malloc((n + 1) * sizeof(unsigned int));
  sthreads = malloc(n * sizeof(pthread_t));

  guard_state = 0;
  num_students = 0;

  semInitB(&mutex, 1);
  semInitB(&room_empty, 0);
  semInitB(&room_not_full, 1);

  seeds[0] = START_SEED;
  pthread_create(&cthread, NULL, guard, (void*) NULL);

  for (i = 1; i <= n; i++) {
    seeds[i] = START_SEED + i; // Different seed for each thread
    pthread_create(&sthreads[i-1], NULL, student, (void*) i);
  }

  pthread_join(cthread, NULL);

  for (i = 0; i < n; i++) {
    pthread_cancel(sthreads[i]);
  }

  free(seeds);
  free(sthreads);

  return 0;
}
