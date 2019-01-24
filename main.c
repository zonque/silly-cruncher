#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

#define ITERATIONS 100
static unsigned long latency[ITERATIONS] = {};

#define BUFSIZE (1024 * 1024)
static char *scratch_a;
static char *scratch_b;
static char *scratch_c;

void compute(int complexity) {
  int i, n;

  for (n = 0; n < complexity; n++) {
    for (i =  0; i < BUFSIZE; i++)
      scratch_a[i] = scratch_b[i] * scratch_c[i];
  }
}

void timespec_diff(const struct timespec *start,
                   const struct timespec *stop,
                   struct timespec *result)
{
  if ((stop->tv_nsec - start->tv_nsec) < 0) {
    result->tv_sec = stop->tv_sec - start->tv_sec - 1;
    result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000UL;
  } else {
    result->tv_sec = stop->tv_sec - start->tv_sec;
    result->tv_nsec = stop->tv_nsec - start->tv_nsec;
  }
}

int main(int argc, char **argv) {
  int i, complexity = 100;
  unsigned long highest = 0;
  unsigned long lowest = ULONG_MAX;
  unsigned long total = 0;

  scratch_a = malloc(BUFSIZE);
  scratch_b = malloc(BUFSIZE);
  scratch_c = malloc(BUFSIZE);

  for (i =  0; i < BUFSIZE; i++) {
    scratch_a[i] = random();
    scratch_b[i] = random();
    scratch_c[i] = random();
  }

  if (scratch_a == NULL || scratch_a == NULL || scratch_c == NULL) {
    printf("Cannot allocate memory.\n");
    return -1;
  }

  if (argc > 1)
    complexity = strtol(argv[1], 0, 0);

  for (i = 0; i < ITERATIONS; i++) {
    struct timespec before, after, delta;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &before);
    compute(complexity);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &after);

    timespec_diff(&before, &after, &delta);
    latency[i] = delta.tv_sec * 1000000000UL + delta.tv_nsec;
  }

  for (i = 0; i < ITERATIONS; i++) {
    if (latency[i] < lowest)
      lowest = latency[i];

    if (latency[i] > highest)
      highest = latency[i];

    total += latency[i];
  }

  printf("Complexity: %d, iterations: %d\n", complexity, ITERATIONS);
  printf("Highest: %lu\n", highest);
  printf("Lowest:  %lu\n", lowest);
  printf("Average: %lu\n", total / ITERATIONS);
  printf("Jitter:  %lu (%lu%%, %lu%%)\n", highest - lowest, (100 * highest) / (total / ITERATIONS), (100 * lowest) / (total / ITERATIONS));

  return 0;
}
